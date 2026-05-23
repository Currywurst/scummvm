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

#include "data/datacalc.h"

S32 Round(S32 v, S32 p)
{				/* p...Stellen ! */
    S32 i, z;

    for (i = 0, z = 1; i < p; i++, z *= 10);

    if (v / z != 0)
	v = (v / z) * z;

    return (v);
}

S32 CalcValue(S32 value, S32 min, S32 max, S32 fact, S32 perc)
{
    perc = ((perc * fact) - (perc * 128)) / 127;

    value = value + (value * perc) / 100;

    return clamp(value, min, max);
}

S32 ChangeAbs(S32 item, S32 value, S32 min, S32 max)
{
    item += value;

    return clamp(item, min, max);
}
