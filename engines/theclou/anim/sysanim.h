/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * Original game code (c) 1993 Helmut Gaberschek & Kaweh Kazemi
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

/**
 * @file anim/sysanim.h
 * @brief Animation handler for Der Clou!
 *
 * Animation list file format (one line per animation entry):
 *   PictureMode, PictsPerSec, Pic1, AnimPic, PicAnzahl, Animphase
 *   Breite, Animphase Hoehe, Animphase offset, PlayMode
 *   XDest, YDest (offset relative to first picture)
 */

#ifndef ENGINES_THECLOU_ANIM_SYSANIM_H
#define ENGINES_THECLOU_ANIM_SYSANIM_H

#include "theclou.h"
#include "text/text.h"
#include "gfx/gfx.h"
#include "random/random.h"

void InitAnimHandler(void);
void CloseAnimHandler(void);

/** Play animation @p AnimID for @p how_often repetitions with @p mode flags. */
void PlayAnim(const char *AnimID, U16 how_often, U32 mode);
void StopAnim(void);

void GetAnim(const char *AnimID, char *Dest);
void animator(void);

void SuspendAnim(void);
void ContinueAnim(void);

#endif // ENGINES_THECLOU_ANIM_SYSANIM_H
