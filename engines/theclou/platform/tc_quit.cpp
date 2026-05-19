/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
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

/*
 * tc_quit.cpp — Implementation of tc_QuitGame().
 *
 * Centralises the setjmp/longjmp quit mechanism that was previously
 * scattered inside base/base.c.  See platform/tc_quit.h for the full
 * design rationale.
 *
 * tc_QuitGame() is declared extern "C" so that every C source file in
 * the engine can call it via the declaration in tc_sdl_compat.h without
 * knowing about the longjmp internals.
 */

#include "theclou/platform/tc_quit.h"

/* Global state — defined here, declared extern in tc_quit.h. */
jmp_buf         g_tcQuitJmp;
volatile int    g_tcQuitJmpValid = 0;

extern "C" void tc_QuitGame(void) {
	if (g_tcQuitJmpValid) {
		g_tcQuitJmpValid = 0;
		longjmp(g_tcQuitJmp, 1);
	}
	/* Called before setjmp was set up — shouldn't happen in normal use. */
}
