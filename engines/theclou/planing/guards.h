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

#ifndef ENGINES_THECLOU_PLANING_GUARDS_H
#define ENGINES_THECLOU_PLANING_GUARDS_H

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

#ifndef ENGINES_THECLOU_PLANING_GUARDS_H
#include "planing/guards.h"
#endif  // ENGINES_THECLOU_PLANING_GUARDS_H

/* method definition */
#define GUARDS_DO_SAVE     1
#define GUARDS_DO_LOAD     2

/* disk definition */
#define GUARD_DISK         0
#define GUARD_EXTENSION    ".gua"
#define GUARD_DIRECTORY    DATA_DIRECTORY


struct System;

struct _GC;

/* main method */
void grdDo(TC_FILE * fh, struct System *sys, LIST * p_PersonsList, U32 p_BurglarsNr,
	   U32 p_PersonsNr, ubyte grdAction);

/* support */
ubyte grdAddToList(U32 bldId, LIST * l);
ubyte grdDraw(struct _GC *gc, U32 bldId, U32 areaId);

/* con- & destructor */
ubyte grdInit(TC_FILE ** fh, const char *mode, U32 bldId, U32 areaId);
void grdDone(TC_FILE * fh);
#endif  // ENGINES_THECLOU_PLANING_GUARDS_H
