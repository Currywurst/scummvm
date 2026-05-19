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
 * Solution: tc_QuitGame() throws TheClou::QuitException.  The exception
 * unwinds through all C and C++ frames back to the try/catch in
 * theclou_run() (base/theclou_run.cpp), which then calls tcDone() for
 * orderly resource cleanup before returning to the ScummVM launcher.
 *
 * C callers (error.c, inphdl.c, tc_platform.cpp, …) see only the plain
 * extern "C" declaration below and call tc_QuitGame() like any other
 * function.  The throw happens inside the C++ implementation in
 * platform/tc_quit.cpp.
 *
 * Platform note: propagating a C++ exception through C stack frames is
 * well-defined on all ABI/platform combinations that ScummVM targets
 * (ELF/DWARF on Linux & Android, Mach-O/DWARF on macOS & iOS,
 * PE/SEH on Windows) — the C frames simply have no destructors to run and
 * the unwinder skips them cleanly.
 */

#ifdef __cplusplus
namespace TheClou {

/**
 * Thrown by tc_QuitGame() to terminate the game loop and return control
 * to the ScummVM launcher.  Caught exclusively in theclou_run().
 */
struct QuitException {};

} // namespace TheClou

extern "C" {
#endif /* __cplusplus */

/**
 * Request an immediate, clean exit from the game loop.
 *
 * Throws TheClou::QuitException which unwinds to the try/catch in
 * theclou_run() (base/theclou_run.cpp).  tcDone() is always called
 * after the catch, so all resources are freed before control returns
 * to the ScummVM launcher.
 *
 * Safe to call from any C or C++ game-code context (error handler,
 * input handler, quit menu, …).  Must NOT be called from background
 * threads — only the main game thread runs inside theclou_run().
 */
void tc_QuitGame(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* THECLOU_TC_QUIT_H */
