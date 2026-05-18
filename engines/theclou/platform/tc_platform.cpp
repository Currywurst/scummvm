/*
 * tc_platform.cpp  —  ScummVM OSystem implementation of the tc_* platform
 * functions declared in tc_sdl_compat.h.
 *
 * Graphics  → g_system->copyRectToScreen / updateScreen / setPalette
 * Input     → g_system->getEventManager()->pollEvent()
 * Timing    → g_system->getMillis() / delayMillis()
 * Audio     → TheClouAudioStream → Audio::Mixer (ring-buffer pull model)
 * Threading → Common::Mutex + POSIX/Win32 thread wrapper
 * Quit      → tc_QuitGame() in base.c via longjmp; we just set a flag
 * Pause     → g_tcPaused flag; tc_Delay spin-waits while paused
 */

/* ---- System headers FIRST (before ScummVM's forbidden.h macros) ---- */
#include <stdlib.h>
#include <string.h>
#if defined(POSIX)
#include <pthread.h>
#include <unistd.h>   /* usleep() */
#endif
#if defined(WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

/* ---- ScummVM headers ---- */
#define FORBIDDEN_SYMBOL_EXCEPTION_printf
#define FORBIDDEN_SYMBOL_EXCEPTION_FILE

#include "common/scummsys.h"
#include "common/system.h"
#include "common/events.h"
#include "common/mutex.h"
#include "common/debug.h"
#include "common/fs.h"
#include "common/stream.h"
#include "common/endian.h"
#include "common/ptr.h"
#include "graphics/paletteman.h"
#include "audio/mixer.h"
#include "audio/audiostream.h"

/* tc_sdl_compat.h must be included AFTER ScummVM headers so the SDL-type
 * #defines don't corrupt ScummVM's own declarations.                     */
#include "theclou/platform/tc_sdl_compat.h"

/* ================================================================== */
/* Forward declarations                                                */
/* ================================================================== */

struct SDL_AudioStream;   /* full definition below */

/* ================================================================== */
/* TheClouAudioStream — bridges the ring buffer to ScummVM's mixer.   */
/*                                                                     */
/* readBuffer() is called by the mixer on its audio thread.           */
/* tc_PutAudioStreamData() is called by the game's audio thread.      */
/* SDL_AudioStream::mutex protects the shared ring-buffer state.      */
/* ================================================================== */

class TheClouAudioStream : public Audio::AudioStream {
public:
    explicit TheClouAudioStream(SDL_AudioStream *rb)
        : _rb(rb), _active(true) {}

    void deactivate() { _active = false; }

    /* readBuffer is implemented after SDL_AudioStream is fully defined */
    int  readBuffer(int16 *buf, const int numSamples) override;
    bool isStereo()  const override { return false; }
    int  getRate()   const override { return 22050; }
    bool endOfData() const override { return !_active; }

private:
    SDL_AudioStream *_rb;
    volatile bool    _active;
};

/* ================================================================== */
/* Internal structs for opaque handle types                            */
/* ================================================================== */

struct SDL_Window   { int dummy; };
struct SDL_Renderer { int dummy; };
struct SDL_Texture  { int dummy; };
struct SDL_Joystick { int dummy; };

/* Ring-buffer used as SDL_AudioStream replacement.
 * mutex protects read_pos / write_pos / available.                   */
struct SDL_AudioStream {
    uint8_t  *buf;
    int       size;
    int       read_pos;
    int       write_pos;
    int       available;

    Common::Mutex      *mutex;       /* protects ring-buffer fields   */
    TheClouAudioStream *scummStream; /* ScummVM pulls audio from here  */
    Audio::SoundHandle  handle;      /* mixer handle for pause/stop    */
};

/* Thin mutex wrapper around Common::Mutex */
struct SDL_Mutex {
    Common::Mutex *m;
};

/* Thin thread wrapper */
struct SDL_Thread {
    int (*fn)(void *);
    void *data;
    int   result;
#if defined(POSIX)
    pthread_t thread;
#elif defined(WIN32)
    HANDLE thread;
#else
    bool started;
#endif
};

