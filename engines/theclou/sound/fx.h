/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * Portions copyright (c) 2005 Vasco Alexandre da Silva Costa
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/** @file sound/fx.h — Low-level audio FX / mixer interface for Der Clou! */

#ifndef ENGINES_THECLOU_SOUND_FX_H
#define ENGINES_THECLOU_SOUND_FX_H

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

#endif // ENGINES_THECLOU_SOUND_FX_H

