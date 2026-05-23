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

#ifndef ENGINES_THECLOU_PLANING_SUPPORT_H
#define ENGINES_THECLOU_PLANING_SUPPORT_H

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

#ifndef ENGINES_THECLOU_PLANING_SUPPORT_H
#include "planing/support.h"
#endif  // ENGINES_THECLOU_PLANING_SUPPORT_H
#include "planing/system.h"
#include "planing/sync.h"
#include "planing/guards.h"


/* Area defines */
#define PLANING_AREA_PERSON  3
#define PLANING_AREA_CAR     16

/* functions for bits */
#define BIT(x)               (UINT32_C(1)<<(x))
#define CHECK_STATE(v,b)     ((v) & BIT(b))

/* Alarms & power control */
#define PLANING_ALARM_Z3          3
#define PLANING_ALARM_X5          8
#define PLANING_ALARM_TOP3        15
#define PLANING_POWER             1

/* all times in 1/3 sec */
#define PLANING_CORRECT_TIME      3


U32 plGetNextLoot(void);

ubyte plLivingsPosAtCar(U32 bldId);
ubyte plAllInCar(U32 bldId);
ubyte plIsStair(U32 objId);

void plCorrectOpened(LSObject obj, ubyte open);
ubyte plIgnoreLock(U32 objId);

void plMove(U32 current, ubyte direction);
void plWork(U32 current);

LIST *plGetObjectsList(U32 current, ubyte addLootBags);
void plInsertGuard(LIST * list, U32 current, U32 guard);
ubyte plObjectInReach(U32 current, U32 objId);
#endif  // ENGINES_THECLOU_PLANING_SUPPORT_H