/* ================================================================== */
/* TheClouAudioStream::readBuffer — after SDL_AudioStream is defined   */
/* ================================================================== */

int TheClouAudioStream::readBuffer(int16 *buf, const int numSamples) {
    if (!_rb || !_active) {
        memset(buf, 0, (size_t)numSamples * sizeof(int16));
        return numSamples;
    }
    Common::StackLock lock(*_rb->mutex);

    int bytesNeeded = numSamples * (int)sizeof(int16);
    int toCopy      = (_rb->available < bytesNeeded)
                      ? _rb->available : bytesNeeded;

    uint8_t *dst = reinterpret_cast<uint8_t *>(buf);
    for (int i = 0; i < toCopy; i++) {
        dst[i]      = _rb->buf[_rb->read_pos];
        _rb->read_pos = (_rb->read_pos + 1) % _rb->size;
    }
    _rb->available -= toCopy;

    /* Fill any deficit with silence */
    if (toCopy < bytesNeeded)
        memset(dst + toCopy, 0, (size_t)(bytesNeeded - toCopy));

    return numSamples;
}

/* ================================================================== */
/* Global state                                                        */
/* ================================================================== */

static const char   *s_lastError    = "none";
static volatile int  g_tcPaused     = 0;   /* set by tc_SetPaused()  */

/* Main-thread ID — set once by tc_InitPlatform() before the game loop.
 * Used in tc_Delay() to distinguish the main game thread (which may pump
 * events) from background threads like the audio mix thread (which must
 * NOT touch the event manager or call tc_QuitGame / longjmp).          */
#if defined(POSIX)
static pthread_t     s_mainThread;
static volatile int  s_mainThreadSet = 0;
#elif defined(WIN32)
static DWORD         s_mainThreadId   = 0;
#endif

/* ================================================================== */
/* Platform initialisation — call once from the main engine thread    */
/* before theclou_run().  Records the thread ID so tc_Delay() can    */
/* distinguish the main game thread from the audio mix thread.        */
/* ================================================================== */

extern "C" void tc_InitPlatform(void) {
#if defined(POSIX)
    s_mainThread    = pthread_self();
    s_mainThreadSet = 1;
#elif defined(WIN32)
    s_mainThreadId  = GetCurrentThreadId();
#endif
}

/* Helper: true when called on the thread that called tc_InitPlatform */
static bool onMainThread(void) {
#if defined(POSIX)
    return s_mainThreadSet && pthread_equal(pthread_self(), s_mainThread);
#elif defined(WIN32)
    return s_mainThreadId != 0 && GetCurrentThreadId() == s_mainThreadId;
#else
    return true;   /* single-threaded platform: always treat as main  */
#endif
}

/* ================================================================== */
/* Error                                                               */
/* ================================================================== */

const char *tc_GetError(void) { return s_lastError; }

/* ================================================================== */
/* Surface management                                                  */
/* ================================================================== */

SDL_Surface *tc_CreateSurface(int w, int h, Uint32 /*format*/) {
    SDL_Surface *s = (SDL_Surface *)calloc(1, sizeof(SDL_Surface));
    if (!s) return nullptr;
    s->w      = w;
    s->h      = h;
    s->pitch  = w; /* 8-bit: 1 byte/pixel */
    s->pixels = (Uint8 *)calloc(1, (size_t)(w * h));
    return s;
}

void tc_DestroySurface(SDL_Surface *s) {
    if (!s) return;
    if (s->_pal) {
        if (s->_pal->_owned)
            free(s->_pal->colors);
        free(s->_pal);
    }
    free(s->pixels);
    free(s);
}

SDL_Palette *tc_GetSurfacePalette(SDL_Surface *s) {
    return s ? s->_pal : nullptr;
}

