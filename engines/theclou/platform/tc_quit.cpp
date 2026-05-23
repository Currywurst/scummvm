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

#include "theclou/platform/tc_quit.h"
#include "engines/engine.h"   /* g_engine, Engine::shouldQuit() / quitGame() */

/* Global flag — defined here, declared extern in tc_quit.h. */
volatile int g_tcShouldQuit = 0;

extern "C" void tc_QuitGame() {
	g_tcShouldQuit = 1;
	if (g_engine)
		g_engine->quitGame();
}

extern "C" int tc_ShouldQuit() {
	if (g_tcShouldQuit)
		return 1;
	if (g_engine && g_engine->shouldQuit()) {
		/* Mirror ScummVM's flag into ours so callers only need tc_ShouldQuit(). */
		g_tcShouldQuit = 1;
		return 1;
	}
	return 0;
}
