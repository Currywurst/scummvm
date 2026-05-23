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

#include "scenes/scenes.h"

char *tcShowPriceOfTool(U32 nr, U32 type, void *data)
{
    static char line[TXT_KEY_LENGTH];
    char line1[TXT_KEY_LENGTH];
    Tool tool = (Tool) data;

    txtGetFirstLine(BUSINESS_TXT, "PRICE_AND_MONEY", line1);
    snprintf(line, sizeof(line), line1, tool->Value);

    return line;
}

ubyte tcBuyTool(ubyte choice)
{
    LIST *tools;
    NODE *node;
    U32 toolID, price, count;
    Person mary = (Person)dbGetObject(Person_Mary_Bolton);
    Tool tool;
    U8 oldChoice = GET_OUT;
    char exp[TXT_KEY_LENGTH];

    ObjectListSuccString = tcShowPriceOfTool;
    ObjectListWidth = 48;

    hasAll(Person_Mary_Bolton,
	   OLF_ALIGNED | OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR |
	   OLF_ADD_SUCC_STRING, Object_Tool);
    tools = ObjectListPrivate;

    ObjectListSuccString = nullptr;
    ObjectListWidth = 0;

    txtGetFirstLine(BUSINESS_TXT, "THANKS", exp);
    ExpandObjectList(tools, exp);

    SetBubbleType(THINK_BUBBLE);

    count = GetNrOfNodes(tools) - 1;

    choice = min(choice, count);

    while (choice != GET_OUT) {
	SetPictID(MATT_PICTID);

	oldChoice = choice;

	if (ChoiceOk(choice = Bubble(tools, choice, 0L, 0L), GET_OUT, tools)) {
	    node = (NODE *)GetNthNode(tools, (U32) choice);
	    toolID = OL_NR(node);
	    tool = (Tool) dbGetObject(toolID);
	    price = tcGetToolPrice(tool);

	    if (has(Person_Matt_Stuvysunt, toolID))
		Say(BUSINESS_TXT, 0, mary->PictID, "AUSVERKAUFT");
	    else {
		if (tcSpendMoney(price, 0)) {
		    hasSet(Person_Matt_Stuvysunt, toolID);
		    Say(BUSINESS_TXT, 0, mary->PictID, "GOOD TOOL");
		}
	    }
	} else
	    choice = GET_OUT;
    }

    RemoveList(tools);

    return (oldChoice);
}

ubyte tcDescTool(ubyte choice)
{
    LIST *tools, *desc;
    char line[TXT_KEY_LENGTH], exp[TXT_KEY_LENGTH];
    U8 oldChoice = GET_OUT;
    Person mary = (Person) dbGetObject(Person_Mary_Bolton);

    ObjectListSuccString = tcShowPriceOfTool;
    ObjectListWidth = 48;

    hasAll(Person_Mary_Bolton,
	   OLF_ALIGNED | OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR |
	   OLF_ADD_SUCC_STRING, Object_Tool);
    tools = ObjectListPrivate;

    ObjectListWidth = 0;
    ObjectListSuccString = nullptr;

    txtGetFirstLine(BUSINESS_TXT, "THANKS", exp);
    ExpandObjectList(tools, exp);

    choice = min(choice, (GetNrOfNodes(tools) - 1));

    while (choice != GET_OUT) {
	SetPictID(MATT_PICTID);

	oldChoice = choice;

	if (ChoiceOk(choice = Bubble(tools, choice, 0L, 0L), GET_OUT, tools)) {
	    dbGetObjectName(OL_NR(GetNthNode(tools, (U32) choice)), line);

	    desc = txtGoKey(TOOLS_TXT, line);

	    SetPictID(mary->PictID);
	    (void)Bubble(desc, 0, 0L, 0L); /* V1071: display-only, result intentionally discarded */

	    RemoveList(desc);
	} else
	    choice = GET_OUT;
    }

    RemoveList(tools);

    return (oldChoice);
}

