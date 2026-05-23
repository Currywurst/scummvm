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

#ifndef ENGINES_THECLOU_PLANING_PLANER_H
#define ENGINES_THECLOU_PLANING_PLANER_H


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

extern ubyte AnimCounter;
extern ubyte PlanChanged;


void plPlaner(U32 objId);
#endif  // ENGINES_THECLOU_PLANING_PLANER_H
