/****************************************************************************
  Copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "error/error.h"
#include "platform/tc_debug.h"
#include "platform/tc_fs.h"

#include "disk/disk.h"
#include "disk/disk.eh"

char RootPathName[DSK_PATH_MAX];

/* SaveRootPath is used for DATADISK (save game) access.
 * Defaults to RootPathName when not explicitly set.     */
static char SaveRootPath[DSK_PATH_MAX];
static int  SaveRootPathSet = 0;

void dskSetRootPath(const char *newRootPath)
{
    strcpy(RootPathName, newRootPath);
    /* If no explicit save path was set, mirror here too */
    if (!SaveRootPathSet)
        strcpy(SaveRootPath, newRootPath);
}

char *dskGetRootPath(char *result)
{
    return strcpy(result, RootPathName);
}

void dskSetSavePath(const char *savePath)
{
    if (savePath && savePath[0]) {
        strcpy(SaveRootPath, savePath);
        SaveRootPathSet = 1;
    }
}

char *dskGetSavePath(char *result)
{
    return strcpy(result, SaveRootPathSet ? SaveRootPath : RootPathName);
}

/* ---------------------------------------------------------------------------
 * dskInitSaveDir
 *
 * Ensures the save directory contains the DATADISK sub-folder and seeds it
 * with GAMES.LST / ORIGIN.LST from the original game data directory.
 * These two files drive the save-slot list; without them the save/load menu
 * refuses to open.
 *
 * The function is idempotent — it does nothing when the files already exist.
 * Uses tc_fs_* wrappers (Common::FSNode / Common::File) — no POSIX stat/mkdir.
 * ---------------------------------------------------------------------------*/
void dskInitSaveDir(void)
{
    char saveDataDisk[DSK_PATH_MAX];
    char gameDataDisk[DSK_PATH_MAX];
    char srcPath[DSK_PATH_MAX];
    char dstPath[DSK_PATH_MAX];

    /* The save root is either explicitly set or falls back to the game root.
     * Either way, we want a "datadisk" sub-directory inside it.          */
    snprintf(saveDataDisk, sizeof(saveDataDisk), "%s" DIR_SEP "datadisk",
             SaveRootPathSet ? SaveRootPath : RootPathName);

    tc_debug(1, "TheClou: save directory = %s", saveDataDisk);

    /* Create <savepath>/datadisk/ if it does not exist yet */
    if (!tc_fs_isdir(saveDataDisk))
        tc_fs_mkdir(saveDataDisk);

    /* Locate the game's own DATADISK directory (try mixed/upper/lower case) */
    snprintf(gameDataDisk, sizeof(gameDataDisk), "%s" DIR_SEP "DATADISK",
             RootPathName);
    if (!tc_fs_isdir(gameDataDisk)) {
        snprintf(gameDataDisk, sizeof(gameDataDisk), "%s" DIR_SEP "datadisk",
                 RootPathName);
    }

    /* Copy GAMES.LST (template save-slot list) */
    snprintf(srcPath, sizeof(srcPath), "%s" DIR_SEP "GAMES.LST", gameDataDisk);
    snprintf(dstPath, sizeof(dstPath), "%s" DIR_SEP "games.lst", saveDataDisk);
    tc_fs_copy_if_missing(srcPath, dstPath);
    /* Fallback: try lowercase source name */
    if (!tc_fs_exists(dstPath)) {
        snprintf(srcPath, sizeof(srcPath), "%s" DIR_SEP "games.lst",
                 gameDataDisk);
        tc_fs_copy_if_missing(srcPath, dstPath);
    }

    /* Copy ORIGIN.LST (pristine reference for "not saved yet" comparison) */
    snprintf(srcPath, sizeof(srcPath), "%s" DIR_SEP "ORIGIN.LST", gameDataDisk);
    snprintf(dstPath, sizeof(dstPath), "%s" DIR_SEP "origin.lst", saveDataDisk);
    tc_fs_copy_if_missing(srcPath, dstPath);
    if (!tc_fs_exists(dstPath)) {
        snprintf(srcPath, sizeof(srcPath), "%s" DIR_SEP "origin.lst",
                 gameDataDisk);
        tc_fs_copy_if_missing(srcPath, dstPath);
    }
}

