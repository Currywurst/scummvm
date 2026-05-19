/*
**	$Filename: error/error.c
**	$Release:
**	$Revision:	04.10.1994 (hg)
**	$Date:
**
**	functions for error handling
**
**	(C) 1993, 1994 ...and avoid panic by, H. Gaberschek
**	    All Rights Reserved
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#define ERR_EXIT_ERROR      20L
#define ERR_EXIT_SHUTDOWN   30L

#include "base/base.h"
#include "platform/tc_debug.h"
#include "platform/tc_fs.h"

/* tc_QuitGame() does a longjmp back to the main game loop.
 * Using it instead of exit() lets ScummVM shut down cleanly
 * (Return-to-Launcher) rather than killing the whole process. */
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

bool pcErrOpen(S32 l_Mode, char *ErrorFilename)
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

        strcpy(ErrorHandler.Filename, ErrorFilename);
        break;
    default:
        break;
    }

    return alright;
}

void ErrorMsg(ErrorE type, ErrorModuleE moduleId, U32 errorId)
{
    DebugMsg(ERR_DEBUG, moduleId, "Error %d", errorId);

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
    vsprintf(txt, format, arglist);
    va_end(arglist);

    ErrDebugMsg(type, moduleNames[moduleId], txt);
}
