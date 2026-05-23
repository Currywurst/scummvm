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

#include "present/interac.h"
#include "present/interac_internal.h"

void SetBubbleType(uword type)
{
    CurrentBubbleType = type;
}

void SetPictID(uword PictID)
{
    ActivPersonPictID = PictID;
}

ubyte GetExtBubbleActionInfo(void)
{
    return ExtBubbleActionInfo;
}

void SetMenuTimeOutFunc(void (*func) (void))
{
    MenuTimeOutFunc = func;
}

ubyte ChoiceOk(ubyte choice, ubyte exit, LIST * l)
{
    if (choice == GET_OUT)
	return 0;

    if (choice == exit)
	return 0;

    if (l && !LIST_EMPTY(l)) {
	struct ObjectNode *objNode =
	    (struct ObjectNode *) GetNthNode(l, choice);

	if (!objNode->nr && !objNode->type && !objNode->data)
	    return 0;
    }

    return 1;
}

static void DrawMenu(LIST * menu, ubyte nr, S32 mode)
{
    ubyte i;
    char *m1 = nullptr, *m2 = nullptr;
    S32 x = 8, lastx = 0;

    if (mode == ACTIV_POSS)
	gfxSetPens(m_gc, 249, GFX_SAME_PEN, GFX_SAME_PEN);
    else
	gfxSetPens(m_gc, 248, GFX_SAME_PEN, GFX_SAME_PEN);

    for (i = 0; i <= nr; i += 2) {
	m1 = NODE_NAME(GetNthNode(menu, i));

	if ((i + 1) <= nr)
	    m2 = NODE_NAME(GetNthNode(menu, i + 1));
	else
	    m2 = nullptr;

	if (m2) {
	    if (strlen(m1) > strlen(m2)) {
		lastx = gfxTextWidth(m_gc, m1, strlen(m1));
		x += lastx;
	    } else {
		lastx = gfxTextWidth(m_gc, m2, strlen(m2));
		x += lastx;
	    }
	} else {
	    lastx = gfxTextWidth(m_gc, m1, strlen(m1));
	    x += lastx;
	}
    }

    if (nr == (i - 2))
	gfxPrintExact(m_gc, m1, x + 8 * nr - lastx, TXT_1ST_MENU_LINE_Y);
    else {
	if (m2)
	    gfxPrintExact(m_gc, m2, x + 8 * (nr - 1) - lastx,
			  TXT_2ND_MENU_LINE_Y);
    }
}

static char SearchActiv(word delta, ubyte activ, U32 possibility, ubyte max)
{
    do {
	activ += delta;

	if (activ > max)
	    activ = max;

	if (possibility & (1L << activ))
	    return (char) activ;
    } while ((activ > 0) && (activ < max));

    return -1;
}

static char SearchMouseActiv(U32 possibility, ubyte max)
{				/* MOD : 14.12.93 hg */
    int activ;
    U16 x, y;

    gfxGetMouseXY(m_gc, &x, &y);

    if ((y > TXT_1ST_MENU_LINE_Y - 10) && (y < TXT_2ND_MENU_LINE_Y + 10)) {
	max -= 1;

	for (activ = 0; (activ < max / 2 + 1) && (x >= MenuCoords[activ]);
	     activ++);

	activ = (activ - 1) * 2;

	if (y > TXT_1ST_MENU_LINE_Y + 7)
	    activ++;

	if (activ > max)
	    activ = max;

	if (possibility & (1L << activ))
	    return (char) activ;
    }

    return -1;
}

void RefreshMenu(void)
{
    ubyte max, i;

    if (refreshMenu) {
	max = GetNrOfNodes(refreshMenu);

	for (i = 0; i < max; i++) {
	    if (refreshPoss & (1L << i))
		DrawMenu(refreshMenu, i, INACTIV_POSS);
	}

	DrawMenu(refreshMenu, refreshActiv, ACTIV_POSS);
    }
}

