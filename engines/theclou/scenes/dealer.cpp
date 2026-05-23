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

void tcDealerDlg(void)
{
    U32 locNr = GetObjNrOfLocation(GetLocation);
    Person dealer = nullptr;
    ubyte dealerNr, choice = 0;

    if (locNr == Location_Parker) {
        dealer = (Person)dbGetObject(Person_Helen_Parker);
        knowsSet(Person_Matt_Stuvysunt, Person_Helen_Parker);
        dealerNr = 2;
    } else if (locNr == Location_Maloya) {
        dealer = (Person)dbGetObject(Person_Frank_Maloya);
        knowsSet(Person_Matt_Stuvysunt, Person_Frank_Maloya);
        dealerNr = 0;
    } else if (locNr == Location_Pooly) {
        dealer = (Person)dbGetObject(Person_Eric_Pooly);
        knowsSet(Person_Matt_Stuvysunt, Person_Eric_Pooly);
        dealerNr = 1;
    } else {
        return;
    }

    while ((choice != 2) && (choice != GET_OUT)) {
	choice = Say(BUSINESS_TXT, 0, MATT_PICTID, "DEALER_QUEST");

	switch (choice) {
	case 0:		/* womit ? */
            if (locNr == Location_Parker) {
		Say(BUSINESS_TXT, 0, dealer->PictID, "DEALER_PARKER");
            } else if (locNr == Location_Maloya) {
		Say(BUSINESS_TXT, 0, dealer->PictID, "DEALER_MALOYA");
            } else if (locNr == Location_Pooly) {
		Say(BUSINESS_TXT, 0, dealer->PictID, "DEALER_POOLY");
            }
	    break;
	case 1:		/* offer */
	    hasAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Loot);

	    if (LIST_EMPTY(ObjectList)) {
		Say(BUSINESS_TXT, 0, dealer->PictID, "NO_LOOT");
		AddVTime(17);
	    } else
		tcDealerOffer(dealer, dealerNr);
	    break;
	case 2:
	default:
	    break;
	}
    }

    AddVTime(11);
    ShowTime(2);
}

void tcDealerOffer(Person dealer, ubyte which)
{
    ubyte Price[3][10] = { {70, 150, 220, 90, 210, 110, 200, 0, 190, 80},	/* maloya */
    {120, 200, 180, 220, 79, 110, 0, 0, 110, 200},	/* pooly */
    {220, 66, 0, 110, 0, 220, 0, 212, 20, 130}
    };				/* parker */
    CompleteLoot comp = (CompleteLoot)dbGetObject(CompleteLoot_LastLoot);

    RemoveList(tcMakeLootList(Person_Matt_Stuvysunt, Relation_has));

    if (comp->Bild)
	tcDealerSays(dealer, 0, (S32) Price[which][0]);
    if (comp->Gold)
	tcDealerSays(dealer, 1, (S32) Price[which][1]);
    if (comp->Geld)
	tcDealerSays(dealer, 2, (S32) Price[which][2]);
    if (comp->Juwelen)
	tcDealerSays(dealer, 3, (S32) Price[which][3]);
    if (comp->Delikates)
	tcDealerSays(dealer, 4, (S32) Price[which][4]);
    if (comp->Statue)
	tcDealerSays(dealer, 5, (S32) Price[which][5]);
    if (comp->Kuriositaet)
	tcDealerSays(dealer, 6, (S32) Price[which][6]);
    if (comp->HistKunst)
	tcDealerSays(dealer, 7, (S32) Price[which][7]);
    if (comp->GebrauchsArt)
	tcDealerSays(dealer, 8, (S32) Price[which][8]);
    if (comp->Vase)
	tcDealerSays(dealer, 9, (S32) Price[which][9]);
}

