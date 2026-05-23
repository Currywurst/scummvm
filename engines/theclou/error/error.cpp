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

/* Allow vsnprintf — used via DebugMsg's internal buffer. */
#define FORBIDDEN_SYMBOL_EXCEPTION_vsnprintf
#define FORBIDDEN_SYMBOL_EXCEPTION_snprintf
/* common/str.h provides Common::strlcpy which we use in pcErrOpen. */

#include "common/scummsys.h"
#include "common/str.h"

#include "base/base.h"
#include "platform/tc_debug.h"
#include "platform/tc_fs.h"

/* tc_QuitGame() throws TheClou::QuitException which unwinds to
 * theclou_run() (base/theclou_run.cpp). Using it instead of exit()
 * lets ScummVM shut down cleanly (Return-to-Launcher). */
extern void tc_QuitGame(void);

#include "error/error.h"

static const char *moduleNames[ERROR_MODULE_LAST] = {
  "",
  "",
  "",
  "Base",
  "Txt",
  "Dsk",
  "Mem",
  "Data",
  "GP",
  "L/S",
  "Land",
  "Liv",
  "Plan",
  "Snd",
  "Pres",
  "Gfx",
  "Input"
};

struct ErrorHandler {
    char Filename[DSK_PATH_MAX];
    bool uch_OutputToFile;
};

/* implementation */

struct ErrorHandler ErrorHandler;

bool pcErrOpen(S32 l_Mode, const char *ErrorFilename)
{
    TC_FILE *p_File;
    bool alright = false;

    switch (l_Mode) {
    case ERR_NO_OUTPUT:
        ErrorHandler.uch_OutputToFile = false;
        break;
    case ERR_OUTPUT_TO_DISK:
        ErrorHandler.uch_OutputToFile = true;

        /* lets have a look if we can open the file */
        /* and then lets clean it : */

        if ((p_File = dskOpen(ErrorFilename, "w"))) {
            alright = true;
            dskClose(p_File);
        }

        Common::strlcpy(ErrorHandler.Filename, ErrorFilename, DSK_PATH_MAX);
        break;
    default:
        break;
    }

    return alright;
}

void ErrorMsg(ErrorE type, ErrorModuleE moduleId, U32 errorId)
{
    /* Always log module + errorId as a warning so diagnosis works without -d */
    tc_warning("TheClou: ErrorMsg module=%s id=%u",
               moduleNames[moduleId], (unsigned)errorId);

    tcDone();

    switch (type) {
    case Internal_Error:
	tc_warning("TheClou: Internal Error!");
	break;

    case No_Mem:
	tc_warning("TheClou: Not enough memory!");
	break;

    case Disk_Defect:
	tc_warning("TheClou: Can't open file! Please reinstall DER CLOU!");
	break;
    default:
	break;
    }
    /* Replace exit() with tc_QuitGame() so ScummVM can return to the
     * launcher cleanly instead of killing the whole process.          */
    tc_QuitGame();
}

static void ErrDebugMsg(DebugE type, const char *moduleName, const char *txt)
{
    if (setup.Debug < type) {
        return;
    }

    /* File logging omitted: ScummVM handles log output centrally. */

    switch (type) {
    case ERR_DEBUG:
        tc_debug(2, "%s\t: %s", moduleName, txt);
        break;

    case ERR_WARNING:
        tc_warning("Module %s: %s", moduleName, txt);
        break;

    case ERR_ERROR:
        tc_warning("ERROR: Module %s: %s", moduleName, txt);

        tcDone();

        tc_QuitGame();
        break;
    }
}

void pcErrClose(void)
{
}

void DebugMsg(DebugE type, ErrorModuleE moduleId, const char *format, ...)
{
    va_list arglist;
    char txt[512];

    va_start(arglist, format);
    vsnprintf(txt, 512, format, arglist);
    va_end(arglist);

    ErrDebugMsg(type, moduleNames[moduleId], txt);
}
