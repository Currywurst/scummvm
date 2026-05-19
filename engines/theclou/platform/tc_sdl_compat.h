/*
 * tc_sdl_compat.h  —  Drop-in replacement for <SDL3/SDL.h> inside the
 * ScummVM/theclou engine.  All SDL3 types, constants and function calls
 * are mapped to tc_* functions implemented in tc_platform.cpp via the
 * ScummVM OSystem/Audio::Mixer APIs.
 *
 * Only the subset of SDL3 actually used by the derclou source files is
 * covered here.
 */

#ifndef TC_SDL_COMPAT_H
#define TC_SDL_COMPAT_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>   /* memset */
#include <stdlib.h>   /* free   */

/* ------------------------------------------------------------------ */
/* Basic integer types SDL3 provides                                   */
/* ------------------------------------------------------------------ */
typedef uint8_t  Uint8;
typedef uint16_t Uint16;
typedef uint32_t Uint32;
typedef int32_t  Sint32;
typedef uint64_t Uint64;

/* ------------------------------------------------------------------ */
/* Pixel / palette types                                               */
/* ------------------------------------------------------------------ */
typedef struct SDL_Color {
    Uint8 r, g, b, a;
} SDL_Color;

typedef struct SDL_Palette {
    SDL_Color *colors;
    int        ncolors;
    /* internal – do not touch from game code */
    int        _owned;
} SDL_Palette;

typedef struct SDL_Surface {
    Uint8       *pixels;
    int          w, h, pitch;
    Uint32       format;        /* dummy – not used in ScummVM path */
    SDL_Palette *_pal;          /* internal palette pointer          */
    Uint32       _colorKey;     /* color key value (for font blitting) */
    int          _hasColorKey;  /* non-zero when color key is active   */
} SDL_Surface;

typedef struct { Sint32 x, y, w, h; } SDL_Rect;
typedef Uint32 SDL_WindowID;

/* ------------------------------------------------------------------ */
/* Opaque handle types (forward-declared structs so fx.h 'struct X'   */
/* forward-declarations stay compatible)                               */
/* ------------------------------------------------------------------ */
typedef struct SDL_Window       SDL_Window;
typedef struct SDL_Renderer     SDL_Renderer;
typedef struct SDL_Texture      SDL_Texture;
typedef struct SDL_AudioStream  SDL_AudioStream;
typedef struct SDL_Mutex        SDL_Mutex;
typedef struct SDL_Thread       SDL_Thread;
typedef struct SDL_Joystick     SDL_Joystick;

/* ------------------------------------------------------------------ */
/* Audio                                                               */
/* ------------------------------------------------------------------ */
typedef struct SDL_AudioSpec {
    Uint16 format;
    Uint8  channels;
    int    freq;
} SDL_AudioSpec;

#define SDL_AUDIO_S16                  0x8010
#define SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK  ((Uint32)0xFFFFFFFF)

/* ------------------------------------------------------------------ */
/* Pixel format constants (dummies – only used in the old SDL3         */
/* window-init path that we replace with no-ops)                       */
/* ------------------------------------------------------------------ */
#define SDL_PIXELFORMAT_INDEX8         0x13000801u
#define SDL_PIXELFORMAT_XRGB8888       0x16161804u
#define SDL_TEXTUREACCESS_STREAMING    1
#define SDL_LOGICAL_PRESENTATION_LETTERBOX 1
typedef int SDL_ScaleMode;
#define SDL_SCALEMODE_NEAREST          ((SDL_ScaleMode)0)
#define SDL_SCALEMODE_PIXELART         ((SDL_ScaleMode)3)
#define SDL_ALPHA_OPAQUE               255

/* ------------------------------------------------------------------ */
/* Window / init flags                                                 */
/* ------------------------------------------------------------------ */
#define SDL_WINDOW_OPENGL              0x0002u
#define SDL_WINDOW_FULLSCREEN          0x0001u
#define SDL_WINDOW_RESIZABLE           0x0020u

#define SDL_INIT_VIDEO                 0x00000020u
#define SDL_INIT_AUDIO                 0x00000010u
#define SDL_INIT_JOYSTICK              0x00000200u
#define SDL_INIT_CDROM                 0x00000100u

/* Disable the SDL1-era CDROM subsystem — ScummVM has no CD-audio backend
 * for this and the SDL_CD / CD_ERROR types are never available here.      */
#ifndef THECLOU_DISABLE_SDL_CDROM
#define THECLOU_DISABLE_SDL_CDROM 1
#endif

