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

#ifndef ENGINES_THECLOU_SCENES_EVIDENCE_H
#define ENGINES_THECLOU_SCENES_EVIDENCE_H

#include "theclou.h"

#include "list/list.h"
#include "text/text.h"
#include "dialog/dialog.h"
#include "present/interac.h"
#include "present/present.h"

#include "data/objstd/tcdata.h"

#include "data/relation.h"
#include "data/database.h"
#include "data/datacalc.h"
#include "data/dataappl.h"

#define FAHN_NOT_ESCAPED      1
#define FAHN_ESCAPED          2

#define FAHN_ALARM            (1)
#define FAHN_ALARM_LOUDN      (1<< 1)
#define FAHN_ALARM_RADIO      (1<< 2)
#define FAHN_ALARM_PATRO      (1<< 3)
#define FAHN_SURROUNDED       (1<< 4)
#define FAHN_ESCAPE           (1<< 5)
#define FAHN_QUIET_ALARM      (1<< 6)
#define FAHN_STD_ESCAPE       (1<< 7) /* wenn Einbruch glatt geht (Standardflucht) */
#define FAHN_ALARM_ALARM      (1<< 8)
#define FAHN_ALARM_TIMECLOCK  (1<< 9)
#define FAHN_ALARM_POWER      (1<<10)
#define FAHN_ALARM_TIMER      (1<<11)
#define FAHN_ALARM_MICRO      (1<<12)
#define FAHN_ALARM_GUARD      (1<<13)

struct Search {			/* wegen Arrays nicht in objects */
    word GuyXPos[4], GuyYPos[4];	/* letzte Position im Falle einer Flucht! */

    ubyte Exhaust[4];

    uword WalkTime[4];
    uword WaitTime[4];
    uword WorkTime[4];
    uword KillTime[4];

    U32 DeriTime;		/* Abweichung vom Soll */

    U32 TimeOfBurglary;		/* Zeitpunkt! */
    U32 TimeOfAlarm;		/* Zeitpunkt! */

    U32 BuildingId;		/* Einbruch */
    U32 LastAreaId;		/* zum Zeitpunkt der Flucht */

    U32 EscapeBits;

    word CallValue;		/* "Wert" der Funkspr., word!!! */
    uword CallCount;		/* Anzahl d. FUnkspr. */

    uword WarningCount;		/* Anzahl der Warnungen */
    ubyte SpotTouchCount[4];

    ubyte KaserneOk;		/* ob die Kaserne geschafft wurde! */
};

extern S32 tcCalcCarEscape(S32 timeLeft);
extern void tcForgetGuys(void);

extern struct Search Search;

#endif  // ENGINES_THECLOU_SCENES_EVIDENCE_H