ubyte tcShowTool(ubyte choice)
{
    LIST *tools;
    NODE *node;
    U32 toolID;
    U8 oldChoice = GET_OUT;
    char exp[TXT_KEY_LENGTH];

    ObjectListSuccString = tcShowPriceOfTool;
    ObjectListWidth = 48;

    hasAll(Person_Mary_Bolton,
	   OLF_ALIGNED | OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR |
	   OLF_ADD_SUCC_STRING, Object_Tool);
    tools = ObjectListPrivate;

    ObjectListSuccString = nullptr;
    ObjectListWidth = 0;

    txtGetFirstLine(BUSINESS_TXT, "THANKS", exp);
    ExpandObjectList(tools, exp);

    choice = min(choice, (GetNrOfNodes(tools) - 1));

    while (choice != GET_OUT) {
	SetPictID(MATT_PICTID);

	oldChoice = choice;

	if (ChoiceOk(choice = Bubble(tools, choice, 0L, 0L), GET_OUT, tools)) {
	    node = (NODE *)GetNthNode(tools, (U32) choice);
	    toolID = OL_NR(node);

	    Present(toolID, "Tool", InitToolPresent);
	} else
	    choice = GET_OUT;
    }

    RemoveList(tools);

    return (oldChoice);
}

void tcSellTool(void)
{
    LIST *bubble, *tools;
    NODE *node;
    U32 toolID, price;
    ubyte choice = 0;
    Tool tool;
    Person mary;

    mary = (Person) dbGetObject(Person_Mary_Bolton);

    hasAll(Person_Matt_Stuvysunt,
	   OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR, Object_Tool);
    tools = ObjectListPrivate;

    dbRemObjectNode(tools, Tool_Hand);
    dbRemObjectNode(tools, Tool_Fusz);

    if (LIST_EMPTY(tools))
	Say(BUSINESS_TXT, 0, MATT_PICTID, "MATT_HAS_NO_TOOL");

    while ((choice != GET_OUT) && (!LIST_EMPTY(tools))) {
	char exp[TXT_KEY_LENGTH];

	txtGetFirstLine(BUSINESS_TXT, "NO_CHOICE", exp);
	ExpandObjectList(tools, exp);

	SetPictID(MATT_PICTID);
	if (ChoiceOk(choice = Bubble(tools, 0, nullptr, 0L), GET_OUT, tools)) {
	    ubyte choice2 = 0;

	    node = (NODE *)GetNthNode(tools, (U32) choice);
	    toolID = OL_NR(node);

	    tool = (Tool) dbGetObject(toolID);
	    price = tcGetToolTraderOffer(tool);

	    bubble = txtGoKeyAndInsert(BUSINESS_TXT, "ANGEBOT_WERKZ", price);

	    SetPictID(mary->PictID);
	    (void)Bubble(bubble, 0, 0L, 0L); /* V1071: display-only, result intentionally discarded */
	    RemoveList(bubble);

	    bubble = txtGoKey(BUSINESS_TXT, "VERKAUF");

	    if (ChoiceOk
		(choice2 = Bubble(bubble, choice2, 0L, 0L), GET_OUT, bubble)) {
		if (choice2 == 0) {
		    tcAddPlayerMoney(price);
		    hasSet(Person_Mary_Bolton, toolID);
		    hasUnSet(Person_Matt_Stuvysunt, toolID);
		}
	    }

	    RemoveList(bubble);
	} else
	    choice = GET_OUT;

	RemoveList(tools);

	hasAll(Person_Matt_Stuvysunt,
	       OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR,
	       Object_Tool);
	tools = ObjectListPrivate;

	dbRemObjectNode(tools, Tool_Hand);
	dbRemObjectNode(tools, Tool_Fusz);
    }

    RemoveList(tools);
}

void tcToolsShop(void)
{
    ubyte choice = 0;
    static ubyte choice1 = 0;

    if (!(knows(Person_Matt_Stuvysunt, Person_Mary_Bolton)))
	knowsSet(Person_Matt_Stuvysunt, Person_Mary_Bolton);

    while (choice != 4) {
	choice = Say(BUSINESS_TXT, choice, MATT_PICTID, "Tools Shop");

	switch (choice) {
	case 0:
	    choice1 = tcBuyTool(choice1);
	    break;
	case 1:
	    tcSellTool();
	    break;
	case 2:
	    choice1 = tcShowTool(choice1);
	    break;
	case 3:
	    choice1 = tcDescTool(choice1);
	    break;
	default:
	    choice = 4;
	    break;
	}

	if (choice1 == GET_OUT)
	    choice1 = 0;
    }
}
