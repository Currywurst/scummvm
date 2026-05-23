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

/** @file sound/newsound.h — High-level sound API for Der Clou! */

#ifndef ENGINES_THECLOU_SOUND_NEWSOUND_H
#define ENGINES_THECLOU_SOUND_NEWSOUND_H

#include "theclou.h"

/** Initialise the sound subsystem (call once at engine start). */
void sndInit(void);
/** Shut down the sound subsystem. */
void sndDone(void);

/** Play a named sound file; @p mode controls looping / priority. */
void sndPlaySound(const char *name, U32 mode);
/** Stop the currently playing sound. */
void sndStopSound(U8 dummy);
/** Fade music volume to @p targetVol. */
void sndFading(short int targetVol);

/** Return the name of the currently playing sound (may be nullptr). */
char *sndGetCurrSoundName(void);

bool sndSpeechLibraryAvailable(void);
bool sndPlaySpeechSample(const char *clipName);
void sndStopSpeechSample(void);
bool sndSpeechSamplePlaying(void);

#endif // ENGINES_THECLOU_SOUND_NEWSOUND_H

