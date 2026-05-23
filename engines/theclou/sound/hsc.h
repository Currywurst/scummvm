/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * Original code (c) 2001 Thomas Trummer
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

/** @file sound/hsc.h — HSC OPL music player for Der Clou! */

#ifndef ENGINES_THECLOU_SOUND_HSC_H
#define ENGINES_THECLOU_SOUND_HSC_H

#include "theclou.h"

void hscInit(void);
void hscReset(void);
void hscDone(void);

/** Load an HSC music file.  Returns 0 on success, non-zero on error. */
int  hscLoad(const char *fileName);

/** Mix @p len samples of OPL music into the audio buffer. */
void hscMusicPlayer(unsigned len);

#endif // ENGINES_THECLOU_SOUND_HSC_H
