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

#include "dialog/dialog.h"
#include "dialog/talkappl.h"
#include "platform/tc_debug.h"

#define DLG_NO_SPEECH	((U32) -1)
U32 StartFrame = DLG_NO_SPEECH;
U32 EndFrame = DLG_NO_SPEECH;
static char SpeechClipKey[TXT_KEY_LENGTH];

struct DynDlgNode {
    NODE Link;

    ubyte KnownBefore;		/* wie gut Sie bekannt sein müssen */
    ubyte KnownAfter;		/* wie gut Sie danach bekannt sind ! */
};


static LIST *PrepareQuestions(LIST * keyWords, U32 talkBits, ubyte textID)
{
    LIST *questionList = 0L, *preparedList = CreateList();
    LIST *stdQuestionList = txtGoKey(BUSINESS_TXT, "STD_QUEST");
    NODE *n;
    char question[TXT_KEY_LENGTH];
    ubyte r, i;

    questionList = txtGoKey((U32) textID, "QUESTIONS");

    for (n = (NODE *) LIST_HEAD(keyWords); NODE_SUCC((NODE *) n);
	 n = (NODE *) NODE_SUCC(n)) {
	r = (ubyte) CalcRandomNr(0L, 6L);

	snprintf(question, sizeof(question), NODE_NAME(GetNthNode(questionList, r)),
		NODE_NAME((NODE *) n));

	CreateNode(preparedList, 0L, question);
    }

    for (i = 0; i < 32; i++) {
	if (talkBits & (1 << i)) {
	    strcpy(question, NODE_NAME(GetNthNode(stdQuestionList, i)));
	    CreateNode(preparedList, 0L, question);
	}
    }

    txtGetFirstLine(BUSINESS_TXT, "Bye_says_Matt", question);
    CreateNode(preparedList, 0L, question);

    RemoveList(stdQuestionList);
    RemoveList(questionList);

    return (preparedList);
}

static LIST *ParseTalkText(LIST * origin, LIST * bubble, ubyte known)
{
    LIST *keyWords;
    NODE *n, *keyNode;
    char line[TXT_KEY_LENGTH], key[TXT_KEY_LENGTH], keyWord[TXT_KEY_LENGTH];
    char *mem, *start;
    ubyte line_pos = 0, key_pos;
    char snr[10], snr1[10];
    ubyte nr, nr1;
    U32 i;
	size_t keyLen;

    keyWords = CreateList();

    for (n = (NODE *) LIST_HEAD(origin); NODE_SUCC(n);
	 n = (NODE *) NODE_SUCC(n)) {
	line_pos = key_pos = 0;

	start = NODE_NAME(n);
	mem = start;

	while (mem < start + strlen(start)) {
	    if (*mem != '[')
		line[line_pos++] = *(mem++);
	    else {
		mem++;		/* Klammer überspringen ! */
		key_pos = 0;

		while ((*mem) != ']')
		    key[key_pos++] = *(mem++);

		key[key_pos++] = EOS;

		mem++;		/* Klammer überspringen ! */

			keyLen = strlen(key);

			if (keyLen < 6)
			    continue;

			for (i = 0; i < 3; i++) {	/* Zahlen entfernen */
			    snr[i] = key[i];
			    snr1[i] = key[keyLen - 3 + i];
			}

			{
			    size_t wordLen = keyLen - 6;
			    for (i = 0; i < wordLen; i++)
				keyWord[i] = key[i + 3];
			    keyWord[wordLen] = EOS;
			}

			/* umwandeln ! */
			snr[3] = EOS;
			snr1[3] = EOS;

		nr = (ubyte) atol(snr);
		nr1 = (ubyte) atol(snr1);

		/* keyword einfügen */
		for (i = 0; i < strlen(keyWord); i++)
		    line[line_pos++] = keyWord[i];

		if (known >= nr) {
		    keyNode =
			(NODE *)CreateNode(keyWords, sizeof(struct DynDlgNode),
				   keyWord);

		    ((struct DynDlgNode *) keyNode)->KnownBefore = nr;
		    ((struct DynDlgNode *) keyNode)->KnownAfter = nr1;
		}
	    }
	}

	line[line_pos++] = EOS;

	(void) CreateNode(bubble, 0L, line);
    }

    return (keyWords);
}

