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

#ifndef ENGINES_THECLOU_LANDSCAP_RASTER_H
#define ENGINES_THECLOU_LANDSCAP_RASTER_H

#include "theclou.h"
#include "dialog/dialog.h"
#include "landscap/landscap.h"

#define LS_RASTER_DISP_WIDTH       320
#define LS_RASTER_DISP_HEIGHT      140

#define LS_RASTER_X_SIZE           4	/* Raster is used for collision */
#define LS_RASTER_Y_SIZE           4	/* dedection, in pixel */


NODE *lsGetSuccObject(NODE * start);
NODE *lsGetPredObject(NODE * start);
void lsFadeRasterObject(U32 areaID, LSObject lso, ubyte status);
void lsShowAllConnections(U32 areaID, NODE * node, ubyte perc);
void lsShowRaster(U32 areaID, ubyte perc);

uword lsGetRasterXSize(U32 areaID);
uword lsGetRasterYSize(U32 areaID);

#endif  // ENGINES_THECLOU_LANDSCAP_RASTER_H