SDL_Palette *tc_CreateSurfacePalette(SDL_Surface *s) {
    if (!s) return nullptr;
    if (!s->_pal) {
        s->_pal = (SDL_Palette *)calloc(1, sizeof(SDL_Palette));
        if (!s->_pal) return nullptr;
        s->_pal->ncolors = 256;
        s->_pal->colors  = (SDL_Color *)calloc(256, sizeof(SDL_Color));
        s->_pal->_owned  = 1;
    }
    return s->_pal;
}

bool tc_SetPaletteColors(SDL_Palette *pal, const SDL_Color *colors,
                          int first, int ncolors) {
    if (!pal || !colors) return false;
    if (pal->colors && first + ncolors <= pal->ncolors)
        memcpy(pal->colors + first, colors,
               (size_t)ncolors * sizeof(SDL_Color));

    if (g_system) {
        byte buf[256 * 3];
        for (int i = 0; i < ncolors; i++) {
            buf[i*3+0] = colors[i].r;
            buf[i*3+1] = colors[i].g;
            buf[i*3+2] = colors[i].b;
        }
        g_system->getPaletteManager()->setPalette(buf,
                                                  (uint)first, (uint)ncolors);
    }
    return true;
}

void tc_FillSurfaceRect(SDL_Surface *dst, const SDL_Rect *rect, Uint32 color) {
    if (!dst || !dst->pixels) return;
    int x0 = rect ? rect->x : 0;
    int y0 = rect ? rect->y : 0;
    int w  = rect ? rect->w : dst->w;
    int h  = rect ? rect->h : dst->h;
    if (x0 < 0) { w += x0; x0 = 0; }
    if (y0 < 0) { h += y0; y0 = 0; }
    if (x0 + w > dst->w) w = dst->w - x0;
    if (y0 + h > dst->h) h = dst->h - y0;
    if (w <= 0 || h <= 0) return;
    Uint8 c = (Uint8)(color & 0xFF);
    for (int y = 0; y < h; y++)
        memset(dst->pixels + (y0 + y) * dst->pitch + x0, c, (size_t)w);
}

void tc_BlitSurface(SDL_Surface *src, const SDL_Rect * /*srcrect*/,
                    SDL_Surface *dst, SDL_Rect * /*dstrect*/) {
    if (!src || !dst) return;
    int rows = (src->h < dst->h) ? src->h : dst->h;
    int cols = (src->w < dst->w) ? src->w : dst->w;
    for (int y = 0; y < rows; y++)
        memcpy(dst->pixels + y * dst->pitch,
               src->pixels + y * src->pitch,
               (size_t)cols);
}

/* ================================================================== */
/* Display                                                             */
/* ================================================================== */

void tc_CopyRectToScreen(const Uint8 *pixels, int pitch,
                          int x, int y, int w, int h) {
    if (g_system)
        g_system->copyRectToScreen(pixels + y * pitch + x,
                                   pitch, x, y, w, h);
}

void tc_UpdateScreen(void) {
    if (g_system)
        g_system->updateScreen();
}

/* ================================================================== */
/* Window / renderer — all no-ops; ScummVM owns the window            */
/* ================================================================== */

static SDL_Window   s_dummyWindow   = {0};
static SDL_Renderer s_dummyRenderer = {0};
static SDL_Texture  s_dummyTexture  = {0};

bool tc_InitSubSystem(Uint32 /*flags*/) { return true; }
void tc_QuitSubSystem(Uint32 /*flags*/) {}

SDL_Window *tc_CreateWindow(const char * /*t*/, int /*w*/, int /*h*/,
                             Uint32 /*f*/) { return &s_dummyWindow; }
SDL_Renderer *tc_CreateRenderer(SDL_Window * /*w*/, const char * /*n*/) {
    return &s_dummyRenderer; }
SDL_Texture *tc_CreateTexture(SDL_Renderer * /*r*/, Uint32 /*f*/, int /*a*/,
                               int /*w*/, int /*h*/) { return &s_dummyTexture; }

