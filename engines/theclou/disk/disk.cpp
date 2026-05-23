/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * Original game code copyright (c) 1993-2001 respective authors
 * (see individual files for details).
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

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "error/error.h"
#include "platform/tc_debug.h"
#include "platform/tc_fs.h"

#include "disk/disk.h"
#include "disk/DiskManager.h"

// ---------------------------------------------------------------------------
// Global DiskManager singleton
// ---------------------------------------------------------------------------
TheClou::DiskManager *g_diskManager = nullptr;

/** Lazily created if not yet set by the engine. */
static TheClou::DiskManager *getDM() {
	if (!g_diskManager)
		g_diskManager = new TheClou::DiskManager();
	return g_diskManager;
}

// ---------------------------------------------------------------------------
// Legacy global that some headers still read directly (e.g. disk.h extern).
// Kept in sync by setRootPath().
// ---------------------------------------------------------------------------
char RootPathName[DSK_PATH_MAX];

// ---------------------------------------------------------------------------
// Free-function API  (thin delegators)
// ---------------------------------------------------------------------------

void dskSetRootPath(const char *newRootPath) {
	getDM()->setRootPath(newRootPath);
	/* Keep the legacy global in sync for any code that reads it directly. */
	strncpy(RootPathName, newRootPath ? newRootPath : ".", DSK_PATH_MAX - 1);
	RootPathName[DSK_PATH_MAX - 1] = '\0';
}

char *dskGetRootPath(char *result) {
	return getDM()->getRootPath(result);
}

void dskSetSavePath(const char *savePath) {
	getDM()->setSavePath(savePath);
}

char *dskGetSavePath(char *result) {
	return getDM()->getSavePath(result);
}

void dskInitSaveDir(void) {
	getDM()->initSaveDir();
}

TC_FILE *dskOpen(const char *Pathname, const char *Mode) {
	return getDM()->openFile(Pathname, Mode);
}

void *dskLoad(const char *Pathname) {
	return getDM()->loadFile(Pathname);
}

void dskSave(char *Pathname, void *src, size_t size) {
	getDM()->saveFile(Pathname, src, size);
}

bool dskBuildPathName(DiskCheckE check,
                      const char *Directory, const char *Filename, char *Result) {
	return getDM()->buildPathName((int)check, Directory, Filename, Result);
}

size_t dskFileLength(const char *Pathname) {
	return getDM()->fileLength(Pathname);
}

void dskClose(TC_FILE *fp) {
	getDM()->closeFile(fp);
}

void dskWrite(TC_FILE *fp, void *src, size_t size) {
	getDM()->writeBytes(fp, src, size);
}

void dskWrite_U8 (TC_FILE *fp, U8  *x) { getDM()->writeU8 (fp, x); }
void dskWrite_S8 (TC_FILE *fp, S8  *x) { getDM()->writeS8 (fp, x); }
void dskWrite_U16LE(TC_FILE *fp, U16 *x) { getDM()->writeU16LE(fp, x); }
void dskWrite_S16LE(TC_FILE *fp, S16 *x) { getDM()->writeS16LE(fp, x); }
void dskWrite_U32LE(TC_FILE *fp, U32 *x) { getDM()->writeU32LE(fp, x); }
void dskWrite_S32LE(TC_FILE *fp, S32 *x) { getDM()->writeS32LE(fp, x); }

void dskRead(TC_FILE *fp, void *dest, size_t size) {
	getDM()->readBytes(fp, dest, size);
}

void dskRead_U8 (TC_FILE *fp, U8  *x) { getDM()->readU8 (fp, x); }
void dskRead_S8 (TC_FILE *fp, S8  *x) { getDM()->readS8 (fp, x); }
void dskRead_U16LE(TC_FILE *fp, U16 *x) { getDM()->readU16LE(fp, x); }
void dskRead_S16LE(TC_FILE *fp, S16 *x) { getDM()->readS16LE(fp, x); }
void dskRead_U32LE(TC_FILE *fp, U32 *x) { getDM()->readU32LE(fp, x); }
void dskRead_S32LE(TC_FILE *fp, S32 *x) { getDM()->readS32LE(fp, x); }

bool dskGetLine(char *s, int size, TC_FILE *fp) {
	return getDM()->getLine(s, size, fp);
}

// ---------------------------------------------------------------------------
// String helpers (not part of DiskManager — pure utility)
// ---------------------------------------------------------------------------

int tc_stricmp(const char *s1, const char *s2) {
	while (*s1 && *s2) {
		int a = toupper((unsigned char)*s1++);
		int b = toupper((unsigned char)*s2++);
		if (a < b) return -1;
		if (a > b) return +1;
	}
	if (!*s1 && *s2) return -1;
	if (*s1 && !*s2) return +1;
	return 0;
}

int tc_strnicmp(const char *s1, const char *s2, size_t n) {
	while (*s1 && *s2 && n--) {
		int a = toupper((unsigned char)*s1++);
		int b = toupper((unsigned char)*s2++);
		if (a < b) return -1;
		if (a > b) return +1;
	}
	if (!*s1 && *s2) return -1;
	if (*s1 && !*s2) return +1;
	return 0;
}
