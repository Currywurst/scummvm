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

#include "dialog/talkappl.h"

void tcJobOffer(Person p)
{
    LIST *bubble;
    ubyte choice;
    S32 part;
    Player player = (Player)dbGetObject(Player_Player_1);

    if (!(join(Person_Matt_Stuvysunt, dbGetObjectNr(p))))
	player->JobOfferCount++;

    choice = Say(BUSINESS_TXT, 0, MATT_PICTID, "PERS_ANZ");

    part = tcGetPersOffer(p, (choice + 2));

    bubble = txtGoKeyAndInsert(BUSINESS_TXT, "JOB_ANSWER", (U32) part, nullptr);

    SetPictID(p->PictID);
    (void)Bubble(bubble, 0, 0L, 0L); /* V1071: display-only, result intentionally discarded */
    RemoveList(bubble);

    choice = Say(BUSINESS_TXT, 0, MATT_PICTID, "NEW_THEEF");

    if (choice == 0) {
	joinSet(Person_Matt_Stuvysunt, dbGetObjectNr(p));
	hasSet(Person_Matt_Stuvysunt, dbGetObjectNr(p));	/* Matt hat jetzt Daten von ihm */
	p->TalkBits &= (0xffffffffL - (1 << Const_tcTALK_JOB_OFFER));	/* Joboffer löschen! */
    } else {
	p->Known = CalcValue(p->Known, 0, 255, 0, 20);
	Say(BUSINESS_TXT, 0, p->PictID, "MY_OFFER");
    }
}

void tcMyJobAnswer(Person p)
{
    LIST *bubble = CreateList();
    LIST *jobs = txtGoKey(OBJECTS_ENUM_TXT, "enum_JobE");
    char line[TXT_KEY_LENGTH], job[TXT_KEY_LENGTH], temp[TXT_KEY_LENGTH];

    strcpy(job, NODE_NAME(GetNthNode(jobs, p->Job)));

    if (strcmp(job, NODE_NAME(GetNthNode(jobs, 10))) != 0) {
	txtGetFirstLine(BUSINESS_TXT, "MY_JOB_IS", temp);
	snprintf(line, sizeof(line), temp, job);
    } else
	txtGetFirstLine(BUSINESS_TXT, "NO_JOB", line);

    SetPictID(p->PictID);
    CreateNode(bubble, 0L, line);
    (void)Bubble(bubble, 0, 0L, 0L); /* V1071: display-only, result intentionally discarded */

    RemoveList(jobs);
    RemoveList(bubble);
}

void tcPrisonAnswer(Person p)
{
    LIST *bubble = CreateList();
    LIST *source = txtGoKey(BUSINESS_TXT, "IN_PRISON_ANSWER");
    char line[TXT_KEY_LENGTH];

    strcpy(line, NODE_NAME(GetNthNode(source, p->KnownToPolice / 52)));

    SetPictID(p->PictID);
    CreateNode(bubble, 0L, line);
    (void)Bubble(bubble, 0, 0, 0); /* V1071: display-only, result intentionally discarded */

    RemoveList(source);
    RemoveList(bubble);
}

void tcAbilityAnswer(U32 personID)
{
    char name[TXT_KEY_LENGTH];
    LIST *bubble;
    Person p = (Person)dbGetObject(personID);

    dbGetObjectName(personID, name);

    bubble = txtGoKey(ABILITY_TXT, name);

    SetPictID(p->PictID);

    (void)Bubble(bubble, 0, 0, 0); /* V1071: display-only, result intentionally discarded */

    RemoveList(bubble);
}