void DynamicTalk(U32 Person1ID, U32 Person2ID, ubyte TalkMode)
{
    const char *Extension[4] = { "_UNKNOWN", "_KNOWN", "_FRIENDLY", "_BUSINESS" };
    const char *Standard = "STANDARD";
    U8 known = 0;
    Person p1 = (Person) dbGetObject(Person1ID);
    Person p2 = (Person) dbGetObject(Person2ID);
    char key[TXT_KEY_LENGTH], name[TXT_KEY_LENGTH];
    U8 choice = 0, max = 1, i, quit, stdcount = 0, j, gencount = 0, textID;
    LIST *origin = 0L, *questions = 0L, *bubble = CreateList(), *keyWords;
    struct DynDlgNode *n;

    tcChgPersPopularity(p1, 5);	/* Bekanntheit steigt sehr gering */

    /* je nach Bekanntheitsgrad wird Matt begrüßt ! */
    dbGetObjectName(Person2ID, name);
    strcpy(key, name);

    if (TalkMode & DLG_TALKMODE_BUSINESS) {
	knowsSet(Person1ID, Person2ID);
	known = 3;		/* Business */
    } else {
	if (!(knows(Person1ID, Person2ID))) {
	    known = 0;
	    knowsSet(Person1ID, Person2ID);
	} else
	    known = 1;		/* MOD - kein "FRIENDLY mehr möglich!" */
    }

    strcat(key, Extension[known]);

    if (p2->TalkFileID)
	textID = TALK_1_TXT;
    else
	textID = TALK_0_TXT;

    if (!(txtKeyExists(textID, key))) {
	strcpy(key, Standard);

	strcat(key, Extension[known]);
    }

    do {
	origin = txtGoKey(textID, key);
	keyWords = ParseTalkText(origin, bubble, p2->Known);
	questions = PrepareQuestions(keyWords, p2->TalkBits, textID);

	if (choice < (max - stdcount)) {
	    SetPictID(p2->PictID);
	    (void)Bubble(bubble, 0, 0L, 0L); /* V1071: display-only, result intentionally discarded */
	}

	SetPictID(MATT_PICTID);
	choice = Bubble(questions, 0, 0L, 0L);

	quit = max = GetNrOfNodes(questions) - 1;

	for (i = 0, stdcount = 0; i < 32; i++)	/* Std Fragen zählen */
	    if (p2->TalkBits & (1 << i))
		stdcount++;

	gencount = max - stdcount;

	if (choice < gencount) {
	    n = (struct DynDlgNode *) GetNthNode(keyWords, (U32) choice);

	    strcpy(key, name);
	    strcat(key, "_");
	    strcat(key, NODE_NAME((NODE *) n));

	    if (n->KnownAfter > p2->Known)
		p2->Known = n->KnownAfter;
	}

	if (choice >= gencount && choice < quit) {
	    for (i = 0; i < 32; i++)	/* beim 1. gesetzten Bit anfangen ! */
		if (1 << i & p2->TalkBits)
		    break;

	    for (j = 0; (i < 32) && (j != (choice - gencount)); i++)
		if (1 << i & p2->TalkBits)
		    j++;

	    switch (i) {
	    case 0:
		tcJobOffer(p2);
		tcChgPersPopularity(p1, 10);
		break;
	    case 1:
		tcMyJobAnswer(p2);
		break;
	    case 2:
		tcPrisonAnswer(p2);
		break;
	    case 3:
		tcAbilityAnswer(Person2ID);
		break;
	    default:
		break;
	    }
	}

	RemoveList(keyWords);
	RemoveList(origin);
	RemoveList(questions);
	RemoveNode(bubble, nullptr);
    }
    while (choice != quit);

    RemoveList(bubble);
}

void PlayFromCDROM(void)
{
	if (!setup.CDAudio) {
		return;
	}

	if (setup.CDAudioFromCD &&
		(StartFrame != DLG_NO_SPEECH) && (EndFrame != DLG_NO_SPEECH)) {
		sndFading(16);
		CDROM_PlayAudioSequence(2, StartFrame, EndFrame);
		return;
	}

	if (setup.CDAudioFromWav && SpeechClipKey[0] != '\0') {
		sndFading(16);
		tc_debug(1, "TheClou: speech: playing clip '%s'", SpeechClipKey);
		if (!sndPlaySpeechSample(SpeechClipKey)) {
			DebugMsg(ERR_WARNING, ERROR_MODULE_SOUND,
					 "Missing speech sample: %s", SpeechClipKey);
			tc_warning("TheClou: speech: WAV not found for clip '%s'", SpeechClipKey);
		}
	} else if (setup.CDAudioFromWav) {
		/* SpeechClipKey is empty — Say() didn't set it (key not in CDROM_TXT?) */
		tc_debug(2, "TheClou: PlayFromCDROM: CDAudioFromWav=1 but SpeechClipKey empty");
	}
}