void tc_ShowWindow(SDL_Window * /*w*/) {}
void tc_DestroyWindow(SDL_Window * /*w*/) {}
void tc_DestroyRenderer(SDL_Renderer * /*r*/) {}
void tc_DestroyTexture(SDL_Texture * /*t*/) {}

bool tc_SetRenderLogicalPresentation(SDL_Renderer * /*r*/, int /*w*/,
                                      int /*h*/, int /*m*/) { return true; }
bool tc_SetWindowFullscreenMode(SDL_Window * /*w*/,
                                const void * /*m*/) { return true; }
void tc_SetWindowMinimumSize(SDL_Window * /*w*/, int /*a*/, int /*b*/) {}

void tc_GetWindowSize(SDL_Window * /*w*/, int *pw, int *ph) {
    if (pw) *pw = 320;  if (ph) *ph = 200; }
void tc_GetWindowSizeInPixels(SDL_Window * /*w*/, int *pw, int *ph) {
    if (pw) *pw = 320;  if (ph) *ph = 200; }

SDL_WindowID tc_GetWindowID(SDL_Window * /*w*/) { return 1; }

void tc_WarpMouseInWindow(SDL_Window * /*w*/, float /*x*/, float /*y*/) {}
void tc_RaiseWindow(SDL_Window * /*w*/) {}
void tc_SetWindowAlwaysOnTop(SDL_Window * /*w*/, bool /*on*/) {}
void tc_SetWindowKeyboardGrab(SDL_Window * /*w*/, bool /*g*/) {}
bool tc_SetTextureScaleMode(SDL_Texture * /*t*/, int /*m*/) { return true; }

bool tc_SetRenderDrawColor(SDL_Renderer * /*r*/, Uint8 /*a*/, Uint8 /*b*/,
                            Uint8 /*c*/, Uint8 /*d*/) { return true; }
bool tc_RenderClear(SDL_Renderer * /*r*/) { return true; }
bool tc_RenderTexture(SDL_Renderer * /*r*/, SDL_Texture * /*t*/,
                      const void * /*s*/, const void * /*d*/) { return true; }
bool tc_RenderPresent(SDL_Renderer * /*r*/) { return true; }

bool tc_LockTexture(SDL_Texture * /*t*/, const void * /*rect*/,
                    void ** /*pixels*/, int * /*pitch*/) { return false; }
void tc_UnlockTexture(SDL_Texture * /*t*/) {}

bool tc_ConvertPixels(int /*w*/, int /*h*/, Uint32 /*sf*/, const void * /*s*/,
                       int /*sp*/, Uint32 /*df*/, void * /*d*/,
                       int /*dp*/) { return true; }

char *tc_GetPrefPath(const char * /*org*/, const char * /*app*/) {
    return nullptr; }

bool tc_RenderCoordinatesFromWindow(SDL_Renderer * /*r*/,
                                    float wx, float wy,
                                    float *rx, float *ry) {
    if (rx) *rx = wx;  if (ry) *ry = wy;
    return true;
}

/* ================================================================== */
/* Input                                                               */
/* ================================================================== */

static float s_mouseX = 160.0f;
static float s_mouseY = 100.0f;

static Uint32 mapKey(Common::KeyCode kc) {
    switch (kc) {
    case Common::KEYCODE_LEFT:     return SDLK_LEFT;
    case Common::KEYCODE_RIGHT:    return SDLK_RIGHT;
    case Common::KEYCODE_UP:       return SDLK_UP;
    case Common::KEYCODE_DOWN:     return SDLK_DOWN;
    case Common::KEYCODE_SPACE:    return SDLK_SPACE;
    case Common::KEYCODE_RETURN:   return SDLK_RETURN;
    case Common::KEYCODE_KP_ENTER: return SDLK_KP_ENTER;
    case Common::KEYCODE_ESCAPE:   return SDLK_ESCAPE;
    case Common::KEYCODE_F1:       return SDLK_F1;
    case Common::KEYCODE_F2:       return SDLK_F2;
    case Common::KEYCODE_F3:       return SDLK_F3;
    case Common::KEYCODE_F4:       return SDLK_F4;
    case Common::KEYCODE_F5:       return SDLK_F5;
    case Common::KEYCODE_F6:       return SDLK_F6;
    case Common::KEYCODE_F7:       return SDLK_F7;
    case Common::KEYCODE_F8:       return SDLK_F8;
    case Common::KEYCODE_F9:       return SDLK_F9;
    case Common::KEYCODE_F10:      return SDLK_F10;
    default:                       return (Uint32)kc;
    }
}

