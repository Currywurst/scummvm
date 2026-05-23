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

#include "base/base.h"

#include "planing/graphics.h"


void plPrintInfo(char *person)
{
    char info[80];

    snprintf(info, sizeof(info), "%s", &person[1]);

    gfxSetRect(2, 320);
    gfxSetPens(m_gc, 249, GFX_SAME_PEN, GFX_SAME_PEN);
    gfxPrint(m_gc, info, 12, GFX_PRINT_CENTER);
}

void plMessage(const char *msg, ubyte flags)
{
    LIST *m = txtGoKey(PLAN_TXT, msg);

    if (flags & PLANING_MSG_REFRESH)
	ShowMenuBackground();

    if (m)
	plPrintInfo((char *) NODE_NAME(LIST_HEAD(m)));

    RemoveList(m);

    if (flags & PLANING_MSG_WAIT) {
	inpSetWaitTicks(140L);
	inpWaitFor(INP_LBUTTONP | INP_TIME);
	inpSetWaitTicks(0L);
    }
}

void plDisplayTimer(U32 time, ubyte doSpotsImmediatly)
{
    char info[80];
    U32 oldTimer = -1;

    if (!time)
	time = CurrentTimer(plSys) / PLANING_CORRECT_TIME;

    if (GamePlayMode & GP_GUARD_DESIGN) {
	snprintf(info, sizeof(info), "x:%d, y:%d   %s %.2d:%.2d:%.2d %s",
		livGetXPos(Planing_Name[CurrentPerson]),
		livGetYPos(Planing_Name[CurrentPerson]), txtTimer,
		(U32) (time / 3600), (U32) ((time / 60) % 60),
		(U32) (time % 60), txtSeconds);

	gfxSetPens(m_gc, 0, 0, 0);
	gfxRectFill(m_gc, 120, 0, 320, 10);

	gfxSetRect(2, 320);
	gfxSetPens(m_gc, 248, GFX_SAME_PEN, GFX_SAME_PEN);
	gfxPrint(m_gc, info, 2, GFX_PRINT_RIGHT);
    } else {
	snprintf(info, sizeof(info), "%s %.2d:%.2d:%.2d %s", txtTimer, (U32) (time / 3600),
		(U32) ((time / 60) % 60), (U32) (time % 60), txtSeconds);

	gfxSetPens(m_gc, 0, 0, 0);
	gfxRectFill(m_gc, 220, 0, 320, 10);

	gfxSetRect(2, 320);
	gfxSetPens(m_gc, 248, GFX_SAME_PEN, GFX_SAME_PEN);
	gfxPrint(m_gc, info, 2, GFX_PRINT_RIGHT);
    }

    if (doSpotsImmediatly || (oldTimer != CurrentTimer(plSys))) {
	oldTimer = CurrentTimer(plSys);
	lsMoveAllSpots(time);
    }
}

void plDisplayInfo(void)
{
    char info[80];

    dbGetObjectName(OL_NR(GetNthNode(PersonsList, CurrentPerson)), info);

    gfxSetPens(m_gc, 0, 0, 0);
    gfxRectFill(m_gc, 0, 0, 120, 10);

    gfxSetRect(2, 320);
    gfxSetPens(m_gc, 248, GFX_SAME_PEN, GFX_SAME_PEN);
    gfxPrint(m_gc, info, 2, GFX_PRINT_LEFT | GFX_PRINT_SHADOW);
}

ubyte plSay(const char *msg, U32 persId)
{
    LIST *l = txtGoKey(PLAN_TXT, msg);
    ubyte choice;

    SetPictID(((Person) dbGetObject(OL_NR(GetNthNode(PersonsList, persId))))->
	      PictID);

    inpTurnESC(0);
    inpTurnFunctionKey(0);

    choice = Bubble(l, 0, nullptr, 200);

    inpTurnFunctionKey(1);
    inpTurnESC(1);

    RemoveList(l);


    plDisplayTimer(0, 1);
    plDisplayInfo();

    return choice;
}

void plDrawWait(U32 sec)
{
    char time[10];

    snprintf(time, sizeof(time), "%.2d:%.2d", (U32) (sec / 60), (U32) (sec % 60));

    gfxSetDrMd(m_gc, GFX_JAM_2);
    gfxSetPens(m_gc, 248, GFX_SAME_PEN, GFX_SAME_PEN);
    gfxSetRect(0, 320);
    gfxPrint(m_gc, time, 31, GFX_PRINT_CENTER);
    gfxSetDrMd(m_gc, GFX_JAM_1);
}

void plRefresh(U32 ItemId)
{
    LSObject obj = (LSObject) dbGetObject(ItemId);

    if (lsIsLSObjectInActivArea(obj))
	lsFastRefresh(obj);

    if (lsGetStartArea() == lsGetActivAreaID())
	lsShowEscapeCar();	/* repaint car */

    lsRefreshAllLootBags();	/* repaint all LootBags */
}
