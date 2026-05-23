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
 * tc_debug.cpp — C-callable bridges to ScummVM's debug() / warning().
 *
 * Implements the tc_debug / tc_warning functions declared in tc_debug.h.
 * Pure-C++ code can alternatively include TcDebug.h for the inline
 * TheClou::tcDebug / TheClou::tcWarning variants.
 */

#define FORBIDDEN_SYMBOL_EXCEPTION_printf

#include "common/scummsys.h"
#include "common/debug.h"
#include "common/str.h"
#include "common/textconsole.h"

#include "theclou/platform/tc_debug.h"

#include <stdarg.h>

extern "C" void tc_debug(int level, const char *format, ...) {
	va_list args;
	va_start(args, format);
	Common::String msg = Common::String::vformat(format, args);
	va_end(args);
	debug(level, "%s", msg.c_str());
}

extern "C" void tc_warning(const char *format, ...) {
	va_list args;
	va_start(args, format);
	Common::String msg = Common::String::vformat(format, args);
	va_end(args);
	warning("%s", msg.c_str());
}