ubyte Say(U32 TextID, ubyte activ, uword Person, const char *text)
{
    LIST *bubble;
    ubyte choice;

    tc_debug(2, "TheClou: Say('%s') CDRom=%d CDAudio=%d CDAudioFromWav=%d",
             text ? text : "(null)", setup.CDRom, setup.CDAudio, setup.CDAudioFromWav);

    if (setup.CDRom) {
        bubble = txtGoKey(TextID, text);

        if (Person != (uword) - 1)
            SetPictID(Person);

        /* speech output must be started out of the bubble because
           after starting it there must be no access to the CDROM
           (neither pictures or text, nor any other directory) or
           speech would be interrupted */

		if (txtKeyExists(CDROM_TXT, text)) {
            tc_debug(2, "TheClou: Say('%s') CDROM_TXT key found — speech armed", text);
            char keys[TXT_KEY_LENGTH];

            txtGetFirstLine(CDROM_TXT, text, keys);

			strncpy(SpeechClipKey, text, sizeof(SpeechClipKey) - 1);
			SpeechClipKey[sizeof(SpeechClipKey) - 1] = '\0';

            StartFrame =
                (txtGetKeyAsULONG(1, keys) * 60L +
                 txtGetKeyAsULONG(2, keys)) * 75L + txtGetKeyAsULONG(3, keys);
            EndFrame =
                (txtGetKeyAsULONG(4, keys) * 60L +
                 txtGetKeyAsULONG(5, keys)) * 75L + txtGetKeyAsULONG(6, keys);

            choice = Bubble(bubble, activ, nullptr, 0L);
        } else {
			StartFrame = DLG_NO_SPEECH;
			EndFrame = DLG_NO_SPEECH;
			SpeechClipKey[0] = '\0';

            choice = Bubble(bubble, activ, nullptr, 0L);
        }

        if (setup.CDAudio) {
			if (setup.CDAudioFromCD) {
				CDROM_StopAudioTrack();
			}
			if (setup.CDAudioFromWav) {
				sndStopSpeechSample();
			}
            sndFading(0);
        }

        StartFrame = DLG_NO_SPEECH;
        EndFrame = DLG_NO_SPEECH;
		SpeechClipKey[0] = '\0';

        RemoveList(bubble);
    } else {
        bubble = txtGoKey(TextID, text);

        if (Person != (uword) - 1)
	    SetPictID(Person);

        /* When WAV speech is available, arm SpeechClipKey so that
         * PlayFromCDROM() (called from inside Bubble()) can play it. */
        if (setup.CDAudio && setup.CDAudioFromWav && txtKeyExists(CDROM_TXT, text)) {
            strncpy(SpeechClipKey, text, sizeof(SpeechClipKey) - 1);
            SpeechClipKey[sizeof(SpeechClipKey) - 1] = '\0';
            sndFading(16);
        }

        choice = Bubble(bubble, activ, nullptr, 0L);

        if (setup.CDAudio && setup.CDAudioFromWav) {
            sndStopSpeechSample();
            sndFading(0);
            SpeechClipKey[0] = '\0';
        }

        RemoveList(bubble);
    }

    return (choice);
}


U32 Talk(void)
{
    U32 succ_event_nr = 0L, locNr, personID;
    LIST *bubble;
    ubyte choice;
    char helloFriends[TXT_KEY_LENGTH];

    inpTurnESC(0);

    locNr = GetObjNrOfLocation(GetLocation);

    if (locNr) {
	hasAll(locNr, OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR,
	       Object_Person);
	bubble = ObjectListPrivate;

	if (!(LIST_EMPTY(bubble))) {
	    inpTurnESC(1);

	    txtGetFirstLine(BUSINESS_TXT, "NO_CHOICE", helloFriends);
	    ExpandObjectList(bubble, helloFriends);

	    if (ChoiceOk((choice = Bubble(bubble, 0, 0L, 0L)), GET_OUT, bubble)) {
		personID =
		    ((struct ObjectNode *) GetNthNode(bubble, (U32) choice))->
		    nr;

		inpTurnESC(0);

		if (PersonWorksHere(personID, locNr))
		    DynamicTalk(Person_Matt_Stuvysunt, personID,
				DLG_TALKMODE_BUSINESS);
		else
		    DynamicTalk(Person_Matt_Stuvysunt, personID,
				DLG_TALKMODE_STANDARD);
	    }
	} else
	    Say(BUSINESS_TXT, 0, MATT_PICTID, "NOBODY HERE");

	RemoveList(bubble);
    }

    inpTurnESC(1);

    return (succ_event_nr);
}
