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

#ifndef ENGINES_THECLOU_GAMEPLAY_GAMEFUNC_H
#define ENGINES_THECLOU_GAMEPLAY_GAMEFUNC_H

#include "theclou.h"
#include "list/list.h"

#define SCENE_NODE              UINT8_C(100)
#define EVENT_NODE              UINT8_C(101)
#define MAX_SCENES_AMOUNT	(1<<14) /* 14 + 18 = 32 Bits !!! */
#define MAX_EVENTS_AMOUNT	(1<<18) /* wichtig da story das benoetigt ! */


struct StoryHeader {
    ubyte StoryName[20];

    U32 EventCount;		/* Zaehler mit der hoechsten EventNr */
    U32 SceneCount;

    U32 AmountOfScenes;
    U32 AmountOfEvents;

    U32 StartZeit;
    U32 StartOrt;
    U32 StartSzene;
};

struct NewStory {
    ubyte StoryName[20];

    struct StoryHeader *sh;

    LIST *scenes;	/* Liste von SceneNodes */
    LIST *events;

    U32 StartZeit;
    U32 StartOrt;
};

struct NewScene {
    U32 EventNr;
    U8 SceneName[20];

    S32 Tag;			/* der Tag an dem sie eintritt */
    S32 MinZeitPunkt;		/* zeitlicher Bereich in dem    */
    S32 MaxZeitPunkt;		/* sie eintritt                         */
    U32 Ort;			/* der erfuellt sein muss                 */

    U32 AnzahlderEvents;
    U32 AnzahlderN_Events;

    U32 *events;		/* Nr. der Events, die erfuellt sein muessen */
    U32 *n_events;		/* Nr. der Events, die nicht erfuellt sein muessen */

    U32 AnzahlderNachfolger;
    U32 *nachfolger;		/* Nr. der NachfolgerEvents */

    U32 Moeglichkeiten;		/* siehe defines oben                   */
    U32 Dauer;			/* Dauer dieser Szene in Minuten     */
    U16 Anzahl;			/* wie oft sie geschehen kann                */
    U16 Geschehen;		/* wie oft sie SCHON geschehen ist */
    U8 Possibility;		/* mit der sie eintritt 0-255   */

    U32 Sample;			/* Nummer des Samples */
    U32 Anim;			/* Nummer der Animation */
    U32 NewOrt;			/* Ort der Scene */
};

struct SceneNode {
    NODE Link;
    struct NewScene ns;
};

struct NewEvent {
    U32 EventNr;
    ubyte EventName[20];
};

struct EventNode {
    NODE Link;
    struct NewEvent ne;
};


/* Prototypes */
/* Story */
extern void NewStory(ubyte * name, U32 StartZeit, U32 StartOrt);
extern void ChangeStory(U32 StartZeit, U32 StartOrt);
extern void RemoveStory(void);
extern void WriteStory(ubyte * filename);
extern void ReadStory(ubyte * filename);

/* Scenes */
extern void AddScene(struct NewScene *ns);
extern void ChangeScene(ubyte * name, struct NewScene *ns);
extern void RemoveScene(ubyte * name);
extern void RemoveAllScenes(void);

/* Events */
extern void AddEvent(struct NewEvent *ev);
extern void RemoveEvent(ubyte * name);
extern void RemoveAllEvents(void);

/* Specials */
char *GetName(U32 EventNr);


extern struct NewStory *story;

#endif // ENGINES_THECLOU_GAMEPLAY_GAMEFUNC_H
