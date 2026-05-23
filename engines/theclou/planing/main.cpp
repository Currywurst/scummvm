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

#include "planing/main.h"


struct System *plSys = nullptr;

char txtTooLoud[20];
char txtTimer[20];
char txtWeight[20];
char txtSeconds[20];


/* System functions */
void plInit(void)
{
    LIST *l;

    /* Get texts */
    l = txtGoKey(PLAN_TXT, "TXT_TOO_LOUD");
    snprintf(txtTooLoud, sizeof(txtTooLoud), "%s", NODE_NAME(LIST_HEAD(l)));
    RemoveList(l);

    l = txtGoKey(PLAN_TXT, "TXT_TIMER");
    snprintf(txtTimer, sizeof(txtTimer), "%s", NODE_NAME(LIST_HEAD(l)));
    RemoveList(l);

    l = txtGoKey(PLAN_TXT, "TXT_WEIGHT");
    snprintf(txtWeight, sizeof(txtWeight), "%s", NODE_NAME(LIST_HEAD(l)));
    RemoveList(l);

    l = txtGoKey(PLAN_TXT, "TXT_SECONDS");
    snprintf(txtSeconds, sizeof(txtSeconds), "%s", NODE_NAME(LIST_HEAD(l)));
    RemoveList(l);

    plSys = InitSystem();
}

void plDone(void)
{
    CloseSystem(plSys);
}
