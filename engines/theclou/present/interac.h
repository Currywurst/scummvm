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

#ifndef ENGINES_THECLOU_PRESENT_INTERAC_H
#define ENGINES_THECLOU_PRESENT_INTERAC_H

#include "theclou.h"
#include "list/list.h"
#include "memory/memory.h"
#include "gfx/gfx.h"
#include "text/text.h"

#define X_OFFSET    112
#define NRBLINES    5U

#define BG_ACTIVE_COLOR   252
#define VG_ACTIVE_COLOR   254

#define BG_TXT_COLOR      252
#define VG_TXT_COLOR      254

#define BG_BAR_COLOR      230
#define VG_BAR_COLOR      230

#define TXT_MENU_TIMEOUT        (-2)

extern ubyte ChoiceOk(ubyte choice, ubyte exit, LIST * l);

/* Bubble() returns the selected item index (ubyte).
 * V1071: Many call sites intentionally discard the return value when Bubble
 * is used purely for display (info/announcement bubbles with only one entry).
 * Use (void)Bubble(...) at those sites to document the intentional discard. */
extern ubyte Bubble(LIST * bubble, ubyte activ, void (*func) (ubyte),
		    U32 waitTime);
extern ubyte Menu(LIST * menu, U32 possibility, ubyte activ,
		  void (*func) (ubyte), U32 waitTime);

extern void RefreshMenu(void);

extern void SetBubbleType(uword type);
extern void SetPictID(uword PictID);
void SetMenuTimeOutFunc(void (*func) (void));

extern ubyte GetExtBubbleActionInfo(void);

#endif  // ENGINES_THECLOU_PRESENT_INTERAC_H