/* ------------------------------------------------------------------ */
/* Version check – we are not really SDL3                              */
/* ------------------------------------------------------------------ */
#define SDL_VERSION_ATLEAST(x,y,z)     0

/* ------------------------------------------------------------------ */
/* Keyboard / mouse constants                                          */
/* ------------------------------------------------------------------ */
typedef Uint32 SDL_Keycode;

/* SDL3 keycode values for the subset used by derclou */
#define SDLK_LEFT       0x40000050u
#define SDLK_RIGHT      0x4000004Fu
#define SDLK_UP         0x40000052u
#define SDLK_DOWN       0x40000051u
#define SDLK_SPACE      0x00000020u
#define SDLK_RETURN     0x0000000Du
#define SDLK_KP_ENTER   0x40000058u
#define SDLK_ESCAPE     0x0000001Bu
#define SDLK_F1         0x4000003Au
#define SDLK_F2         0x4000003Bu
#define SDLK_F3         0x4000003Cu
#define SDLK_F4         0x4000003Du
#define SDLK_F5         0x4000003Eu
#define SDLK_F6         0x4000003Fu
#define SDLK_F7         0x40000040u
#define SDLK_F8         0x40000041u
#define SDLK_F9         0x40000042u
#define SDLK_F10        0x40000043u

#define SDL_BUTTON_LEFT  1
#define SDL_BUTTON_RIGHT 3

/* ------------------------------------------------------------------ */
/* Event types                                                         */
/* ------------------------------------------------------------------ */
#define SDL_EVENT_QUIT                       0x100u
#define SDL_EVENT_KEY_DOWN                   0x300u
#define SDL_EVENT_MOUSE_MOTION               0x400u
#define SDL_EVENT_MOUSE_BUTTON_DOWN          0x401u
#define SDL_EVENT_MOUSE_BUTTON_UP            0x402u
#define SDL_JOYBUTTONDOWN                    0x600u
#define SDL_EVENT_JOYSTICK_BUTTON_DOWN       0x600u
#define SDL_EVENT_WINDOW_RESIZED             0x200u
#define SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED  0x204u

typedef struct {
    Uint32     type;
    SDL_Keycode key;
    Uint16     mod;
} SDL_KeyboardEvent;

typedef struct {
    Uint32 type;
    float  x, y;
    float  xrel, yrel;   /* relative motion — used by inphdl.c */
} SDL_MouseMotionEvent;

typedef struct {
    Uint32 type;
    Uint8  button;
    float  x, y;   /* SDL3 uses float; cast to int where needed */
    Uint32 windowID;
} SDL_MouseButtonEvent;

typedef struct {
    Uint32 type;
    Uint32 windowID;
    int    data1, data2;
} SDL_WindowEvent;

typedef struct {
    Uint32 type;
    Uint8  button;
} SDL_JoyButtonEvent;

typedef union SDL_Event {
    Uint32                type;
    SDL_KeyboardEvent     key;
    SDL_MouseMotionEvent  motion;
    SDL_MouseButtonEvent  button;
    SDL_WindowEvent       window;
    SDL_JoyButtonEvent    jbutton;
    Uint8                 _padding[56];
} SDL_Event;

/* ------------------------------------------------------------------ */
/* Calling convention                                                  */
/* ------------------------------------------------------------------ */
#define SDLCALL
#define SDL_CALL

/* ------------------------------------------------------------------ */
/* Simple utility macros                                               */
/* ------------------------------------------------------------------ */
#define SDL_MUSTLOCK(s)         0
#define SDL_LockSurface(s)      0
#define SDL_UnlockSurface(s)    ((void)0)
#define SDL_memset              memset
#define SDL_free                free
#define SDL_min(a,b)            ((a)<(b)?(a):(b))
#define SDL_max(a,b)            ((a)>(b)?(a):(b))
#define SDL_zero(x)             (memset(&(x), 0, sizeof(x)))

