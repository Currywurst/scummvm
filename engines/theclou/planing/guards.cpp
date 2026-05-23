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

#include "planing/guards.h"


void grdDo(TC_FILE * fh, struct System *sys, LIST * p_PersonsList, U32 p_BurglarsNr,
	   U32 p_PersonsNr, ubyte grdAction)
{
    ubyte i;

    for (i = p_BurglarsNr; i < p_PersonsNr; i++) {
	switch (grdAction) {
	case GUARDS_DO_SAVE:
	    SaveHandler(fh, sys, OL_NR(GetNthNode(p_PersonsList, i)));
	    break;

	case GUARDS_DO_LOAD:
	    LoadHandler(fh, sys, OL_NR(GetNthNode(p_PersonsList, i)));
	    break;
	}
    }
}

ubyte grdInit(TC_FILE ** fh, const char *mode, U32 bldId, U32 areaId)
{
    char bldName[TXT_KEY_LENGTH], fileName[DSK_PATH_MAX];

    dbGetObjectName(areaId, fileName);
	size_t nameLen = strlen(fileName);
	if (!nameLen)
	    return 0;
	fileName[nameLen - 1] = '\0';
    snprintf(bldName, sizeof(bldName), "%s%s", fileName, GUARD_EXTENSION);

    dskBuildPathName(DISK_CHECK_FILE, GUARD_DIRECTORY, bldName, fileName);

    if ((*fh = dskOpen(fileName, mode)))
	return 1;

    return 0;
}

void grdDone(TC_FILE * fh)
{
    dskClose(fh);
}

ubyte grdAddToList(U32 bldId, LIST * l)
{
    struct ObjectNode *n;

    isGuardedbyAll(bldId, OLF_INCLUDE_NAME | OLF_INSERT_STAR, Object_Police);

    if (!LIST_EMPTY(ObjectList)) {
	for (n = (struct ObjectNode *) LIST_HEAD(ObjectList); NODE_SUCC(n);
	     n = (struct ObjectNode *) NODE_SUCC(n))
	    dbAddObjectNode(l, OL_NR(n), OLF_INCLUDE_NAME | OLF_INSERT_STAR);

	return 1;
    }

    return 0;
}

ubyte grdDraw(GC *gc, U32 bldId, U32 areaId)
{
    TC_FILE *fh;
    ubyte ret = 0;
    LIST *GuardsList = CreateList();

    if (grdAddToList(bldId, GuardsList)) {
	if (grdInit(&fh, "r", bldId, areaId)) {
	    struct System *grdSys;
	    U32 GuardsNr, i;
	    uword xpos = 0, ypos = 0;

	    GuardsNr = GetNrOfNodes(GuardsList);

	    grdSys = InitSystem();

	    for (i = 0; i < GuardsNr; i++)
		InitHandler(grdSys, OL_NR(GetNthNode(GuardsList, i)),
			    SHF_AUTOREVERS);

	    grdDo(fh, grdSys, GuardsList, 0, GuardsNr, GUARDS_DO_LOAD);
	    grdDone(fh);

	    dbSortObjectList(&GuardsList, dbStdCompareObjects);

	    for (i = 0; i < GuardsNr; i++) {
		if (areaId ==
		    isGuardedbyGet(bldId, OL_NR(GetNthNode(GuardsList, i)))) {
		    struct Handler *h =
			FindHandler(grdSys, OL_NR(GetNthNode(GuardsList, i)));
		    struct Action *action;

		    /* getting start coordinates */
		    switch (i) {
		    case 0:
		    case 2:
			xpos = (((LSArea) dbGetObject(areaId))->us_StartX4) / 2;
			ypos = (((LSArea) dbGetObject(areaId))->us_StartY4) / 2;
			break;

		    case 1:
		    case 3:
			xpos = (((LSArea) dbGetObject(areaId))->us_StartX5) / 2;
			ypos = (((LSArea) dbGetObject(areaId))->us_StartY5) / 2;
			break;
		    }

		    xpos += 4;

		    gfxMoveCursor(gc, xpos, ypos);

		    /* drawing system */
		    for (action = (struct Action *) LIST_HEAD(h->Actions);
			 NODE_SUCC(action);
			 action = (struct Action *) NODE_SUCC(action)) {
			switch (action->Type) {
			case ACTION_GO:
			    switch ((ActionData(action, struct ActionGo *))->
				    Direction) {
			    case DIRECTION_LEFT:
				xpos -= action->TimeNeeded;
				break;

			    case DIRECTION_RIGHT:
				xpos += action->TimeNeeded;
				break;

			    case DIRECTION_UP:
				ypos -= action->TimeNeeded;
				break;

			    case DIRECTION_DOWN:
				ypos += action->TimeNeeded;
				break;
			    }

			    gfxDraw(gc, xpos, ypos);
			    break;

			case ACTION_WAIT:
			case ACTION_OPEN:
			case ACTION_CLOSE:
			case ACTION_CONTROL:
			    /*DrawCircle(rp, xpos, ypos, 1);*/
			    break;
			}
		    }
		}
	    }

	    ret = 1;

	    for (i = 0; i < GuardsNr; i++)
		CloseHandler(grdSys, OL_NR(GetNthNode(GuardsList, i)));

	    CloseSystem(grdSys);
	}
    }

    RemoveList(GuardsList);

    return ret;
}
