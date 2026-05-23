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

#ifndef ENGINES_THECLOU_PLATFORM_TCDEBUG_H
#define ENGINES_THECLOU_PLATFORM_TCDEBUG_H

/**
 * @file TcDebug.h
 * @brief TheClou debug/warning helpers — C++ native, no extern "C" needed.
 *
 * Since all engine translation units are now .cpp, there is no reason to
 * keep debug output behind extern "C" C-linkage wrappers.  This header
 * provides two lightweight inline functions directly in the TheClou namespace
 * that forward to ScummVM's debug() / warning() without any indirection.
 *
 * The legacy tc_debug() / tc_warning() names are preserved as #define
 * aliases so existing call-sites need no changes.
 *
 *   TheClou::tcDebug(level, fmt, ...)   — maps to ScummVM debug(level, ...)
 *   TheClou::tcWarning(fmt, ...)        — maps to ScummVM warning(...)
 *
 * Convenience aliases (backward-compatible):
 *   tc_debug(level, fmt, ...)           — alias for TheClou::tcDebug
 *   tc_warning(fmt, ...)                — alias for TheClou::tcWarning
 */

#include "common/scummsys.h"
#include "common/debug.h"
#include "common/str.h"
#include "common/textconsole.h"

#include <stdarg.h>

namespace TheClou {

/**
 * Emit a debug message at the given verbosity level.
 * Delegates to ScummVM's global debug(level, ...) function.
 *
 * @param level  Verbosity level (0 = always shown when debug output enabled).
 * @param format printf-style format string.
 */
inline void tcDebug(int level, const char *format, ...) GCC_PRINTF(2, 3);

inline void tcDebug(int level, const char *format, ...) {
	va_list args;
	va_start(args, format);
	Common::String msg = Common::String::vformat(format, args);
	va_end(args);
	debug(level, "%s", msg.c_str());
}

/**
 * Emit a warning message (always visible regardless of debug level).
 * Delegates to ScummVM's global warning() function.
 *
 * @param format printf-style format string.
 */
inline void tcWarning(const char *format, ...) GCC_PRINTF(1, 2);

inline void tcWarning(const char *format, ...) {
	va_list args;
	va_start(args, format);
	Common::String msg = Common::String::vformat(format, args);
	va_end(args);
	warning("%s", msg.c_str());
}

} // End of namespace TheClou

// ---------------------------------------------------------------------------
// Backward-compatibility shims
//
// All existing call-sites use tc_debug() / tc_warning() — keep those names
// working without modifying a single call-site.
// ---------------------------------------------------------------------------
#define tc_debug   TheClou::tcDebug
#define tc_warning TheClou::tcWarning

#endif // ENGINES_THECLOU_PLATFORM_TCDEBUG_H