/* ------------------------------------------------------------------ */
/* tc_* function declarations (implemented in tc_platform.cpp)        */
/* ------------------------------------------------------------------ */
#ifdef __cplusplus
extern "C" {
#endif

/* --- Error string -------------------------------------------------- */
const char *tc_GetError(void);

/* --- Surface management ------------------------------------------- */
SDL_Surface *tc_CreateSurface(int w, int h, Uint32 format);
void         tc_DestroySurface(SDL_Surface *s);
SDL_Palette *tc_GetSurfacePalette(SDL_Surface *s);
SDL_Palette *tc_CreateSurfacePalette(SDL_Surface *s);
bool         tc_SetPaletteColors(SDL_Palette *pal, const SDL_Color *colors,
                                 int first, int ncolors);
void         tc_FillSurfaceRect(SDL_Surface *dst, const SDL_Rect *rect,
                                Uint32 color);
void         tc_BlitSurface(SDL_Surface *src, const SDL_Rect *srcrect,
                            SDL_Surface *dst, SDL_Rect *dstrect);

/* --- Display ------------------------------------------------------- */
/* Called by our replacement of gfxRealRefreshArea */
void tc_CopyRectToScreen(const Uint8 *pixels, int pitch,
                         int x, int y, int w, int h);
void tc_UpdateScreen(void);

/* --- Window (all no-ops; ScummVM owns the window) ------------------ */
bool tc_InitSubSystem(Uint32 flags);
void tc_QuitSubSystem(Uint32 flags);
SDL_Window   *tc_CreateWindow(const char *title, int w, int h, Uint32 flags);
SDL_Renderer *tc_CreateRenderer(SDL_Window *win, const char *name);
SDL_Texture  *tc_CreateTexture(SDL_Renderer *r, Uint32 fmt, int access,
                               int w, int h);
void tc_ShowWindow(SDL_Window *win);
void tc_DestroyWindow(SDL_Window *win);
void tc_DestroyRenderer(SDL_Renderer *r);
void tc_DestroyTexture(SDL_Texture *t);
bool tc_SetRenderLogicalPresentation(SDL_Renderer *r, int w, int h, int mode);
bool tc_SetWindowFullscreenMode(SDL_Window *win, const void *mode);
void tc_SetWindowMinimumSize(SDL_Window *win, int w, int h);
void tc_GetWindowSize(SDL_Window *win, int *w, int *h);
void tc_GetWindowSizeInPixels(SDL_Window *win, int *w, int *h);
SDL_WindowID tc_GetWindowID(SDL_Window *win);
void tc_WarpMouseInWindow(SDL_Window *win, float x, float y);
void tc_RaiseWindow(SDL_Window *win);
void tc_SetWindowAlwaysOnTop(SDL_Window *win, bool on);
void tc_SetWindowKeyboardGrab(SDL_Window *win, bool grabbed);
bool tc_SetTextureScaleMode(SDL_Texture *t, int mode);
bool tc_SetRenderDrawColor(SDL_Renderer *r, Uint8 red, Uint8 g, Uint8 b, Uint8 a);
bool tc_RenderClear(SDL_Renderer *r);
bool tc_RenderTexture(SDL_Renderer *r, SDL_Texture *t, const void *src, const void *dst);
bool tc_RenderPresent(SDL_Renderer *r);
bool tc_LockTexture(SDL_Texture *t, const void *rect, void **pixels, int *pitch);
void tc_UnlockTexture(SDL_Texture *t);
bool tc_ConvertPixels(int w, int h, Uint32 src_fmt, const void *src, int src_pitch,
                      Uint32 dst_fmt, void *dst, int dst_pitch);
char *tc_GetPrefPath(const char *org, const char *app);
bool tc_RenderCoordinatesFromWindow(SDL_Renderer *r, float wx, float wy,
                                    float *rx, float *ry);

/* --- Input --------------------------------------------------------- */
void tc_PumpEvents(void);
bool tc_PollEvent(SDL_Event *ev);
bool tc_GetMouseState(float *x, float *y);

/* --- Timing -------------------------------------------------------- */
Uint64 tc_GetPerformanceFrequency(void);
Uint64 tc_GetPerformanceCounter(void);
void   tc_Delay(Uint32 ms);
Uint32 tc_GetTicks(void);

/* --- Joystick (all stubs) ----------------------------------------- */
int          tc_NumJoysticks(void);
SDL_Joystick *tc_JoystickOpen(int idx);
bool         tc_JoystickOpened(int idx);
void         tc_JoystickClose(SDL_Joystick *j);
Uint32       tc_WasInit(Uint32 flags);

/* --- Audio --------------------------------------------------------- */
SDL_AudioStream *tc_OpenAudioDeviceStream(Uint32 devid,
                                           const SDL_AudioSpec *spec,
                                           void *callback, void *userdata);
int  tc_GetAudioStreamAvailable(SDL_AudioStream *stream);
bool tc_PutAudioStreamData(SDL_AudioStream *stream,
                           const void *buf, int buflen);
bool tc_ResumeAudioStreamDevice(SDL_AudioStream *stream);
bool tc_PauseAudioStreamDevice(SDL_AudioStream *stream);
void tc_FlushAudioStream(SDL_AudioStream *stream);
void tc_DestroyAudioStream(SDL_AudioStream *stream);

/* --- Threading ----------------------------------------------------- */
SDL_Mutex  *tc_CreateMutex(void);
void        tc_DestroyMutex(SDL_Mutex *m);
void        tc_LockMutex(SDL_Mutex *m);
void        tc_UnlockMutex(SDL_Mutex *m);
SDL_Thread *tc_CreateThread(int (*fn)(void *), const char *name, void *data);
void        tc_WaitThread(SDL_Thread *t, int *status);

/* --- WAV loading --------------------------------------------------- */
bool tc_LoadWAV(const char *file, SDL_AudioSpec *spec,
                Uint8 **audio_buf, Uint32 *audio_len);
bool tc_ConvertAudioSamples(const SDL_AudioSpec *src_spec,
                            const Uint8 *src_data, int src_len,
                            const SDL_AudioSpec *dst_spec,
                            Uint8 **dst_data, int *dst_len);

#ifdef __cplusplus
}  /* extern "C" */
#endif

