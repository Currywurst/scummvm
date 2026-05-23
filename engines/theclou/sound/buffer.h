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

/** @file sound/buffer.h — Ring-buffer for audio data in Der Clou! */

#ifndef ENGINES_THECLOU_SOUND_BUFFER_H
#define ENGINES_THECLOU_SOUND_BUFFER_H

#include "theclou.h"

/** Opaque ring-buffer type for audio samples. */
struct SndBuffer;
typedef struct SndBuffer SND_BUFFER;   // C compat alias

SND_BUFFER *sndCreateBuffer(unsigned size);
void        sndResetBuffer(SND_BUFFER *buffer);
void        sndFreeBuffer(SND_BUFFER *buffer);

unsigned sndLenBuffer(SND_BUFFER *buffer);

unsigned sndInsertBuffer(SND_BUFFER *buffer, const void *src, unsigned srcLen);
unsigned sndRemoveBuffer(SND_BUFFER *buffer, void *dst, unsigned dstLen);

#endif // ENGINES_THECLOU_SOUND_BUFFER_H