ubyte Menu(LIST * menu, U32 possibility, ubyte activ, void (*func) (ubyte),
	   U32 waitTime)
{
    ubyte i;
    S32 action;
    int nextActiv; /* V1112: was char — signed/unsigned mismatch with ubyte activ */
    ubyte max;
    bool ende = false;
    uword x;
    NODE *n;

    if (menu && !LIST_EMPTY(menu)) {
	if (!possibility)
	    return 0;

	x = 0;

	for (max = 0, n = (NODE *)LIST_HEAD(menu); NODE_SUCC(n);
	     n = (NODE *)NODE_SUCC(n), max++) {
	    if ((max % 2) == 0) {
		uword l1 = 0, l2 = 0;

		MenuCoords[max / 2] = x;

		l1 = gfxTextWidth(m_gc, NODE_NAME(n),
				  strlen(NODE_NAME(n))) + 11;

		if (NODE_SUCC(NODE_SUCC(n)))
		    l2 = gfxTextWidth(m_gc, NODE_NAME(NODE_SUCC(n)),
				      strlen(NODE_NAME(NODE_SUCC(n)))) + 11;

		x += max(l1, l2);
	    }
	}

	for (i = 0; i < max; i++) {
	    if (possibility & (1L << i))
		DrawMenu(menu, i, INACTIV_POSS);
	}

	DrawMenu(menu, activ, ACTIV_POSS);

	if (func)
	    func(activ);

	if (waitTime)
	    inpSetWaitTicks(waitTime);

	while (!ende) {
	    action = INP_LEFT | INP_RIGHT | INP_UP | INP_DOWN | INP_LBUTTONP;

	    if (waitTime)
		action |= INP_TIME;

	    action = inpWaitFor(action);

	    if (action & INP_TIME) {
		refreshMenu = nullptr;

		if (MenuTimeOutFunc)
		    MenuTimeOutFunc();
		else
		    return (ubyte) TXT_MENU_TIMEOUT;
	    }

	    if (action & INP_FUNCTION_KEY) {
		refreshMenu = menu;
		refreshPoss = possibility;
		refreshActiv = activ;
		return ((ubyte) - 1);
	    }

	    if ((action & INP_ESC) || (action & INP_RBUTTONP))
		return GET_OUT;

	    if (action & INP_LBUTTONP)
		ende = true;

	    if (action & INP_MOUSE) {	/* MOD : 14.12.93 hg */
		if ((nextActiv =
		     SearchMouseActiv(possibility, max)) != -1) {
		    if (nextActiv != activ) {
			DrawMenu(menu, activ, INACTIV_POSS);
			activ = nextActiv;
			DrawMenu(menu, activ, ACTIV_POSS);

			if (func)
			    func(activ);
		    }
		}
	    } else {
		if ((action & INP_UP) && (activ & 1)) {
		    if ((nextActiv =
			 SearchActiv(-1, activ, possibility,
				     max)) != -1) {
			if (!(nextActiv & 1)) {
			    DrawMenu(menu, activ, INACTIV_POSS);
			    activ = nextActiv;
			    DrawMenu(menu, activ, ACTIV_POSS);

			    if (func)
				func(activ);
			}
		    }
		}

		if ((action & INP_DOWN) && !(activ & 1)) {
		    if ((nextActiv =
			 SearchActiv(+1, activ, possibility,
				     max)) != -1) {
			if (nextActiv & 1) {
			    DrawMenu(menu, activ, INACTIV_POSS);
			    activ = nextActiv;
			    DrawMenu(menu, activ, ACTIV_POSS);

			    if (func)
				func(activ);
			}
		    }
		}

		if (action & INP_LEFT) {
		    if ((nextActiv =
			 SearchActiv(-2, activ, possibility,
				     max)) != -1) {
			DrawMenu(menu, activ, INACTIV_POSS);
			activ = nextActiv;
			DrawMenu(menu, activ, ACTIV_POSS);

			if (func)
			    func(activ);
		    }
		}

		if (action & INP_RIGHT) {
		    if ((nextActiv =
			 SearchActiv(+2, activ, possibility,
				     max)) != -1) {
			DrawMenu(menu, activ, INACTIV_POSS);
			activ = nextActiv;
			DrawMenu(menu, activ, ACTIV_POSS);

			if (func)
			    func(activ);
		    }
		}
	    }
	}

	refreshMenu = nullptr;

	return activ;
    }

    refreshMenu = nullptr;

    return activ;
}