void tc_PumpEvents(void) { /* pumped automatically by ScummVM */ }

bool tc_PollEvent(SDL_Event *out) {
    if (!g_system || !out) return false;

    Common::EventManager *mgr = g_system->getEventManager();
    Common::Event ev;

    while (mgr->pollEvent(ev)) {
        memset(out, 0, sizeof(SDL_Event));

        switch (ev.type) {

        /* ---- Quit / Return-to-launcher ---- */
        case Common::EVENT_QUIT:
        case Common::EVENT_RETURN_TO_LAUNCHER:
            out->type = SDL_EVENT_QUIT;
            return true;

        /* ---- Window focus: handle pause inline ---- */
        case Common::EVENT_FOCUS_LOST:
            g_tcPaused = 1;
            if (g_system && g_system->getMixer())
                g_system->getMixer()->pauseAll(true);
            continue;   /* don't deliver to game */

        case Common::EVENT_FOCUS_GAINED:
            g_tcPaused = 0;
            if (g_system && g_system->getMixer())
                g_system->getMixer()->pauseAll(false);
            continue;

        /* ---- Keyboard ---- */
        case Common::EVENT_KEYDOWN:
            out->type    = SDL_EVENT_KEY_DOWN;
            out->key.key = mapKey(ev.kbd.keycode);
            return true;

        /* ---- Mouse motion ---- */
        case Common::EVENT_MOUSEMOVE:
            s_mouseX           = (float)ev.mouse.x;
            s_mouseY           = (float)ev.mouse.y;
            out->type          = SDL_EVENT_MOUSE_MOTION;
            out->motion.x      = s_mouseX;
            out->motion.y      = s_mouseY;
            out->motion.xrel   = 0.0f;   /* relative not tracked here */
            out->motion.yrel   = 0.0f;
            return true;

        /* ---- Mouse buttons ---- */
        case Common::EVENT_LBUTTONDOWN:
            s_mouseX           = (float)ev.mouse.x;
            s_mouseY           = (float)ev.mouse.y;
            out->type          = SDL_EVENT_MOUSE_BUTTON_DOWN;
            out->button.button = SDL_BUTTON_LEFT;
            out->button.x      = s_mouseX;
            out->button.y      = s_mouseY;
            return true;

        case Common::EVENT_LBUTTONUP:
            s_mouseX           = (float)ev.mouse.x;
            s_mouseY           = (float)ev.mouse.y;
            out->type          = SDL_EVENT_MOUSE_BUTTON_UP;
            out->button.button = SDL_BUTTON_LEFT;
            out->button.x      = s_mouseX;
            out->button.y      = s_mouseY;
            return true;

        case Common::EVENT_RBUTTONDOWN:
            s_mouseX           = (float)ev.mouse.x;
            s_mouseY           = (float)ev.mouse.y;
            out->type          = SDL_EVENT_MOUSE_BUTTON_DOWN;
            out->button.button = SDL_BUTTON_RIGHT;
            out->button.x      = s_mouseX;
            out->button.y      = s_mouseY;
            return true;

        case Common::EVENT_RBUTTONUP:
            s_mouseX           = (float)ev.mouse.x;
            s_mouseY           = (float)ev.mouse.y;
            out->type          = SDL_EVENT_MOUSE_BUTTON_UP;
            out->button.button = SDL_BUTTON_RIGHT;
            out->button.x      = s_mouseX;
            out->button.y      = s_mouseY;
            return true;

        default:
            continue;   /* discard, keep polling */
        }
    }
    return false;
}

