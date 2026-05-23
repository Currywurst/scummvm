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

void DemoDialog(void)
{
    static int first_time = 1;
    ubyte choice = 0;
    uword briggs = 56, tony = 57, matt = 7;

    tcSetPlayerMoney(10000);

    if (first_time) {
	first_time = 0;

	SetBubbleType(THINK_BUBBLE);
	Say(DEMO_TXT, 0, matt, "Matt_Start");
	Say(DEMO_TXT, 0, briggs, "Briggs_1");
	Say(DEMO_TXT, 0, matt, "Matt_Hi");
	Say(DEMO_TXT, 0, tony, "Tony_Hi");
	Say(DEMO_TXT, 0, briggs, "Briggs_1_2");
	Say(DEMO_TXT, 0, tony, "Tony_1");

	while (choice != 4) {
	    choice = Say(DEMO_TXT, 0, matt, "Matt_1");

	    switch (choice) {
	    case 0:
		Say(DEMO_TXT, 0, tony, "Tony_Juw");
		break;
	    case 1:
		Say(DEMO_TXT, 0, tony, "Tony_Aunt");
		break;
	    case 2:
		Say(DEMO_TXT, 0, tony, "Tony_Ken");
		break;
	    case 3:
		Say(DEMO_TXT, 0, tony, "Tony_Bank");
		break;
	    default:
		break;
	    }
	}

	Say(DEMO_TXT, 0, briggs, "Briggs_2");
	Say(DEMO_TXT, 0, tony, "Tony_2");
	Say(DEMO_TXT, 0, briggs, "Briggs_3");
	Say(DEMO_TXT, 0, tony, "Tony_3");
	Say(DEMO_TXT, 0, briggs, "Briggs_4");
	Say(DEMO_TXT, 0, tony, "Tony_Bye");
	Say(DEMO_TXT, 0, matt, "Matt_Bye");

	knowsSet(Person_Matt_Stuvysunt, Person_Herbert_Briggs);
	knowsSet(Person_Matt_Stuvysunt, Person_Tony_Allen);

	joined_bySet(Person_Matt_Stuvysunt, Person_Herbert_Briggs);
	joined_bySet(Person_Matt_Stuvysunt, Person_Tony_Allen);

	joinSet(Person_Matt_Stuvysunt, Person_Herbert_Briggs);
	joinSet(Person_Matt_Stuvysunt, Person_Tony_Allen);
    }
}
