/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * Original game code (c) 1993, 1994 H. Gaberschek
 * Portions copyright (c) 2005 Vasco Alexandre da Silva Costa
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

/**
 * @file error/error.h
 * @brief Error and debug message subsystem for Der Clou!
 *
 * Deliberately does NOT include common/scummsys.h so that legacy game files
 * can include this header without triggering common/forbidden.h bans.
 * Only <stdarg.h> is needed for the variadic DebugMsg prototype.
 */

#ifndef ENGINES_THECLOU_ERROR_ERROR_H
#define ENGINES_THECLOU_ERROR_ERROR_H

#include <stdarg.h>

#include "theclou.h"
#include "disk/disk.h"

/* -------------------------------------------------------------------------
 * Error module identifiers
 * ---------------------------------------------------------------------- */
typedef enum {
    ERROR_MODULE_BASE       =  3,
    ERROR_MODULE_TXT        =  4,
    ERROR_MODULE_DISK       =  5,
    ERROR_MODULE_MEMORY     =  6,
    ERROR_MODULE_DATABASE   =  7,
    ERROR_MODULE_GAMEPLAY   =  8,
    ERROR_MODULE_LOADSAVE   =  9,
    ERROR_MODULE_LANDSCAP   = 10,
    ERROR_MODULE_LIVING     = 11,
    ERROR_MODULE_PLANING    = 12,
    ERROR_MODULE_SOUND      = 13,
    ERROR_MODULE_PRESENT    = 14,
    ERROR_MODULE_GFX        = 15,
    ERROR_MODULE_INPUT      = 16,
    ERROR_MODULE_LAST
} ErrorModuleE;

/* -------------------------------------------------------------------------
 * Error output mode constants (parameter for pcErrOpen)
 * ---------------------------------------------------------------------- */
#define ERR_STD_ERROR_FILENAME  "tc.err"
#define ERR_NO_OUTPUT           1
#define ERR_OUTPUT_TO_DISK      2

/* -------------------------------------------------------------------------
 * Error type enumeration
 * ---------------------------------------------------------------------- */
typedef enum {
    No_Error        = 0,
    Internal_Error,
    No_Mem,
    Disk_Defect,
    Insert_Disk,
    Lib_Error,
    Last_Error
} ErrorE;

/* -------------------------------------------------------------------------
 * Debug severity enumeration
 * ---------------------------------------------------------------------- */
typedef enum {
    ERR_ERROR   = 0,
    ERR_WARNING = 1,
    ERR_DEBUG   = 2
} DebugE;

/* -------------------------------------------------------------------------
 * Public API
 * ---------------------------------------------------------------------- */

/** Open the error manager; mode is ERR_NO_OUTPUT or ERR_OUTPUT_TO_DISK. */
bool  pcErrOpen(S32 l_Mode, const char *ErrorFilename);
void  pcErrClose(void);

/**
 * Report a fatal error: logs a warning, calls tcDone(), then tc_QuitGame().
 * Replaces the original exit()-based implementation.
 */
void  ErrorMsg(ErrorE type, ErrorModuleE moduleId, U32 errorId);

/**
 * Log a diagnostic message at the given severity for the given module.
 * ERR_DEBUG messages are suppressed unless setup.Debug is high enough.
 * ERR_ERROR also calls tc_QuitGame() after logging.
 */
void  DebugMsg(DebugE type, ErrorModuleE moduleId, const char *format, ...);

#endif // ENGINES_THECLOU_ERROR_ERROR_H