bool tc_GetMouseState(float *x, float *y) {
    if (x) *x = s_mouseX;
    if (y) *y = s_mouseY;
    return true;
}

/* ================================================================== */
/* Timing                                                              */
/* ================================================================== */

Uint64 tc_GetPerformanceFrequency(void) { return 1000ULL; }

Uint64 tc_GetPerformanceCounter(void) {
    return g_system ? (Uint64)g_system->getMillis() : 0ULL;
}

void tc_Delay(Uint32 ms) {
    /* ---------------------------------------------------------------
     * Background threads (e.g. the audio mix thread) must NOT touch
     * ScummVM's event manager, call longjmp/tc_QuitGame, or invoke
     * g_system APIs that are only safe on the main thread.
     * For them: a plain usleep() is correct and sufficient.
     * --------------------------------------------------------------- */
    if (!onMainThread()) {
#if defined(POSIX)
        if (ms) usleep((useconds_t)ms * 1000u);
#elif defined(WIN32)
        if (ms) Sleep(ms);
#endif
        return;
    }

    if (!g_system) return;

    /* Main thread: spin-wait while paused, pumping events so that
     * EVENT_FOCUS_GAINED can arrive and lift the pause.              */
    while (g_tcPaused) {
        g_system->delayMillis(10);
        Common::EventManager *mgr = g_system->getEventManager();
        Common::Event ev;
        while (mgr->pollEvent(ev)) {
            if (ev.type == Common::EVENT_FOCUS_GAINED) {
                g_tcPaused = 0;
                if (g_system->getMixer()) g_system->getMixer()->pauseAll(false);
            } else if (ev.type == Common::EVENT_QUIT ||
                       ev.type == Common::EVENT_RETURN_TO_LAUNCHER) {
                tc_QuitGame();
            }
        }
    }

    g_system->delayMillis(ms);
}

Uint32 tc_GetTicks(void) {
    return g_system ? g_system->getMillis() : 0;
}

/* ================================================================== */
/* Pause control (called from engine.cpp::pauseEngineIntern)          */
/* extern "C" so engine.cpp and the C header see the same symbol.     */
/* ================================================================== */

extern "C" void tc_SetPaused(int paused) {
    g_tcPaused = paused ? 1 : 0;
    /* The mixer pause is handled by ScummVM's Engine::pauseEngineIntern
     * base implementation, which calls getMixer()->pauseAll().         */
}

/* ================================================================== */
/* Joystick (all stubs)                                                */
/* ================================================================== */

int          tc_NumJoysticks(void)               { return 0; }
SDL_Joystick *tc_JoystickOpen(int /*idx*/)       { return nullptr; }
bool         tc_JoystickOpened(int /*idx*/)      { return false; }
void         tc_JoystickClose(SDL_Joystick * /*j*/) {}
Uint32       tc_WasInit(Uint32 /*flags*/)         { return 0; }

/* ================================================================== */
/* Audio — TheClouAudioStream ring-buffer → ScummVM Mixer             */
/*                                                                     */
/* Architecture:                                                       */
/*   Game audio thread  → tc_PutAudioStreamData() → ring buffer       */
/*   ScummVM mixer thread ← TheClouAudioStream::readBuffer() ← buffer */
/* SDL_AudioStream::mutex serialises the two sides.                   */
/* ================================================================== */

/* ~1 second of audio at 22050 Hz / 16-bit mono */
#define TC_AUDIO_BUF_SIZE (22050 * 2)