void tcDealerSays(Person dealer, ubyte textNr, S32 perc)
{
    LIST *lootNames = txtGoKey(OBJECTS_ENUM_TXT, "enum_LootE");
    LIST *specialLoot = txtGoKey(OBJECTS_ENUM_TXT, "enum_LootNameE");
    LIST *dealerText = txtGoKey(BUSINESS_TXT, "DEALER_OFFER");
    LIST *dealerOffer = CreateList();
    char line[TXT_KEY_LENGTH];
    ubyte symp, i;
    struct ObjectNode *n;
    Person others[3];
    Player player = (Player)dbGetObject(Player_Player_1);

    others[0] = (Person)dbGetObject(Person_Frank_Maloya);
    others[1] = (Person)dbGetObject(Person_Eric_Pooly);
    others[2] = (Person)dbGetObject(Person_Helen_Parker);

    if (perc == 0) {
	snprintf(line, sizeof(line), NODE_NAME(GetNthNode(dealerText, 4)),
		NODE_NAME(GetNthNode(lootNames, (U32) textNr)));
	CreateNode(dealerOffer, 0L, line);

	CreateNode(dealerOffer, 0L, NODE_NAME(GetNthNode(dealerText, 5)));

	SetPictID(dealer->PictID);
	(void)Bubble(dealerOffer, 0, 0L, 0L); /* V1071: display-only, result intentionally discarded */
    } else {
	hasAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Loot);
	perc = tcGetDealerPerc(dealer, perc);

	for (n = (struct ObjectNode *) LIST_HEAD(ObjectList); NODE_SUCC(n);
	     n = (struct ObjectNode *) NODE_SUCC(n)) {
	    Loot loot = (Loot)OL_DATA(n);
	    U32 price = hasGet(Person_Matt_Stuvysunt, OL_NR(n)), offer;

	    offer = tcGetDealerOffer(price, perc);
	    offer = max(offer, 1);

	    RemoveNode(dealerOffer, nullptr);

	    if ((loot->Type) == textNr) {
		if (loot->Name) {
		    symp = 10;

		    snprintf(line, sizeof(line), NODE_NAME(GetNthNode(dealerText, 2)),
			    NODE_NAME(GetNthNode
				      (specialLoot, (U32) loot->Name)));
		    CreateNode(dealerOffer, 0L, line);

		    snprintf(line, sizeof(line), NODE_NAME(GetNthNode(dealerText, 3)), offer);
		    CreateNode(dealerOffer, 0L, line);
		} else {
		    symp = 1;

		    snprintf(line, sizeof(line), NODE_NAME(GetNthNode(dealerText, 0)),
			    NODE_NAME(GetNthNode(lootNames, (U32) textNr)));
		    CreateNode(dealerOffer, 0L, line);

		    snprintf(line, sizeof(line), NODE_NAME(GetNthNode(dealerText, 1)), price,
			    offer);
		    CreateNode(dealerOffer, 0L, line);
		}

		SetPictID(dealer->PictID);
		(void)Bubble(dealerOffer, 0, 0L, 0L); /* V1071: display-only, result intentionally discarded */

		if (!(Say(BUSINESS_TXT, 0, MATT_PICTID, "DEALER_ANSWER"))) {
		    S32 mattsMoney;

		    hasUnSet(Person_Matt_Stuvysunt, OL_NR(n));

		    mattsMoney = max(((offer * (player->MattsPart)) / 100), 1);

		    tcAddDealerSymp(dealer, symp);
		    tcAddPlayerMoney(mattsMoney);

		    player->StolenMoney += offer;
		    player->MyStolenMoney += mattsMoney;

		    for (i = 0; i < 3; i++)
			if (dealer != others[i])
			    tcAddDealerSymp((others[i]), (symp * (-1)));
		}
	    }
	}
    }

    RemoveList(specialLoot);
    RemoveList(dealerOffer);
    RemoveList(dealerText);
    RemoveList(lootNames);
}

LIST *tcMakeLootList(U32 containerID, U32 relID)
{
    NODE *n;
    Loot loot;
    CompleteLoot comp = (CompleteLoot)dbGetObject(CompleteLoot_LastLoot);
    char data[TXT_KEY_LENGTH];
    U32 value;
    LIST *out = CreateList();
    LIST *loots;
    LIST *lootE = txtGoKey(OBJECTS_ENUM_TXT, "enum_LootE");
    LIST *lootNameE = txtGoKey(OBJECTS_ENUM_TXT, "enum_LootNameE");

    /* Listen initialisieren */

    SetObjectListAttr(OLF_PRIVATE_LIST, Object_Loot);
    AskAll(dbGetObject(containerID), relID, BuildObjectList);
    loots = ObjectListPrivate;

    comp->Bild = comp->Gold = comp->Geld = comp->Juwelen = 0;
    comp->Delikates = comp->Statue = comp->Kuriositaet = 0;
    comp->HistKunst = comp->GebrauchsArt = comp->Vase = 0;

    comp->TotalWeight = comp->TotalVolume = 0;

    /* Liste durcharbeiten */

    if (!(LIST_EMPTY(loots))) {
	for (n = (NODE *) LIST_HEAD(loots); NODE_SUCC(n);
	     n = (NODE *) NODE_SUCC(n)) {
	    if (OL_TYPE(n) == Object_Loot) {
		loot = (Loot)OL_DATA(n);

		value = GetP(dbGetObject(containerID), relID, loot);

		switch (loot->Type) {
		case Ein_Bild:
		    comp->Bild += value;
		    break;
		case Gold:
		    comp->Gold += value;
		    break;
		case Geld:
		    comp->Geld += value;
		    break;
		case Juwelen:
		    comp->Juwelen += value;
		    break;
		case Delikatessen:
		    comp->Delikates += value;
		    break;
		case Eine_Statue:
		    comp->Statue += value;
		    break;
		case Eine_Kuriositaet:
		    comp->Kuriositaet += value;
		    break;
		case Eine_Vase:
		    comp->Vase += value;
		    break;
		case Ein_historisches_Kunstobjekt:
		    comp->HistKunst += value;
		    break;
		case Gebrauchsartikel:
		    comp->GebrauchsArt += value;
		    break;
		default:
		    break;
		}

		comp->TotalWeight += loot->Weight;
		comp->TotalVolume += loot->Volume;

		if (loot->Name)
		    strcpy(data, NODE_NAME(GetNthNode(lootNameE, loot->Name)));
		else
		    strcpy(data, NODE_NAME(GetNthNode(lootE, loot->Type)));

		CreateNode(out, 0L, data);

		snprintf(data, sizeof(data), "%" PRIu32, value);
		CreateNode(out, 0L, data);

		snprintf(data, sizeof(data), "%" PRIu32, loot->Volume);
		CreateNode(out, 0L, data);

		snprintf(data, sizeof(data), "%" PRIu16, loot->Weight);
		CreateNode(out, 0L, data);
	    }
	}
    }

    RemoveList(lootE);
    RemoveList(lootNameE);
    RemoveList(loots);

    return (out);
}
