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
#include "organisa/organisa.h"

/* Forward declarations for functions defined later in this file */
static void tcDisplayCommon(void);
static void tcDisplayPerson(U32 displayMode);
static void tcDisplayAbilities(U32 personNr, U32 displayData);

#define   ORG_DISP_GUY_Y           60
#define   ORG_DISP_ABILITIES_Y     75
#define   ORG_DISP_GUY_WIDTH       80
#define   ORG_DISP_LINE            10

#define   ORG_PICT_ID				  3

static void RefreshDisplayConfig(void)
{
    gfxScreenFreeze();

    gfxSetGC(l_gc);
    gfxShow(ORG_PICT_ID, GFX_NO_REFRESH | GFX_BLEND_UP, 0, -1, -1);

    gfxScreenThaw(l_gc, 0, 0, 320, 140);

    CurrentBackground = BGD_PLANUNG;
    ShowMenuBackground();
}

void tcInitDisplayOrganisation(void)
{
    gfxChangeColors(l_gc, 5, GFX_FADE_OUT, 0);
    gfxClearArea(l_gc);

    RefreshDisplayConfig();
}

void tcDoneDisplayOrganisation(void)
{
    gfxChangeColors(l_gc, 5, GFX_FADE_OUT, 0);
    gfxClearArea(l_gc);
    gfxSetPens(m_gc, GFX_SAME_PEN, GFX_SAME_PEN, 0);
}

void tcDisplayOrganisation(void)
{
    gfxScreenFreeze();

    tcDisplayCommon();
    tcDisplayPerson(ORG_DISP_ABILITIES);

    gfxScreenThaw(l_gc, 0, 0, 320, 140);
}

static void tcDisplayCommon(void)
{
    LIST *texts;
    char line[TXT_KEY_LENGTH], name[TXT_KEY_LENGTH];
    Building building = nullptr;

    texts = txtGoKey(BUSINESS_TXT, "PLAN_COMMON_DATA");

    gfxSetGC(l_gc);
    gfxShow(ORG_PICT_ID, GFX_ONE_STEP | GFX_NO_REFRESH, 0, -1, -1);

    /* Gebäude anzeigen  */

    gfxSetFont(l_gc, menuFont);
    gfxSetDrMd(l_gc, GFX_JAM_1);
    gfxSetRect(0, 320);

    gfxSetPens(l_gc, 249, 254, GFX_SAME_PEN);

    if (Organisation.BuildingID)
        building = (Building) dbGetObject(Organisation.BuildingID);

    if (Organisation.BuildingID && building) {
        dbGetObjectName(Organisation.BuildingID, line);
        gfxPrint(l_gc, line, 9, GFX_PRINT_CENTER | GFX_PRINT_SHADOW);
    }


    gfxSetFont(l_gc, bubbleFont);
    gfxSetPens(l_gc, 249, GFX_SAME_PEN, GFX_SAME_PEN);

	/*************************************************************
	 *  Fluchtwagen anzeigen
	 */

    gfxSetRect(0, 106);
    strcpy(line, NODE_NAME(GetNthNode(texts, 0L)));

    if (Organisation.CarID) {
	dbGetObjectName(Organisation.CarID, name);
	tcCutName(name, (ubyte) ' ', 12);
	strcat(line, name);
    } else
	strcat(line, " ? ");

    gfxPrint(l_gc, line, 25, GFX_PRINT_LEFT);

	/*************************************************************
	 *  Zulassung anzeigen
	 */

    gfxSetRect(106, 106);
    strcpy(line, NODE_NAME(GetNthNode(texts, 2L)));

    if (Organisation.CarID) {
	snprintf(name, sizeof(name), "%d", Organisation.PlacesInCar);
	strcat(line, name);
    } else
	strcat(line, " ? ");

    gfxPrint(l_gc, line, 25, GFX_PRINT_CENTER);

	/*************************************************************
	 *  Fahrer anzeigen
	 */

    gfxSetRect(212, 106);
    strcpy(line, NODE_NAME(GetNthNode(texts, 1L)));

    if (Organisation.DriverID) {
	dbGetObjectName(Organisation.DriverID, name);
	strcat(line, name);
    } else
	strcat(line, " ? ");

    gfxPrint(l_gc, line, 25, GFX_PRINT_RIGHT);

	/*************************************************************
	 *  Fluchtweg
	 */

    gfxSetRect(0, 106);
    strcpy(line, NODE_NAME(GetNthNode(texts, 3L)));

    if (Organisation.BuildingID && building) {
	LIST *enums = txtGoKey(OBJECTS_ENUM_TXT, "enum_RouteE");

	strcpy(name, NODE_NAME(GetNthNode(enums, building->EscapeRoute)));
	strcat(line, name);

	RemoveList(enums);
    } else
	strcat(line, " ? ");

    gfxPrint(l_gc, line, 35, GFX_PRINT_LEFT);

	/*************************************************************
	 *  Fluchtweg-länge
	 */

    gfxSetRect(106, 106);
    strcpy(line, NODE_NAME(GetNthNode(texts, 4L)));

    if (Organisation.BuildingID && building) {
	snprintf(name, sizeof(name), "%d (km)", building->EscapeRouteLength);
	strcat(line, name);
    } else
	strcat(line, " ? ");

    gfxPrint(l_gc, line, 35, GFX_PRINT_CENTER);

	/*************************************************************
	 *  mein Anteil
	 */

    gfxSetRect(212, 106);
    strcpy(line, NODE_NAME(GetNthNode(texts, 5L)));

    snprintf(name, sizeof(name), " %" PRId32 "%%", tcCalcMattsPart());
    strcat(line, name);

    gfxPrint(l_gc, line, 35, GFX_PRINT_RIGHT);

    RemoveList(texts);
}

