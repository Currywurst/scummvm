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

#ifndef ENGINES_THECLOU_LANDSCAP_LANDSCAP_H
#define ENGINES_THECLOU_LANDSCAP_LANDSCAP_H

#include "theclou.h"
#include "error/error.h"
#include "list/list.h"
#include "memory/memory.h"
#include "disk/disk.h"
#include "inphdl/inphdl.h"
#include "gfx/gfx.h"
#include "gfx/gfxnch4.h"
#include "text/text.h"
#include "data/relation.h"
#include "data/database.h"

#include "data/objstd/tcdata.h"
#include "living/living.h"
#include "landscap/spot.h"

#define   REL_CONSIST_OFFSET       3
#define   REL_HAS_LOCK_OFFSET      4
#define   REL_HAS_ALARM_OFFSET     5
#define   REL_HAS_POWER_OFFSET     6
#define   REL_HAS_LOOT_OFFSET      7
#define   REL_HAS_ROOM_OFFSET     10

#define   LS_OBJECT_VISIBLE   	((ubyte) 1)
#define   LS_OBJECT_INVISIBLE 	((ubyte) 0)


#define   LS_SCROLL_CHECK     	  0
#define   LS_SCROLL_PREPARE   	  1

/* Scroll defines */
#define   LS_SCROLL_LEFT				1
#define   LS_SCROLL_RIGHT		     	2
#define   LS_SCROLL_UP		   	  	4
#define   LS_SCROLL_DOWN		     	8

#define   LS_NO_COLLISION     		0
#define   LS_COLLISION        		1


#define   LS_MAX_AREA_WIDTH        640
#define   LS_MAX_AREA_HEIGHT       256	/* so that wall fits, also in the last line */

#define   LS_FLOOR_X_SIZE           32
#define   LS_FLOOR_Y_SIZE           32

#define   LS_STD_SCROLL_SPEED        2
#define   LS_STD_MAX_PERSONS         7

#define   LS_PERSON_X_SIZE          14
#define   LS_PERSON_Y_SIZE          14

#define   LS_PERSON_ACCESS_FOCUS_X   7
#define   LS_PERSON_ACCESS_FOCUS_Y   7

#define   LS_DARKNESS              ((ubyte)255)
#define   LS_BRIGHTNESS            ((ubyte)0)
#define   LS_DARK_FUNNY            ((ubyte)85)

#define	LS_PC_CORRECT_X				(-9)	/* when sign changes landscap.c */
#define  LS_PC_CORRECT_Y				(19)

/* Coll Mode */
#define   LS_COLL_PLAN                 1
#define   LS_COLL_NORMAL               2
#define   LS_LEVEL_DESIGNER            4

#define   lsGetOldState(lso)           ((((lso)->ul_Status) >> 16) & 0xffff)
#define   lsGetNewState(lso)           (((lso)->ul_Status) & 0xffff)
#define   lsSetOldState(lso)           ((lso)->ul_Status = (((lso)->ul_Status << 16) & 0xffff0000) + (((lso)->ul_Status) & 0xffff))

/* globale functions ! */
extern void lsInitLandScape(U32 bID, ubyte mode);
extern void lsDoneLandScape(void);

extern void lsSetScrollSpeed(ubyte pixel);
extern void lsSetActivLiving(char *Name, uword x, uword y);

extern void lsRefreshObjectList(U32 areaID);

extern U32 lsAddLootBag(uword x, uword y, ubyte bagNr);	/* bagNr : 1 - 8! */
extern void lsRemLootBag(U32 bagId);
extern void lsRefreshAllLootBags(void);

extern void lsShowEscapeCar(void);

extern void lsFastRefresh(LSObject lso);

extern void lsSetObjectState(U32 objID, ubyte bitNr, ubyte value);

extern void lsSetViewPort(uword x, uword y);

extern void lsSetDarkness(ubyte value);
extern void lsSetCollMode(ubyte collMode);

extern LIST *lsGetRoomsOfArea(U32 ul_AreaId);

extern void lsSetObjectRetrievalList(U32 ul_AreaId);
extern U32 lsGetCurrObjectRetrieval(void);

extern U32 lsGetObjectState(U32 objID);
extern uword lsGetObjectCount(void);
extern ubyte lsGetLoudness(uword x, uword y);

extern void lsDoScroll(void);
extern void lsScrollCorrectData(S32 dx, S32 dy);
extern ubyte lsScrollLandScape(ubyte direction);
extern ubyte lsInitScrollLandScape(ubyte direction, ubyte mode);

extern LIST *lsGetObjectsByList(uword x, uword y, uword width, uword height,
				ubyte showInvisible, ubyte addLootBags);

void lsInitActivArea(U32 areaID, uword x, uword y, char *livingName);
extern void lsDoneActivArea(U32 newAreaID);
extern void lsTurnObject(LSObject lso, ubyte status, ubyte Collis);

extern void lsCalcExactSize(LSObject lso, uword * x0, uword * y0, uword * x1,
			    uword * y1);

extern void lsInitDoorRefresh(U32 ObjId);
extern void lsDoDoorRefresh(LSObject lso);

extern uword lsGetTotalXPos(void);
extern uword lsGetTotalYPos(void);
extern uword lsGetWindowXPos(void);
extern uword lsGetWindowYPos(void);
extern uword lsGetFloorIndex(uword x, uword y);

extern void lsGuyInsideSpot(uword * us_XPos, uword * us_YPos, U32 * areaId);

extern bool lsIsObjectAWall(LSObject lso);
extern bool lsIsObjectAnAddOn(LSObject lso);
extern bool lsIsObjectADoor(LSObject lso);
extern bool lsIsObjectAStdObj(LSObject lso);
extern bool lsIsObjectSpecial(LSObject lso);

extern bool lsIsLSObjectInActivArea(LSObject lso);

extern ubyte lsIsCollision(S32 x, S32 y, ubyte direction);	/* sprite needs it */

extern U32 lsGetCurrBuildingID(void);
extern U32 lsGetActivAreaID(void);

extern void lsInitObjectDB(U32 bld, U32 areaID);
extern void lsLoadGlobalData(U32 bld, U32 ul_AreaId);
extern void lsDoneObjectDB(U32 areaID);
extern void lsInitRelations(U32 areaID);
extern void lsSetRelations(U32 areaID);

extern void lsBlitFloor(uword floorIndex, uword destx, uword desty);

extern void lsWalkThroughWindow(LSObject lso, uword us_LivXPos,
				uword us_LivYPos, uword * us_XPos,
				uword * us_YPos);

/* internal functions ! */
extern void lsPatchObjects(void);

extern void lsSetVisibleWindow(uword x, uword y);

extern ubyte lsIsInside(LSObject lso, uword x, uword y, uword x1, uword y1);

extern U32 lsGetStartArea(void);

extern void lsInitObjects(void);

extern void lsBuildScrollWindow(void);
extern void lsShowAlarmAndPower(LSObject lso, U32 x, U32 y);

extern S32 lsShowOneObject(LSObject lso, word destx, word desty,
			   U32 ObjTypeFilter);

void  lsSafeRectFill(U16 x0, U16 y0, U16 x1, U16 y1, U8 color);

/* external functions */
extern void lsInitGfx(void);
extern void lsCloseGfx(void);

extern U32 ConsistOfRelationID;
extern U32 hasLockRelationID;
extern U32 hasAlarmRelationID;
extern U32 hasPowerRelationID;
extern U32 hasLootRelationID;
extern U32 hasRoomRelationID;

#endif  // ENGINES_THECLOU_LANDSCAP_LANDSCAP_H
