/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/
#ifndef TC_FX_H
#define TC_FX_H

#include "theclou.h"

#include "sound/buffer.h"

struct SDL_AudioStream;

#define	SND_FREQUENCY				22050
#define SND_MAX_VOLUME                          255

#define SND_BUFFER_SIZE				65536
#define SND_SAMPLES                             1024


struct FXBase {
    uword us_AudioOk;

    SND_BUFFER *pSfxBuffer;
    SND_BUFFER *pMusicBuffer;

    struct SDL_AudioStream *audioStream;
    /* audioThread / audioThreadRunning removed: mixing now happens inline
     * inside TheClouAudioStream::readBuffer() (ScummVM pull model).      */
    SDL_Mutex *audioMutex;
};

extern struct FXBase FXBase;

extern void sndInitFX(void);
extern void sndDoneFX(void);
extern void sndPrepareFX(const char *Name);
extern void sndPlayFX(void);

extern void InitAudio(void);
extern void RemoveAudio(void);
void sndAudioLock(void);
void sndAudioUnlock(void);

/* Called by TheClouAudioStream::readBuffer() on the ScummVM mixer thread.
 * Mixes one chunk of audio (OPL music + SFX + speech) into buf[0..len-1].
 * Internally acquires/releases the audio mutex.                          */
void sndMixIntoBuffer(Uint8 *buf, int len);

#endif

