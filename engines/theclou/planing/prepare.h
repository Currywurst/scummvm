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

#ifndef ENGINES_THECLOU_PLANING_PREPARE_H
#define ENGINES_THECLOU_PLANING_PREPARE_H

#include "theclou.h"
#include "memory/memory.h"
#include "error/error.h"
#include "disk/disk.h"
#include "list/list.h"
#include "data/database.h"
#include "data/relation.h"

#include "data/objstd/tcdata.h"
#include "text/text.h"
#include "landscap/landscap.h"
#include "planing/main.h"
#include "planing/graphics.h"
#include "planing/io.h"
#include "planing/prepare.h"
#include "planing/support.h"
#include "planing/system.h"
#include "planing/sync.h"
#include "planing/guards.h"


/* Gfx prepare modes */
#define PLANING_GFX_LANDSCAPE  (1)
#define PLANING_GFX_SPRITES    (1<<1)
#define PLANING_GFX_BACKGROUND (1<<2)

/* Sys modes */
#define PLANING_INIT_PERSONSLIST    (1)
#define PLANING_HANDLER_OPEN        (1<<1)
#define PLANING_HANDLER_CLOSE       (1<<2)
#define PLANING_HANDLER_CLEAR       (1<<3)
#define PLANING_HANDLER_SET         (1<<4)
#define PLANING_HANDLER_ADD         (1<<5)
#define PLANING_GUARDS_LOAD         (1<<6)

/* Counts */
#define PLANING_NR_PERSONS          4
#define PLANING_NR_GUARDS           4
#define PLANING_NR_LOOTS            8
#define PLANING_NR_PLANS            (NRBLINES - 1)

/* getting correct has loot relation */
#define hasLoot(current)            (((LSArea)dbGetObject(livWhereIs(Planing_Name[current])))->ul_ObjectBaseNr + REL_HAS_LOOT_OFFSET)


extern LIST *PersonsList;
extern LIST *BurglarsList;

extern ubyte PersonsNr;
extern ubyte BurglarsNr;
extern U32 CurrentPerson;

extern U32 Planing_Weight[PLANING_NR_PERSONS];
extern U32 Planing_Volume[PLANING_NR_PERSONS];

extern ubyte Planing_Loot[PLANING_NR_LOOTS];
extern ubyte Planing_Guard[PLANING_NR_GUARDS];

extern char Planing_Name[PLANING_NR_PERSONS + PLANING_NR_GUARDS][20];

extern LIST *Planing_GuardRoomList[PLANING_NR_GUARDS];

extern U32 Planing_BldId;



void plBuildHandler(NODE * n);
void plClearHandler(NODE * n);
void plCloseHandler(NODE * n);

void plPrepareData(void);
void plPrepareSprite(U32 livNr, U32 areaId);
void plPrepareNames(void);

void plPrepareGfx(U32 objId, ubyte landscapMode, ubyte prepareMode);
void plPrepareRel(void);
void plPrepareSys(U32 currPer, U32 objId, ubyte sysMode);

void plUnprepareGfx(void);
void plUnprepareRel(void);
void plUnprepareSys(void);
#endif  // ENGINES_THECLOU_PLANING_PREPARE_H
