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

#ifndef ENGINES_THECLOU_PLATFORM_TC_DEBUG_H
#define ENGINES_THECLOU_PLATFORM_TC_DEBUG_H

/*
 * tc_debug.h — C-callable wrappers around ScummVM's debug() / warning().
 *
 * Intentionally free of ScummVM includes so legacy game files that use
 * forbidden symbols (strcpy, strcat, …) don't pick up common/forbidden.h
 * through this header.  Pure-C++ files can also use TcDebug.h directly
 * for the inline TheClou::tcDebug / TheClou::tcWarning variants.
 *
 *   tc_debug(level, fmt, ...)   — maps to ScummVM debug(level, ...)
 *   tc_warning(fmt, ...)        — maps to ScummVM warning(...)
 */

#ifdef __cplusplus
extern "C" {
#endif

void tc_debug  (int level, const char *format, ...);
void tc_warning(const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif /* ENGINES_THECLOU_PLATFORM_TC_DEBUG_H */
