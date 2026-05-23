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

#ifndef ENGINES_THECLOU_PLANING_GRAPHICS_H
#define ENGINES_THECLOU_PLANING_GRAPHICS_H


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

#ifndef ENGINES_THECLOU_PLANING_GRAPHICS_H
#include "planing/graphics.h"
#endif  // ENGINES_THECLOU_PLANING_GRAPHICS_H
#include "planing/io.h"
#include "planing/prepare.h"
#include "planing/support.h"
#include "planing/system.h"
#include "planing/sync.h"
#include "planing/guards.h"



/* Messages modes */
#define PLANING_MSG_REFRESH   1
#define PLANING_MSG_WAIT      2

/* RefreshRP */
#define PLANING_REFRESH_RP_WIDTH    200
#define PLANING_REFRESH_RP_HEIGHT   50
#define PLANING_REFRESH_RP_DEPTH    2

void plPrintInfo(char *person);
void plMessage(const char *msg, ubyte flags);
void plPersonPict(U32 personId);
void plDisplayAbilities(void);
void plDisplayTimer(U32 time, ubyte doSpotsImmediatly);
void plDisplayInfo(void);
void plRefresh(U32 ItemId);
void plDrawWait(U32 sec);
ubyte plSay(const char *msg, U32 persId);
#endif  // ENGINES_THECLOU_PLANING_GRAPHICS_H
