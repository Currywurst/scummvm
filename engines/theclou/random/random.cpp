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
 * @file random/random.cpp
 *
 * Uses ScummVM's Common::RandomSource for portable random numbers instead of
 * the POSIX rand()/srand() pair.  Common::RandomSource is not banned by
 * common/forbidden.h so no FORBIDDEN_SYMBOL_EXCEPTION_* defines are needed.
 */

#include "common/scummsys.h"
#include "common/random.h"
#include "common/str.h"

#include "random/random.h"

// ---------------------------------------------------------------------------
// Module-private state
// ---------------------------------------------------------------------------
namespace {
Common::RandomSource *g_rnd = nullptr;
} // anonymous namespace

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void rndInit(void) {
    // "theclou" is the seed name — also used for deterministic replay/saves.
    g_rnd = new Common::RandomSource(Common::String("theclou"));
}

void rndDone(void) {
    delete g_rnd;
    g_rnd = nullptr;
}

U32 CalcRandomNr(U32 l_limit, U32 u_limit) {
    if (!g_rnd || u_limit <= l_limit)
        return l_limit;

    const uint32 range = static_cast<uint32>(u_limit - l_limit);
    return static_cast<U32>(l_limit + g_rnd->getRandomNumber(range - 1));
}
