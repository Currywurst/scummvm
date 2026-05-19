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
 * Declared extern "C" so that every C source file in the engine can call
 * it without knowing about C++ exceptions.  The throw propagates through
 * all C and C++ frames on the call stack until it reaches the try/catch
 * in theclou_run() (base/theclou_run.cpp), where tcDone() is called for
 * orderly cleanup before returning to the ScummVM launcher.
 */

#include "theclou/platform/tc_quit.h"

extern "C" void tc_QuitGame(void) {
	throw TheClou::QuitException();
}