SDL_AudioStream *tc_OpenAudioDeviceStream(Uint32 /*devid*/,
                                           const SDL_AudioSpec * /*spec*/,
                                           void * /*cb*/, void * /*ud*/) {
    SDL_AudioStream *s =
        (SDL_AudioStream *)calloc(1, sizeof(SDL_AudioStream));
    if (!s) return nullptr;

    s->buf = (uint8_t *)calloc(1, TC_AUDIO_BUF_SIZE);
    if (!s->buf) { free(s); return nullptr; }
    s->size  = TC_AUDIO_BUF_SIZE;
    s->mutex = new Common::Mutex();

    s->scummStream = new TheClouAudioStream(s);

    if (g_system && g_system->getMixer()) {
        g_system->getMixer()->playStream(
            Audio::Mixer::kMusicSoundType,
            &s->handle,
            s->scummStream,
            -1,
            Audio::Mixer::kMaxChannelVolume,
            0,
            DisposeAfterUse::NO);   /* we manage lifetime explicitly */
    }
    return s;
}

int tc_GetAudioStreamAvailable(SDL_AudioStream *s) {
    if (!s) return 0;
    Common::StackLock lock(*s->mutex);
    return s->available;
}

bool tc_PutAudioStreamData(SDL_AudioStream *s, const void *buf, int len) {
    if (!s || !buf || len <= 0) return false;
    Common::StackLock lock(*s->mutex);

    const uint8_t *src = (const uint8_t *)buf;
    int space = s->size - s->available;
    if (len > space) len = space;   /* drop excess if buffer full */

    for (int i = 0; i < len; i++) {
        s->buf[s->write_pos] = src[i];
        s->write_pos = (s->write_pos + 1) % s->size;
    }
    s->available += len;
    return true;
}

bool tc_ResumeAudioStreamDevice(SDL_AudioStream *s) {
    if (s && g_system && g_system->getMixer())
        g_system->getMixer()->pauseHandle(s->handle, false);
    return true;
}

bool tc_PauseAudioStreamDevice(SDL_AudioStream *s) {
    if (s && g_system && g_system->getMixer())
        g_system->getMixer()->pauseHandle(s->handle, true);
    return true;
}

void tc_FlushAudioStream(SDL_AudioStream *s) {
    if (!s) return;
    Common::StackLock lock(*s->mutex);
    s->read_pos = s->write_pos = s->available = 0;
}

void tc_DestroyAudioStream(SDL_AudioStream *s) {
    if (!s) return;

    /* Deactivate stream so readBuffer() returns silence immediately */
    if (s->scummStream) s->scummStream->deactivate();

    /* Stop mixer channel (stops pulling from scummStream) */
    if (g_system && g_system->getMixer())
        g_system->getMixer()->stopHandle(s->handle);

    delete s->scummStream;
    delete s->mutex;
    free(s->buf);
    free(s);
}

/* ================================================================== */
/* Threading                                                           */
/* ================================================================== */

SDL_Mutex *tc_CreateMutex(void) {
    SDL_Mutex *m = (SDL_Mutex *)calloc(1, sizeof(SDL_Mutex));
    if (m) m->m = new Common::Mutex();
    return m;
}

void tc_DestroyMutex(SDL_Mutex *m) {
    if (!m) return;
    delete m->m;
    free(m);
}

void tc_LockMutex(SDL_Mutex *m)   { if (m && m->m) m->m->lock(); }
void tc_UnlockMutex(SDL_Mutex *m) { if (m && m->m) m->m->unlock(); }

#if defined(POSIX)
static void *threadTrampoline(void *arg) {
    SDL_Thread *t = (SDL_Thread *)arg;
    t->result = t->fn(t->data);
    return nullptr;
}
#elif defined(WIN32)
static DWORD WINAPI threadTrampoline(LPVOID arg) {
    SDL_Thread *t = (SDL_Thread *)arg;
    t->result = t->fn(t->data);
    return 0;
}
#endif

