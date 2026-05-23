/****************************************************************************
  Copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#ifndef ENGINES_THECLOU_DISK_DISK_H
#define ENGINES_THECLOU_DISK_DISK_H

#include "platform/tc_fs.h"

#include "theclou.h"

#define DSK_PATH_MAX 512

typedef enum {
 DISK_CHECK_FILE,
 DISK_CHECK_DIR
} DiskCheckE;

extern void dskSetRootPath(const char *newRootPath);
extern char *dskGetRootPath(char *result);

/* Optional separate path for save files (datadisk/ directory).
 * If not set, falls back to RootPathName (original behaviour).  */
extern void  dskSetSavePath(const char *savePath);
extern char *dskGetSavePath(char *result);

/* Create <savepath>/datadisk/ and seed it with GAMES.LST / ORIGIN.LST
 * from the game directory if they are not already present.
 * Call once after dskSetRootPath() and dskSetSavePath(). */
extern void dskInitSaveDir(void);

void *dskLoad(const char *Pathname);
void dskSave(char *Pathname, void *src, size_t size);
bool dskBuildPathName(DiskCheckE check,
		      const char *Directory, const char *Filename, char *Result);
void dskClose(TC_FILE *fp);
void dskWrite(TC_FILE *fp, void *src, size_t size);

void dskWrite_U8(TC_FILE *fp, U8 *x);
void dskWrite_S8(TC_FILE *fp, S8 *x);
void dskWrite_U16LE(TC_FILE *fp, U16 *x);
void dskWrite_S16LE(TC_FILE *fp, S16 *x);
void dskWrite_U32LE(TC_FILE *fp, U32 *x);
void dskWrite_S32LE(TC_FILE *fp, S32 *x);

void dskRead(TC_FILE *fp, void *dest, size_t size);

void dskRead_U8(TC_FILE *fp, U8 *x);
void dskRead_S8(TC_FILE *fp, S8 *x);
void dskRead_U16LE(TC_FILE *fp, U16 *x);
void dskRead_S16LE(TC_FILE *fp, S16 *x);
void dskRead_U32LE(TC_FILE *fp, U32 *x);
void dskRead_S32LE(TC_FILE *fp, S32 *x);


size_t dskFileLength(const char *Pathname);

TC_FILE *dskOpen(const char *Pathname, const char *Mode);

extern bool dskGetLine(char *s, int size, TC_FILE *fp);

/* Case-insensitive string comparison helpers.
 * Renamed from stricmp/strnicmp to avoid collision with ScummVM's
 * common/forbidden.h which bans those POSIX names.
 * Use scumm_stricmp / scumm_strnicmp (common/str.h) in pure C++ files. */
int tc_stricmp (const char *s1, const char *s2);
int tc_strnicmp(const char *s1, const char *s2, size_t n);

#endif  // ENGINES_THECLOU_DISK_DISK_H
