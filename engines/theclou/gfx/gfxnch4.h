/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * Original game code (c) 1994 ...and avoid panic by
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

/** @file gfx/gfxnch4.h — NCH4 (256-colour) graphics backend for Der Clou! */

#ifndef ENGINES_THECLOU_GFX_GFXNCH4_H
#define ENGINES_THECLOU_GFX_GFXNCH4_H

#include "theclou.h"

#include "gfx/gfx.h"

#define GFX_NCH4_SCROLLOFFSET 	(160 * 72)	/* memory used by the fixed display */

#define ScrLEFT 0
#define ScrRIGHT 639
#define ScrTOP 0
#define ScrBOTTOM 255

struct RastPort;

extern U32 gfxNCH4GetCurrScrollOffset(void);
void gfxSetDarkness(ubyte value);

void gfxNCH4Refresh(void);

extern void gfxNCH4SetViewPort(int x, int y);
extern void gfxNCH4Scroll(int x, int y);
extern void gfxNCH4SetSplit(uword line);
void gfxLSInit(void);

void gfxLSPut(MemRastPort *sp, U16 sx, U16 sy, U16 dx, U16 dy, U16 w, U16 h);
void gfxLSPutMsk(MemRastPort *sp, U16 sx, U16 sy, U16 dx, U16 dy, U16 w, U16 h);
void gfxLSOverlay(MemRastPort *sp, U16 sx, U16 sy, U16 dx, U16 dy, U16 w, U16 h);
void gfxLSOverlayMask(MemRastPort *sp, U16 sx, U16 sy, U16 dx, U16 dy, U16 w, U16 h);
void gfxLSPutClr(MemRastPort *sp, U16 sx, U16 sy, U16 dx, U16 dy, U16 w, U16 h);
void gfxLSPutSet(MemRastPort *sp, U16 sx, U16 sy, U16 dx, U16 dy, U16 w, U16 h);

void gfxLSRectFill(U16 sx, U16 sy, U16 ex, U16 ey, U8 color);
U8 gfxLSReadPixel(U16 x, U16 y);
#endif // ENGINES_THECLOU_GFX_GFXNCH4_H
