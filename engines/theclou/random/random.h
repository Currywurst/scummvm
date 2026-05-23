/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * Original game code (c) 1994 H. Gaberschek
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

/**
 * @file random/random.h
 * @brief Random number generation for Der Clou!
 *
 * Thin wrapper: rndInit() seeds ScummVM's Common::RandomSource,
 * CalcRandomNr() returns a value in [l_limit, u_limit).
 *
 * Does NOT include common/scummsys.h so legacy game files can safely
 * include this without triggering common/forbidden.h bans.
 */

#ifndef ENGINES_THECLOU_RANDOM_RANDOM_H
#define ENGINES_THECLOU_RANDOM_RANDOM_H

#include "theclou.h"

/** Initialise the random-number generator (call once at engine start). */
void rndInit(void);

/** Clean up the random-number generator (currently a no-op). */
void rndDone(void);

/**
 * Return a uniformly distributed random integer in the half-open
 * interval [l_limit, u_limit).
 *
 * @param l_limit  Lower bound (inclusive).
 * @param u_limit  Upper bound (exclusive).  Must be > l_limit.
 */
U32 CalcRandomNr(U32 l_limit, U32 u_limit);

#endif // ENGINES_THECLOU_RANDOM_RANDOM_H
