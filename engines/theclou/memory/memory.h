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
 * @file memory/memory.h
 * @brief Memory allocation wrappers for Der Clou!
 *
 * TCAllocMem / TCFreeMem are thin wrappers around malloc/free that track
 * total allocated bytes and call ErrorMsg(No_Mem) on allocation failure.
 *
 * Does NOT include common/scummsys.h so that legacy game files can safely
 * include this header without triggering common/forbidden.h bans.
 */

#ifndef ENGINES_THECLOU_MEMORY_MEMORY_H
#define ENGINES_THECLOU_MEMORY_MEMORY_H

#include <stddef.h>   // size_t, ptrdiff_t
#include <stdbool.h>

#include "theclou.h"
#include "error/error.h"

/**
 * Allocate @p size bytes.
 * If @p clear is true the block is zero-initialised (calloc semantics).
 * On failure ErrorMsg(No_Mem, …) is called (which never returns).
 * @return pointer to the allocated block, never nullptr.
 */
void *TCAllocMem(size_t size, bool clear);

/**
 * Free a block previously returned by TCAllocMem.
 * @p size must match the original allocation size (used for bookkeeping).
 */
void TCFreeMem(void *ptr, size_t size);

/** Return the net number of bytes currently allocated through TCAllocMem. */
ptrdiff_t memGetAllocatedMem(void);

#endif // ENGINES_THECLOU_MEMORY_MEMORY_H
