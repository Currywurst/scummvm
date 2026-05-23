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

#ifndef ENGINES_THECLOU_PLATFORM_TC_QUIT_H
#define ENGINES_THECLOU_PLATFORM_TC_QUIT_H

/*
 * tc_quit.h — Clean-quit mechanism for the TheClou engine.
 *
 * The original Der Clou! code calls exit() in many places (error handler,
 * input handler, quit menu).  Inside ScummVM an engine must never call
 * exit(); it must return from Engine::run() so the launcher can continue.
 *
 * Mechanism (flag-based, no setjmp/longjmp):
 *
 *   1. Any game-code site that needs to quit calls tc_QuitGame().
 *   2. tc_QuitGame() sets g_tcShouldQuit = true and calls g_engine->quitGame()
 *      so ScummVM's own event loop also sees the quit request.
 *   3. All long-running game loops call tc_ShouldQuit() and break/return
 *      when it returns true.  The key chokepoint is inpWaitFor() in
 *      inphdl/inphdl.cpp — every interactive wait passes through it.
 *   4. theclou_run() (base/theclou_run.cpp) calls tcDone() for orderly
 *      cleanup after tcDo() returns.
 *
 * tc_QuitGame() is safe to call from any C or C++ translation unit.
 * It must NOT be called from background threads.
 */

/**
 * Set by tc_QuitGame(); read by tc_ShouldQuit().
 * Declared volatile so the compiler does not cache the value across
 * loop iterations in game-loop hot paths.
 */
extern volatile int g_tcShouldQuit;

/**
 * Request an immediate, clean exit from the game loop.
 * Sets g_tcShouldQuit and signals ScummVM's engine via g_engine->quitGame().
 * Must NOT be called from background threads.
 *
 * Declared extern "C" so callers across translation units get a stable,
 * unmangled symbol regardless of include order.
 */
extern "C" void tc_QuitGame();

/**
 * Returns non-zero if the engine should stop.
 * Checks both g_tcShouldQuit (set by tc_QuitGame) and ScummVM's own
 * shouldQuit flag (set when the user closes the window or presses
 * Return-to-Launcher in the ScummVM GUI).
 * Use this in all long-running game loops.
 */
extern "C" int tc_ShouldQuit();

#endif /* ENGINES_THECLOU_PLATFORM_TC_QUIT_H */