TC_FILE *dskOpen(const char *Pathname, const char *Mode)
{
    TC_FILE *fp;

    DebugMsg(ERR_DEBUG, ERROR_MODULE_DISK, "Opening :%s (%s)", Pathname, Mode);

    fp = tc_fopen(Pathname, Mode);
    if (!fp) {
        /* Use ERR_WARNING (not ERR_ERROR) so we return NULL gracefully.
         * ERR_ERROR would call tc_QuitGame() immediately, bypassing all the
         * NULL-checks that callers (ReadList, dbSaveAllObjects, …) rely on. */
        DebugMsg(ERR_WARNING, ERROR_MODULE_DISK, "Open :%s", Pathname);
    }

    return fp;
}

void *dskLoad(const char *Pathname)
{
    TC_FILE *fp;
    U8 *ptr;
    size_t size, pos;

    pos  = 0;
    size = BUFSIZ;
    ptr  = (U8 *)malloc(size);

    if (!ptr)
        return NULL;

    if ((fp = dskOpen(Pathname, "rb"))) {
        size_t nread = 0;

        while ((nread = tc_fread(ptr + pos, 1, BUFSIZ, fp)) == BUFSIZ) {
            U8 *tmp;

            pos  += nread;
            tmp   = (U8 *)realloc(ptr, size + BUFSIZ);
            if (!tmp) {
                free(ptr);
                dskClose(fp);
                return NULL;
            }

            ptr  = tmp;
            size += nread;
        }

        pos += nread;

        if (pos) {
            U8 *tmp = (U8 *)realloc(ptr, pos);
            if (!tmp) {
                free(ptr);
                dskClose(fp);
                return NULL;
            }
            ptr = tmp;
        }

        dskClose(fp);
        return (void *) ptr;
    }

    free(ptr);
    return NULL;
}

void dskSave(char *Pathname, void *src, size_t size)
{
    TC_FILE *fp;

    if ((fp = dskOpen(Pathname, "wb"))) {
        dskWrite(fp, src, size);
        dskClose(fp);
    }
}

static void strUpper(char *s)
{
    while (*s != '\0') {
        *s = toupper(*s);
        s++;
    }
}

static void strLower(char *s)
{
    while (*s != '\0') {
        *s = tolower(*s);
        s++;
    }
}

bool dskBuildPathName(DiskCheckE check,
		      const char *Directory, const char *Filename, char *Result)
{
    char Dir [DSK_PATH_MAX];
    char File[DSK_PATH_MAX];

    /* Use the save path for DATADISK (saves), game root for everything else */
    const char *BaseDir = (stricmp(Directory, DATADISK) == 0)
                          ? SaveRootPath : RootPathName;

    int step = 0;

    do {
        switch (step++) {
        case 0:
            strcpy(Dir, Directory);
            strcpy(File, Filename);
            break;

        case 1:
            strUpper(Dir);
            strUpper(File);
            break;

        case 2:
            strLower(Dir);
            strLower(File);
            break;

        case 3:
            snprintf(Result, DSK_PATH_MAX, "%s" DIR_SEP "%s" DIR_SEP "%s",
                BaseDir, Directory, Filename);

            DebugMsg(ERR_DEBUG, ERROR_MODULE_DISK,
                "Path failure: %s", Result);
            return false;
        }

        if (check == DISK_CHECK_FILE) {
            snprintf(Result, DSK_PATH_MAX, "%s" DIR_SEP "%s" DIR_SEP "%s",
                BaseDir, Dir, File);
         } else {
            snprintf(Result, DSK_PATH_MAX, "%s" DIR_SEP "%s", BaseDir, Dir);
         }

    } while (!tc_fs_exists(Result));

    if (check == DISK_CHECK_DIR) {
        strcat(Result, DIR_SEP);
        strcat(Result, File);
    }

    return true;
}

size_t dskFileLength(const char *Pathname)
{
    return tc_fs_filesize(Pathname);
}

void dskClose(TC_FILE *fp)
{
    tc_fclose(fp);
}

void dskWrite(TC_FILE *fp, void *src, size_t size)
{
    if (tc_fwrite(src, 1, size, fp) != size) {
        ErrorMsg(Disk_Defect, ERROR_MODULE_DISK, ERR_DISK_WRITE_FAILED);
    }
}

void dskWrite_U8(TC_FILE *fp, U8 *x)
{
    U8 tmp = *x;
    dskWrite(fp, &tmp, sizeof(tmp));
}

void dskWrite_S8(TC_FILE *fp, S8 *x)
{
    S8 tmp = *x;
    dskWrite(fp, &tmp, sizeof(tmp));
}