static void DrawBubble(LIST * bubble, U8 firstLine, U8 activ, GC *gc, U32 max)
{
    unsigned i, j;
    char *line = nullptr;

    gfxScreenFreeze();

    gfxSetPens(gc, 224, 224, 224);

    gfxRectFill(gc, X_OFFSET, 3, X_OFFSET + INT_BUBBLE_WIDTH, 49);

    if (firstLine) {
	gfxSetGC(gc);
	gfxShow(145, GFX_OVERLAY | GFX_NO_REFRESH, 0, X_OFFSET + 135, 5);
    }

    if ((max > (firstLine + NRBLINES))) {
	gfxSetGC(gc);
	gfxShow(146, GFX_OVERLAY | GFX_NO_REFRESH, 0, X_OFFSET + 135, 40);
    }

    for (i = firstLine, j = 4;
	 (max < firstLine + NRBLINES) ? i < max : i < firstLine + NRBLINES;
	 i++, j += 9) {
	line = NODE_NAME(GetNthNode(bubble, i));

	if (!line)
	    break;

	if (*line != '*') {
	    gfxSetPens(gc, BG_TXT_COLOR, GFX_SAME_PEN, GFX_SAME_PEN);
	    gfxPrintExact(gc, line, X_OFFSET, j + 1);

	    gfxSetPens(gc, VG_TXT_COLOR, GFX_SAME_PEN, GFX_SAME_PEN);
	    gfxPrintExact(gc, line, X_OFFSET, j);
	} else {
	    line = line + 1;

	    uword bgColor =
		(activ == i) ? BG_ACTIVE_COLOR : BG_INACTIVE_COLOR;
	    uword vgColor =
		(activ == i) ? VG_ACTIVE_COLOR : VG_INACTIVE_COLOR;

	    gfxSetPens(gc, bgColor, GFX_SAME_PEN, GFX_SAME_PEN);
	    gfxPrintExact(gc, line, X_OFFSET + 1, j + 1);

	    gfxSetPens(gc, vgColor, GFX_SAME_PEN, GFX_SAME_PEN);
	    gfxPrintExact(gc, line, X_OFFSET, j);
	}
    }

    gfxScreenThaw(gc, X_OFFSET, 3, INT_BUBBLE_WIDTH, 46);
}