static void tcDisplayPerson(U32 displayMode)
{
    U32 objNr, i;
    NODE *node;
    char line[TXT_KEY_LENGTH];
    LIST *guys;

    joined_byAll(Person_Matt_Stuvysunt, OLF_INCLUDE_NAME | OLF_PRIVATE_LIST,
		 Object_Person);
    guys = ObjectListPrivate;
    dbSortObjectList(&guys, dbStdCompareObjects);

    for (node = (NODE *)LIST_HEAD(guys), i = 0; NODE_SUCC(node);
	 node = NODE_SUCC(node), i++) {
	objNr = ((struct ObjectNode *) node)->nr;

	if (strlen(NODE_NAME(node)) >= 16)
	    tcGetLastName(NODE_NAME(node), line, 15);
	else
	    strcpy(line, NODE_NAME(node));

	gfxSetRect(ORG_DISP_GUY_WIDTH * i + 5, ORG_DISP_GUY_WIDTH - 5);
	gfxSetPens(l_gc, 248, GFX_SAME_PEN, GFX_SAME_PEN);
	gfxSetFont(l_gc, menuFont);
	gfxSetDrMd(l_gc, GFX_JAM_1);
	gfxPrint(l_gc, line, ORG_DISP_GUY_Y, GFX_PRINT_CENTER);
	gfxSetFont(l_gc, bubbleFont);

	if (displayMode & ORG_DISP_ABILITIES)
	    tcDisplayAbilities(objNr, i);
    }

    RemoveList(guys);
}

static void tcDisplayAbilities(U32 personNr, U32 displayData)
{
    LIST *abilities;
    NODE *node;
    U32 abiNr, ability;
    unsigned i;

    hasAll(personNr, OLF_PRIVATE_LIST | OLF_INCLUDE_NAME, Object_Ability);
    abilities = ObjectListPrivate;

    prSetBarPrefs(l_gc, ORG_DISP_GUY_WIDTH - 5, ORG_DISP_LINE + 1, 251,
		  250, 249);

    if (!(LIST_EMPTY(abilities))) {
	for (node = (NODE *)LIST_HEAD(abilities), i = 0; NODE_SUCC(node);
	     node = NODE_SUCC(node), i++) {
	    char line[TXT_KEY_LENGTH];

	    abiNr = ((struct ObjectNode *) GetNthNode(abilities, (U32) i))->nr;
	    ability = hasGet(personNr, abiNr);

	    snprintf(line, sizeof(line), "%s %d%%", NODE_NAME(node),
		    (uword) ((ability * 100) / 255));

	    prDrawTextBar(line, ability, 255L,
			  displayData * ORG_DISP_GUY_WIDTH + 5,
			  ORG_DISP_ABILITIES_Y + i * (ORG_DISP_LINE + 4));
	}
    } else {
	char line[TXT_KEY_LENGTH];

	txtGetFirstLine(BUSINESS_TXT, "PLAN_NO_CAPABILITY", line);
	gfxSetRect(ORG_DISP_GUY_WIDTH + 5, ORG_DISP_GUY_WIDTH - 5);
	gfxSetDrMd(l_gc, GFX_JAM_1);
	gfxPrint(l_gc, line, ORG_DISP_ABILITIES_Y + ORG_DISP_LINE,
		 GFX_PRINT_LEFT);
    }

    RemoveList(abilities);
}