void dskWrite_U16LE(TC_FILE *fp, U16 *x)
{
    U8 tmp[2];
    tmp[0] = (U8)((*x) & 0xff);
    tmp[1] = (U8)((*x >> 8) & 0xff);
    dskWrite(fp, &tmp, sizeof(tmp));
}

void dskWrite_S16LE(TC_FILE *fp, S16 *x)
{
    U8 tmp[2];
    tmp[0] = (U8)((*x) & 0xff);
    tmp[1] = (U8)((*x >> 8) & 0xff);
    dskWrite(fp, &tmp, sizeof(tmp));
}

void dskWrite_U32LE(TC_FILE *fp, U32 *x)
{
    U8 tmp[4];
    tmp[0] = (U8)((*x) & 0xff);
    tmp[1] = (U8)((*x >> 8) & 0xff);
    tmp[2] = (U8)((*x >> 16) & 0xff);
    tmp[3] = (U8)((*x >> 24) & 0xff);
    dskWrite(fp, &tmp, sizeof(tmp));
}

void dskWrite_S32LE(TC_FILE *fp, S32 *x)
{
    U8 tmp[4];
    tmp[0] = (U8)((*x) & 0xff);
    tmp[1] = (U8)((*x >> 8) & 0xff);
    tmp[2] = (U8)((*x >> 16) & 0xff);
    tmp[3] = (U8)((*x >> 24) & 0xff);
    dskWrite(fp, &tmp, sizeof(tmp));
}

void dskRead(TC_FILE *fp, void *dest, size_t size)
{
    if (tc_fread(dest, 1, size, fp) != size) {
        ErrorMsg(Disk_Defect, ERROR_MODULE_DISK, ERR_DISK_READ_FAILED);
    }
}

void dskRead_U8(TC_FILE *fp, U8 *x)
{
    U8 tmp;
    dskRead(fp, &tmp, sizeof(tmp));
    *x = tmp;
}

void dskRead_S8(TC_FILE *fp, S8 *x)
{
    S8 tmp;
    dskRead(fp, &tmp, sizeof(tmp));
    *x = tmp;
}

void dskRead_U16LE(TC_FILE *fp, U16 *x)
{
    U8 tmp[2];
    dskRead(fp, &tmp, sizeof(tmp));
    *x = (U16)((U16)tmp[0] | ((U16)tmp[1] << 8));
}

void dskRead_S16LE(TC_FILE *fp, S16 *x)
{
    U8 tmp[2];
    dskRead(fp, &tmp, sizeof(tmp));
    *x = (S16)((U16)tmp[0] | ((U16)tmp[1] << 8));
}

void dskRead_U32LE(TC_FILE *fp, U32 *x)
{
    U8 tmp[4];
    dskRead(fp, &tmp, sizeof(tmp));
    *x = (U32)((U32)tmp[0] | ((U32)tmp[1] << 8)
        | ((U32)tmp[2] << 16) | ((U32)tmp[3] << 24));
}

void dskRead_S32LE(TC_FILE *fp, S32 *x)
{
    U8 tmp[4];
    dskRead(fp, &tmp, sizeof(tmp));
    *x = (S32)((U32)tmp[0] | ((U32)tmp[1] << 8)
        | ((U32)tmp[2] << 16) | ((U32)tmp[3] << 24));
}

bool dskGetLine(char *s, int size, TC_FILE *fp)
{
    int ch;

    while ((ch = tc_fgetc(fp)) != -1 && ch != '\r' && size-- > 0)
        *s++ = (char)ch;
    *s = '\0';

    while (ch != '\r' && (ch = tc_fgetc(fp)) != -1)
        ;

    if (ch == -1)
        return false;

    tc_fgetc(fp); /* consume trailing '\n' */
    return true;
}

int stricmp(const char *s1, const char *s2)
{
    while (*s1 && *s2) {
        int a, b;

        a = toupper(*s1++);
        b = toupper(*s2++);

        if (a < b) return -1;
        if (a > b) return +1;
    }

    if (*s1 == '\0' && *s2 != '\0') return -1;
    if (*s1 != '\0' && *s2 == '\0') return +1;
    return 0;
}

int strnicmp(const char *s1, const char *s2, size_t n)
{
    while (*s1 && *s2 && n--) {
        int a, b;

        a = toupper(*s1++);
        b = toupper(*s2++);

        if (a < b) return -1;
        if (a > b) return +1;
    }

    if (*s1 == '\0' && *s2 != '\0') return -1;
    if (*s1 != '\0' && *s2 == '\0') return +1;
    return 0;
}
