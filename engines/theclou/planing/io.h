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

#ifndef ENGINES_THECLOU_PLANING_IO_H
#define ENGINES_THECLOU_PLANING_IO_H

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

#ifndef ENGINES_THECLOU_PLANING_IO_H
#include "planing/io.h"
#endif  // ENGINES_THECLOU_PLANING_IO_H
#include "planing/prepare.h"
#include "planing/support.h"
#include "planing/system.h"
#include "planing/sync.h"
#include "planing/guards.h"


/* Extensions */
#define PLANING_PLAN_EXTENSION       ".pln"
#define PLANING_PLAN_LIST_EXTENSION  ".pll"

/* Disks */
#define PLANING_PLAN_DISK         	  0

/* Tool sequences in file */
#define PLANING_PLAN_TOOL_BEGIN_ID  "TOOB"	/* TOOl Begin */
#define PLANING_PLAN_TOOL_END_ID    "TOOE"	/* TOOl End */

/* open cmd */
#define PLANING_OPEN_READ_PLAN        0
#define PLANING_OPEN_WRITE_PLAN       1
#define PLANING_OPEN_READ_BURGLARY    2

/* open error */
#define PLANING_OPEN_OK               0
#define PLANING_OPEN_ERR_NO_CHOICE    1
#define PLANING_OPEN_ERR_NO_PLAN      2


ubyte plOpen(U32 objId, ubyte mode, TC_FILE ** fh);
void plLoad(U32 objId);
void plSave(U32 objId);
void plSaveChanged(U32 objId);

LIST *plLoadTools(TC_FILE * fh);
void plSaveTools(TC_FILE * fh);
#endif  // ENGINES_THECLOU_PLANING_IO_H