ubyte Bubble(LIST * bubble, ubyte activ, void (*func) (ubyte), U32 waitTime)
{
    ubyte firstVis = 0;
    bool ende = false;
    U32 action;
    S32 max = GetNrOfNodes(bubble);

    SuspendAnim();

    gfxPrepareRefresh();

    gfxScreenFreeze();

    if (activ == GET_OUT)
	activ = 0;

    if (activ >= NRBLINES)
	firstVis = activ - NRBLINES + 1;

    if (func)
	func(activ);

    if (ActivPersonPictID == (uword) - 1)
	gfxShow((uword) CurrentBubbleType, GFX_NO_REFRESH | GFX_OVERLAY, 0,
		-1, -1);
    else {
	gfxShow((uword) ActivPersonPictID,
		GFX_NO_REFRESH | GFX_OVERLAY | GFX_BLEND_UP, 0, -1, -1);
	gfxShow((uword) CurrentBubbleType, GFX_NO_REFRESH | GFX_OVERLAY, 0,
		-1, -1);

	if (CurrentBubbleType == SPEAK_BUBBLE)
	    gfxShow(9, GFX_NO_REFRESH | GFX_OVERLAY, 0, -1, -1);

	if (CurrentBubbleType == THINK_BUBBLE)
	    gfxShow(10, GFX_NO_REFRESH | GFX_OVERLAY, 0, -1, -1);
    }

    gfxSetDrMd(u_gc, GFX_JAM_1);
    gfxSetFont(u_gc, bubbleFont);
    DrawBubble(bubble, firstVis, activ, u_gc, max);

    gfxScreenThaw(u_gc, 0, 0, 320, 67);

    if (setup.CDAudio) {
        PlayFromCDROM();
    }

    if (waitTime) {
	inpSetWaitTicks(waitTime);
	action = inpWaitFor(INP_LBUTTONP | INP_TIME | INP_RBUTTONP);

	if (action & INP_LBUTTONP)
	    activ = 1;

	if ((action & INP_ESC) || (action & INP_RBUTTONP)
	    || (action & INP_TIME))
	    activ = GET_OUT;

	inpSetWaitTicks(0L);

	ExtBubbleActionInfo = action;
    } else {
	while (!ende) {
	    action =
		inpWaitFor(INP_UP | INP_DOWN | INP_LBUTTONP | INP_RBUTTONP
			   | INP_LEFT | INP_RIGHT);

	    ExtBubbleActionInfo = action;

	    if ((action & INP_ESC) || (action & INP_RBUTTONP)) {
		activ = GET_OUT;
		ende = true;
	    }

	    if (!ende) {
		if ((action & INP_LBUTTONP))
		    ende = true;

		if ((action & INP_MOUSE)) {
		    U16 x, y;

		    gfxGetMouseXY(u_gc, &x, &y);

		    if ((x >= X_OFFSET)
			&& (x <= X_OFFSET + INT_BUBBLE_WIDTH)) {
			if ((y < 4) && (firstVis > 0)) {	/* Scroll up */
			    while ((y < 4) && (firstVis > 0)) {
				firstVis -= 1;
				activ = firstVis;

				DrawBubble(bubble, firstVis, activ, u_gc, max);

				if (func)
				    func(activ);

				gfxWaitTOS();

				gfxGetMouseXY(u_gc, nullptr, &y);
			    }
			} else if ((y > 48) && (y <= 52) && (firstVis < (max - 5))) {	/* Scroll down */
			    while ((y > 48) && (y <= 52) && (firstVis < (max - 5))) {
				firstVis += 1;
				activ = firstVis + 4;

				if (activ > (max - 1))
				    activ = max - 1;

				DrawBubble(bubble, firstVis, activ, u_gc, max);

				if (func)
				    func(activ);

				gfxWaitTOS();

				gfxGetMouseXY(u_gc, nullptr, &y);
			    }
			} else if ((y >= 4) && (y <= 48)) {
			    ubyte newactiv = firstVis + (y - 4) / 9;

			    if (newactiv != activ) {
				activ = newactiv;

				if (activ > (max - 1))
				    activ = max - 1;

				if (activ < firstVis)
				    activ = firstVis;

				if (activ > firstVis + 4)
				    activ = firstVis + 4;

				DrawBubble(bubble, firstVis, activ, u_gc, max);

				if (func)
				    func(activ);
			    }
			}
		    }
		} else {
		    if ((action & INP_UP)) {
			if (activ > 0) {
			    int cl = abs(firstVis - activ) + 1;

			    while ((activ > 0) && cl) {
				activ--;
				cl--;

				if (*NODE_NAME(GetNthNode(bubble, activ))
				    == '*')
				    break;
			    }

			    if (activ < firstVis)
				firstVis = activ;

			    DrawBubble(bubble, firstVis, activ, u_gc, max);

			    if (func)
				func(activ);
			}
		    }

		    if ((action & INP_DOWN)) {
			if (activ < max - 1) {
			    int cl = NRBLINES - abs(firstVis - activ);

			    while ((activ < max - 1) && cl) {
				activ++;
				cl--;

				if (*NODE_NAME(GetNthNode(bubble, activ))
				    == '*')
				    break;
			    }

			    if (activ > (firstVis + NRBLINES - 1))
				firstVis = activ - NRBLINES + 1;

			    DrawBubble(bubble, firstVis, activ, u_gc, max);

			    if (func)
				func(activ);
			}
		    }
		}
	    }
	}
    }

    if (activ != GET_OUT) {
	if (*NODE_NAME(GetNthNode(bubble, 0L)) != '*')
	    activ = 0;
    }

    SetBubbleType(SPEAK_BUBBLE);
    SetPictID(MATT_PICTID);

    gfxRefresh();

    ContinueAnim();

    return activ;
}
