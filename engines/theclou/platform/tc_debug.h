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

#ifndef THECLOU_TC_DEBUG_H
#define THECLOU_TC_DEBUG_H

/*
 * C-callable wrappers around ScummVM's debug() / warning() functions.
 * C code in the engine must not call debug()/warning() directly because
 * those are C++-linkage functions.  Use these wrappers instead.
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

#endif /* THECLOU_TC_DEBUG_H */
