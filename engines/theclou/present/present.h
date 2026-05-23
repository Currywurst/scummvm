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

#ifndef ENGINES_THECLOU_PRESENT_PRESENT_H
#define ENGINES_THECLOU_PRESENT_PRESENT_H

#include <math.h>

#include "theclou.h"
#include "list/list.h"
#include "text/text.h"

#include "gfx/gfx.h"
#include "data/database.h"
#include "data/relation.h"

#include "data/objstd/tcdata.h"
#include "data/dataappl.h"
#include "scenes/scenes.h"
#include "landscap/landscap.h"

#define PRESENT_AS_TEXT       1
#define PRESENT_AS_BAR        2
#define PRESENT_AS_NUMBER     3

struct _GC;

extern ubyte Present(U32 nr, const char *presentationText,
		     void (*initPresentation) (U32, LIST *, LIST *));

extern void InitPersonPresent(U32 nr, LIST * presentationData, LIST * texts);
extern void InitCarPresent(U32 nr, LIST * presentationData, LIST * texts);
extern void InitPlayerPresent(U32 nr, LIST * presentationData, LIST * texts);
extern void InitBuildingPresent(U32 nr, LIST * presentationData, LIST * texts);
extern void InitToolPresent(U32 nr, LIST * presentationData, LIST * texts);
extern void InitObjectPresent(U32 nr, LIST * presentationData, LIST * texts);
extern void InitEvidencePresent(U32 nr, LIST * presentationData, LIST * texts);
extern void InitLootPresent(U32 nr, LIST * presentationData, LIST * texts);
extern void InitOneLootPresent(U32 nr, LIST * presentationData, LIST * texts);

extern void prSetBarPrefs(struct _GC *gc, uword us_BarWidth,
			  uword us_BarHeight, ubyte uch_FCol,
			  ubyte uch_BCol, ubyte uch_TCol);
extern void prDrawTextBar(char *puch_Text, U32 ul_Value, U32 ul_Max,
			  uword us_XPos, uword us_YPos);

extern void DrawPresent(LIST * present, U8 firstLine, struct _GC *gc, U8 max);
void AddPresentTextLine(LIST * l, const char *data, U32 max, LIST * texts,
			U16 textNr);
void AddPresentLine(LIST * l, U8 presentHow, U32 data, U32 max,
		    LIST * texts, U16 textNr);

#endif  // ENGINES_THECLOU_PRESENT_PRESENT_H
