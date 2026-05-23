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

#include "gameplay/gp.h"
#include "gameplay/gp_app.h"

void PrintStatus(const char *text)
{
    gfxSetDrMd(m_gc, GFX_JAM_1);
    gfxSetRect(0, 320);

    gfxSetPens(m_gc, 249, 254, GFX_SAME_PEN);

    gfxPrint(m_gc, text, 10, GFX_PRINT_SHADOW | GFX_PRINT_CENTER);
}

U32 ShowTheClouRequester(S32 error_class)
{
    LIST *menu;
    U32 answer = 0;
    ubyte choices;
    Player player = (Player)dbGetObject(Player_Player_1);

    inpTurnESC(0);

    ShowMenuBackground();

    switch (error_class) {
    case No_Error:

	ShowMenuBackground();

	menu = txtGoKey(MENU_TXT, "ESCMenu_STD");

	inpTurnFunctionKey(0);

	if (GamePlayMode & GP_DEMO)
	    choices = Menu(menu, 3, 0, nullptr, 0);
	else
	    choices = Menu(menu, 15, 0, nullptr, 0);

	inpTurnFunctionKey(1);

	switch (choices) {
	case 0:		/* continue playing */
	    player->CurrScene = 0;
	    ShowMenuBackground();
	    tcRefreshLocationInTitle(GetLocation);
	    break;
	case 1:
	    player->CurrScene = SCENE_THE_END;
	    ShowMenuBackground();
	    break;
	case 2:
	    tcSaveTheClou();
	    player->CurrScene = 0;
	    ShowMenuBackground();
	    tcRefreshLocationInTitle(GetLocation);
	    break;
	case 3:		/* load */
	    tcLoadTheClou();
	    ShowMenuBackground();
	    break;
	default:
	    break;
	}

	RemoveList(menu);

	break;
    case Internal_Error:
	PrintStatus("Gravierender Fehler !");
	inpWaitFor(INP_LBUTTONP);
	ShowMenuBackground();
	break;
    case No_Mem:
	PrintStatus("Speichermangel !! - No Memory !!");
	inpWaitFor(INP_LBUTTONP);
	ShowMenuBackground();
	break;
    case Disk_Defect:
	PrintStatus("Disk Defekt !");
	inpWaitFor(INP_LBUTTONP);
	ShowMenuBackground();
	break;
    }

    inpTurnESC(1);

    return (answer);
}
