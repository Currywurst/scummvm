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

#ifndef ENGINES_THECLOU_LANDSCAP_SPOT_H
#define ENGINES_THECLOU_LANDSCAP_SPOT_H

#include "theclou.h"
#include "list/list.h"
#include "text/text.h"
#include "gfx/gfx.h"
#include "memory/memory.h"
#include "disk/disk.h"

#define LS_SPOT_NAME					"s"

#define LS_SPOT_SMALL_SIZE         16
#define LS_SPOT_MEDIUM_SIZE        32
#define LS_SPOT_LARGE_SIZE         48

#define LS_SPOT_ON                  1
#define LS_SPOT_OFF                 2

#define LS_ALL_VISIBLE_SPOTS        1
#define LS_ALL_INVISIBLE_SPOTS      2

struct SpotPosition {
    NODE Link;

    uword us_XPos;
    uword us_YPos;
};

struct Spot {
    NODE Link;

    uword us_Size;
    uword us_Speed;		/* secconds per move */

    U32 ul_CtrlObjId;		/* data : objId */

    U32 ul_AreaId;

    ubyte uch_Status;

    uword us_OldXPos;
    uword us_OldYPos;

    uword us_PosCount;

    LIST *p_positions;

    struct SpotPosition *p_CurrPos;	/* for fast access */
};

extern void lsInitSpots(void);
extern void lsDoneSpots(void);

extern void lsMoveAllSpots(U32 time);
extern void lsShowAllSpots(U32 time, U32 mode);
extern void lsShowSpot(struct Spot *s, U32 time);
extern void lsHideSpot(struct Spot *s);

extern void lsBlitSpot(uword us_Size, uword us_XPos, uword us_YPos,
		       ubyte visible);

extern void lsSetSpotStatus(U32 CtrlObjId, ubyte uch_Status);

extern void lsAddSpotPosition(struct Spot *spot, uword us_XPos, uword us_YPos);
extern void lsLoadSpots(U32 bldId, char *uch_FileName);
extern void lsWriteSpots(char *uch_FileName);
extern void lsFreeAllSpots(void);
extern void lsRemSpot(struct Spot *spot);

extern LIST *lsGetSpotList(void);

extern struct Spot *lsAddSpot(uword us_Size, uword us_Speed, U32 ul_CtrlObjId);
extern struct Spot *lsGetSpot(char *uch_Name);

#endif  // ENGINES_THECLOU_LANDSCAP_SPOT_H
