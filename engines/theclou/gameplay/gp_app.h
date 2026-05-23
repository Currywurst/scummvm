/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * Original game code copyright (c) 1993-2001 respective authors
 * (see individual files for details).
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

#ifndef ENGINES_THECLOU_GAMEPLAY_GP_APP_H
#define ENGINES_THECLOU_GAMEPLAY_GP_APP_H

#include "theclou.h"
#include "memory/memory.h"
#include "error/error.h"
#include "list/list.h"
#include "disk/disk.h"
#include "gfx/gfx.h"
#include "anim/sysanim.h"
#include "story/story.h"
#include "data/dataappl.h"
#include "random/random.h"
#include "gameplay/gp.h"
#include "sound/newsound.h"
#include "scenes/evidence.h"
#include "scenes/scenes.h"

struct Scene;

extern void TCGamePause(ubyte activ);

extern ubyte tcPersonIsHere(void);

extern void tcPlaySound(void);
extern void tcPlayStreetSound(void);
extern void tcPersonGreetsMatt(void);

extern U32 StdHandle(U32 choice);

extern void SetFunc(struct Scene *sc, void (*init) (void), void (*done) (void));
void PrintStatus(const char *text);
extern void InitTaxiLocations(void);
extern void LinkScenes(void);

extern void tcGetLastName(char *Name, char *dest, U32 maxLength);
extern void tcCutName(char *Name, ubyte Sign, U32 maxLength);

extern ubyte tcLoadTheClou(void);
extern void tcSaveTheClou(void);
extern U32 tcBurglary(U32 buildingID);

extern ubyte tcLoadChangesInScenes(char *fileName);
extern ubyte tcSaveChangesInScenes(char *fileName);

extern void tcRefreshLocationInTitle(U32 locNr);
extern void ShowTime(U32 delay);
extern void RefreshAfterDiskRequester(void);

extern ubyte tcLoadIt(char activ);
extern void tcRefreshAfterLoad(ubyte loaded);

extern void ShowMenuBackground(void);

#endif  // ENGINES_THECLOU_GAMEPLAY_GP_APP_H
