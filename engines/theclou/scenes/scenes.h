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

#ifndef ENGINES_THECLOU_SCENES_SCENES_H
#define ENGINES_THECLOU_SCENES_SCENES_H
#include "list/list.h"
#include "text/text.h"
#include "present/interac.h"
#include "present/present.h"
#include "dialog/dialog.h"
#include "data/relation.h"
#include "data/database.h"
#include "data/datacalc.h"
#include "data/dataappl.h"

#include "data/objstd/tcdata.h"
#include "gameplay/gp_app.h"
#include "landscap/raster.h"

extern uword CurrentBackground;

/* standard scene functions - look at scenes.c */

extern U32 Go(LIST * succ);
extern U32 tcTelefon(void);

extern void Look(U32 locNr);
extern void Information(void);
extern void tcWait(void);

/* taxi functions */

extern void AddTaxiLocation(U32 locNr);
extern void RemTaxiLocation(U32 locNr);

/* Done Funktionen */

void DoneHotelRoom(void);
void DoneTaxi(void);
void DoneGarage(void);
void DoneParking(void);
void DoneTools(void);
void DoneDealer(void);
void DoneInsideHouse(void);

void tcInitFahndung(void);
void tcDoneFahndung(void);

/* Hilfsfunktionen */

void Investigate(char *location);

void SetCarColors(ubyte index);

void tcSellCar(U32 ObjectID);
void tcColorCar(Car car);
void tcRepairCar(Car car, const char *repairWhat);
void tcCarGeneralOverhoul(Car car);
void tcToolsShop(void);

S32 tcEscapeFromBuilding(U32 escBits);
S32 tcEscapeByCar(U32 escBits, S32 timeLeft);

U32 tcChooseCar(U32 backgroundNr);

LIST *tcMakeLootList(U32 containerID, U32 relID);

char *tcShowPriceOfCar(U32 nr, U32 type, void *data);
char *tcShowPriceOfTool(U32 nr, U32 type, void *data);

void tcDealerSays(Person dealer, ubyte textNr, S32 perc);
void tcDealerOffer(Person dealer, ubyte which);

void tcBuyCar(void);
void tcCarInGarage(U32 carID);

ubyte tcBuyTool(ubyte choice);
void tcSellTool(void);
ubyte tcShowTool(ubyte choice);
ubyte tcDescTool(ubyte choice);
void tcDealerDlg(void);

U32 tcStartEvidence(void);
U32 tcGoInsideOfHouse(U32 buildingID);

void tcShowLootInfo(Loot loot);
void tcInsideOfHouse(U32 buildingID, U32 areaID, ubyte perc);
void tcShowObjectData(U32 areaID, NODE * node, ubyte perc);

#endif  // ENGINES_THECLOU_SCENES_SCENES_H