/* ------------------------------------------------------------------ */
/* Map SDL_* calls to tc_* implementations                             */
/* ------------------------------------------------------------------ */

/* Error */
#define SDL_GetError()                  tc_GetError()

/* Surface */
#define SDL_CreateSurface(w,h,f)        tc_CreateSurface(w,h,f)
#define SDL_DestroySurface(s)           tc_DestroySurface(s)
#define SDL_GetSurfacePalette(s)        tc_GetSurfacePalette(s)
#define SDL_CreateSurfacePalette(s)     tc_CreateSurfacePalette(s)
#define SDL_SetPaletteColors(p,c,f,n)   tc_SetPaletteColors(p,c,f,n)
#define SDL_FillSurfaceRect(d,r,c)      tc_FillSurfaceRect(d,r,c)
#define SDL_BlitSurface(s,sr,d,dr)      tc_BlitSurface(s,sr,d,dr)

/* Top-level init / quit – ScummVM owns SDL, so these are no-ops */
#define SDL_Init(flags)                 ((void)(flags), true)
#define SDL_Quit()                      ((void)0)

/* Window / render (no-ops) */
#define SDL_InitSubSystem(f)            tc_InitSubSystem(f)
#define SDL_QuitSubSystem(f)            tc_QuitSubSystem(f)
#define SDL_CreateWindow(t,w,h,f)       tc_CreateWindow(t,w,h,f)
#define SDL_CreateRenderer(w,n)         tc_CreateRenderer(w,n)
#define SDL_CreateTexture(r,f,a,w,h)    tc_CreateTexture(r,f,a,w,h)
#define SDL_ShowWindow(w)               tc_ShowWindow(w)
#define SDL_DestroyWindow(w)            tc_DestroyWindow(w)
#define SDL_DestroyRenderer(r)          tc_DestroyRenderer(r)
#define SDL_DestroyTexture(t)           tc_DestroyTexture(t)
#define SDL_SetRenderLogicalPresentation(r,w,h,m) \
                                        tc_SetRenderLogicalPresentation(r,w,h,m)
#define SDL_SetWindowFullscreenMode(w,m) tc_SetWindowFullscreenMode(w,m)
#define SDL_SetWindowMinimumSize(w,a,b) tc_SetWindowMinimumSize(w,a,b)
#define SDL_GetWindowSize(w,a,b)        tc_GetWindowSize(w,a,b)
#define SDL_GetWindowSizeInPixels(w,a,b) tc_GetWindowSizeInPixels(w,a,b)
#define SDL_GetWindowID(w)              tc_GetWindowID(w)
#define SDL_WarpMouseInWindow(w,x,y)    tc_WarpMouseInWindow(w,x,y)
#define SDL_RaiseWindow(w)              tc_RaiseWindow(w)
#define SDL_SetWindowAlwaysOnTop(w,o)   tc_SetWindowAlwaysOnTop(w,o)
#define SDL_SetWindowKeyboardGrab(w,g)  tc_SetWindowKeyboardGrab(w,g)
#define SDL_SetTextureScaleMode(t,m)    tc_SetTextureScaleMode(t,m)
#define SDL_SetRenderDrawColor(r,a,b,c,d) tc_SetRenderDrawColor(r,a,b,c,d)
#define SDL_RenderClear(r)              tc_RenderClear(r)
#define SDL_RenderTexture(r,t,s,d)      tc_RenderTexture(r,t,s,d)
#define SDL_RenderPresent(r)            tc_RenderPresent(r)
#define SDL_LockTexture(t,r,p,pi)       tc_LockTexture(t,r,p,pi)
#define SDL_UnlockTexture(t)            tc_UnlockTexture(t)
#define SDL_ConvertPixels(w,h,sf,s,sp,df,d,dp) \
                                        tc_ConvertPixels(w,h,sf,s,sp,df,d,dp)