SDL_Thread *tc_CreateThread(int (*fn)(void *), const char * /*name*/,
                             void *data) {
    SDL_Thread *t = (SDL_Thread *)calloc(1, sizeof(SDL_Thread));
    if (!t) return nullptr;
    t->fn   = fn;
    t->data = data;
#if defined(POSIX)
    if (pthread_create(&t->thread, nullptr, threadTrampoline, t) != 0) {
        free(t); return nullptr; }
#elif defined(WIN32)
    DWORD tid;
    t->thread = CreateThread(nullptr, 0, threadTrampoline, t, 0, &tid);
    if (!t->thread) { free(t); return nullptr; }
#else
    debug(1, "tc_CreateThread: threading not supported on this platform");
    t->started = false;
#endif
    return t;
}

void tc_WaitThread(SDL_Thread *t, int *status) {
    if (!t) return;
#if defined(POSIX)
    pthread_join(t->thread, nullptr);
#elif defined(WIN32)
    WaitForSingleObject(t->thread, INFINITE);
    CloseHandle(t->thread);
#endif
    if (status) *status = t->result;
    free(t);
}

/* ================================================================== */
/* WAV loading                                                         */
/* ================================================================== */

bool tc_LoadWAV(const char *file, SDL_AudioSpec *spec,
                Uint8 **audio_buf, Uint32 *audio_len) {
    if (!file || !spec || !audio_buf || !audio_len) return false;

    Common::FSNode node(file);
    Common::SeekableReadStream *stream = node.createReadStream();
    if (!stream) { s_lastError = "file not found"; return false; }

    int64_t total = stream->size();
    if (total < 44) { delete stream; s_lastError = "too small for WAV"; return false; }

    byte hdr[44];
    if (stream->read(hdr, 44) != 44) { delete stream; return false; }

    if (memcmp(hdr, "RIFF", 4) || memcmp(hdr+8, "WAVEfmt ", 8)) {
        delete stream; s_lastError = "not a WAV file"; return false; }

    uint16_t channels      = (uint16_t)(hdr[22] | (hdr[23] << 8));
    uint32_t sampleRate    = (uint32_t)(hdr[24] | (hdr[25]<<8) |
                                        (hdr[26]<<16) | (hdr[27]<<24));
    uint16_t bitsPerSample = (uint16_t)(hdr[34] | (hdr[35] << 8));

    /* Find 'data' chunk */
    stream->seek(36);
    byte tag[4];
    uint32_t chunkSize = 0;
    while (stream->read(tag, 4) == 4) {
        chunkSize = stream->readUint32LE();
        if (memcmp(tag, "data", 4) == 0) break;
        stream->skip((int32_t)chunkSize);
        chunkSize = 0;
    }
    if (chunkSize == 0) {
        delete stream; s_lastError = "no data chunk"; return false; }

    *audio_buf = (Uint8 *)malloc(chunkSize);
    if (!*audio_buf) { delete stream; return false; }
    if ((uint32_t)stream->read(*audio_buf, chunkSize) != chunkSize) {
        free(*audio_buf); *audio_buf = nullptr;
        delete stream; return false; }
    delete stream;

    *audio_len     = chunkSize;
    spec->freq     = (int)sampleRate;
    spec->channels = (uint8_t)channels;
    spec->format   = (bitsPerSample == 16) ? SDL_AUDIO_S16 : 0;
    return true;
}

bool tc_ConvertAudioSamples(const SDL_AudioSpec *src_spec,
                            const Uint8 *src_data, int src_len,
                            const SDL_AudioSpec *dst_spec,
                            Uint8 **dst_data, int *dst_len) {
    if (!src_spec || !dst_spec || !src_data || !dst_data || !dst_len)
        return false;
    /* Simple passthrough when specs already match */
    if (src_spec->format   == dst_spec->format   &&
        src_spec->channels == dst_spec->channels &&
        src_spec->freq     == dst_spec->freq) {
        *dst_data = (Uint8 *)malloc((size_t)src_len);
        if (!*dst_data) return false;
        memcpy(*dst_data, src_data, (size_t)src_len);
        *dst_len = src_len;
        return true;
    }
    *dst_data = nullptr;
    *dst_len  = 0;
    s_lastError = "audio conversion not supported";
    return false;
}
