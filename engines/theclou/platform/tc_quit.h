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

#ifndef THECLOU_TC_QUIT_H
#define THECLOU_TC_QUIT_H

/*
 * tc_quit.h — Clean-quit mechanism for the TheClou engine.
 *
 * The original Der Clou! code calls exit() in many places (error handler,
 * input handler, quit menu).  Inside ScummVM an engine must never call
 * exit(); it must return from Engine::run() so the launcher can continue.
 *
 * ScummVM builds with -fno-exceptions, so C++ throw/catch is unavailable.
 * Instead we use setjmp/longjmp, isolated in this module so that the
 * mechanism is not scattered across base.c as it was originally.
 *
 * Usage:
 *   1. theclou_run() (base/theclou_run.cpp) calls setjmp(g_tcQuitJmp) and
 *      sets g_tcQuitJmpValid = 1 before entering the game loop.
 *   2. Any game-code site that needs to quit calls tc_QuitGame().
 *   3. tc_QuitGame() longjmps back to the setjmp site in theclou_run().
 *   4. theclou_run() calls tcDone() for orderly cleanup and returns.
 *
 * Safety:
 *   longjmp is safe here because there are no C++ objects with non-trivial
 *   destructors on the stack between the setjmp and the longjmp sites —
 *   the frames in between are all plain C functions (tcInit, tcDo, ...).
 *   tc_QuitGame() must NOT be called from background threads.
 */

#include <setjmp.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Jump buffer — set up by theclou_run() before the game loop starts. */
extern jmp_buf          g_tcQuitJmp;

/** Non-zero while the jump buffer above is valid (i.e. inside theclou_run). */
extern volatile int     g_tcQuitJmpValid;

/**
 * Request an immediate, clean exit from the game loop.
 *
 * longjmps back to theclou_run() (base/theclou_run.cpp).
 * tcDone() is always called after the longjmp so all resources are freed
 * before control returns to the ScummVM launcher.
 *
 * Safe to call from any C or C++ game-code context (error handler,
 * input handler, quit menu, …).  Must NOT be called from background threads.
 */
void tc_QuitGame(void);

#ifdef __cplusplus
}
#endif

#endif /* THECLOU_TC_QUIT_H */