#define SDL_GetPrefPath(o,a)            tc_GetPrefPath(o,a)
#define SDL_RenderCoordinatesFromWindow(r,wx,wy,rx,ry) \
                                        tc_RenderCoordinatesFromWindow(r,wx,wy,rx,ry)

/* Input */
#define SDL_PumpEvents()                tc_PumpEvents()
#define SDL_PollEvent(e)                tc_PollEvent(e)
#define SDL_GetMouseState(x,y)          tc_GetMouseState(x,y)

/* Timing */
#define SDL_GetPerformanceFrequency()   tc_GetPerformanceFrequency()
#define SDL_GetPerformanceCounter()     tc_GetPerformanceCounter()
#define SDL_Delay(ms)                   tc_Delay(ms)
#define SDL_GetTicks()                  tc_GetTicks()

/* Joystick */
#define SDL_NumJoysticks()              tc_NumJoysticks()
#define SDL_JoystickOpen(i)             tc_JoystickOpen(i)
#define SDL_JoystickOpened(i)           tc_JoystickOpened(i)
#define SDL_JoystickClose(j)            tc_JoystickClose(j)
#define SDL_WasInit(f)                  tc_WasInit(f)

/* Audio */
#define SDL_OpenAudioDeviceStream(d,s,c,u) tc_OpenAudioDeviceStream(d,s,c,u)
#define SDL_GetAudioStreamAvailable(s)  tc_GetAudioStreamAvailable(s)
#define SDL_PutAudioStreamData(s,b,l)   tc_PutAudioStreamData(s,b,l)
#define SDL_ResumeAudioStreamDevice(s)  tc_ResumeAudioStreamDevice(s)
#define SDL_PauseAudioStreamDevice(s)   tc_PauseAudioStreamDevice(s)
#define SDL_FlushAudioStream(s)         tc_FlushAudioStream(s)
#define SDL_DestroyAudioStream(s)       tc_DestroyAudioStream(s)

/* Threading */
#define SDL_CreateMutex()               tc_CreateMutex()
#define SDL_DestroyMutex(m)             tc_DestroyMutex(m)
#define SDL_LockMutex(m)                tc_LockMutex(m)
#define SDL_UnlockMutex(m)              tc_UnlockMutex(m)
#define SDL_CreateThread(f,n,d)         tc_CreateThread(f,n,d)
#define SDL_WaitThread(t,s)             tc_WaitThread(t,s)

/* WAV */
#define SDL_LoadWAV(f,s,b,l)            tc_LoadWAV(f,s,b,l)
#define SDL_ConvertAudioSamples(ss,sd,sl,ds,dd,dl) \
                                        tc_ConvertAudioSamples(ss,sd,sl,ds,dd,dl)

/* ------------------------------------------------------------------ */
/* Quit / Pause control                                                */
/* tc_QuitGame()  — defined in platform/tc_quit.cpp; throws              */
/*                  TheClou::QuitException (clean, RAII-safe exit).       */
/* tc_SetPaused() — defined in platform/tc_platform.cpp; suspends the */
/*                  game loop and pauses ScummVM's audio mixer.       */
/* ------------------------------------------------------------------ */
#ifdef __cplusplus
extern "C" {
#endif
void tc_QuitGame(void);        /* platform/tc_quit.cpp — throws QuitException  */
void tc_SetPaused(int paused); /* tc_platform.cpp — suspends game loop    */
#ifdef __cplusplus
}
#endif

/* Window management — ScummVM owns the window; resize is a no-op */
#define SDL_SetWindowSize(win,w,h)      ((void)0)

/* Surface color key — stores key in SDL_Surface._colorKey / _hasColorKey */
static inline int SDL_SetSurfaceColorKey(SDL_Surface *s, int enable, Uint32 key) {
    if (s) { s->_hasColorKey = enable; s->_colorKey = key; }
    return 0;
}
static inline int SDL_GetSurfaceColorKey(SDL_Surface *s, Uint32 *key) {
    if (s && s->_hasColorKey) { if (key) *key = s->_colorKey; return 0; }
    return -1;
}

/* Math helpers — SDL3 provides these as wrappers around standard C math */
#include <math.h>
#define SDL_roundf(x)   roundf(x)
#define SDL_floorf(x)   floorf(x)
#define SDL_ceilf(x)    ceilf(x)
#define SDL_fabsf(x)    fabsf(x)
#define SDL_sqrtf(x)    sqrtf(x)

#endif /* TC_SDL_COMPAT_H */
