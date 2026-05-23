/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * Original game code (c) 1993, 1994 ...and avoid panic by
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
 * @file memory/memory.cpp
 *
 * TCAllocMem / TCFreeMem delegate to malloc/calloc/free from <cstdlib>.
 * ScummVM's common/forbidden.h does not ban malloc/free, so no
 * FORBIDDEN_SYMBOL_EXCEPTION_* defines are required here.
 */

#include "common/scummsys.h"  // Must come first — pulls in forbidden.h

#include <cstdlib>   // malloc, calloc, free
#include <cstring>   // memset (fallback, not needed but included for clarity)

#include "memory/memory.h"

// ---------------------------------------------------------------------------
// Error code for memory allocation failure
// ---------------------------------------------------------------------------
#define ERR_MEMORY_NO_MEM  1

// ---------------------------------------------------------------------------
// Module-private state: net allocated byte count
// ---------------------------------------------------------------------------
namespace {

struct MemControl {
    ptrdiff_t allocated;
};

static MemControl g_memControl;

} // anonymous namespace

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void *TCAllocMem(size_t size, bool clear) {
    void *p = clear ? calloc(1, size) : malloc(size);

    if (!p) {
        ErrorMsg(No_Mem, ERROR_MODULE_MEMORY, ERR_MEMORY_NO_MEM);
        // ErrorMsg never returns — it calls tc_QuitGame().
        return nullptr;
    }

    g_memControl.allocated += static_cast<ptrdiff_t>(size);
    return p;
}

void TCFreeMem(void *ptr, size_t size) {
    if (ptr) {
        free(ptr);
        g_memControl.allocated -= static_cast<ptrdiff_t>(size);
    }
}

ptrdiff_t memGetAllocatedMem(void) {
    return g_memControl.allocated;
}
