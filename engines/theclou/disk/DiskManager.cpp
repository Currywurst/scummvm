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

/* ScummVM forbidden-symbol exceptions for this pure C++ file.
 * DiskManager.cpp is new ScummVM-style code; it must not use the raw
 * forbidden functions, but some legacy game headers included below still
 * need them.  We use scumm_strlcpy / scumm_stricmp from common/str.h
 * instead of strcpy / stricmp, so no exceptions are needed here.      */

#define FORBIDDEN_SYMBOL_EXCEPTION_printf

#include "common/scummsys.h"
#include "common/str.h"       // scumm_stricmp, Common::strlcpy
#include "common/debug.h"
#include "common/textconsole.h"

#include "theclou/disk/DiskManager.h"
#include "theclou/platform/tc_fs.h"
#include "theclou/platform/TcDebug.h"

// disk.h and disk.eh bring in the game error codes and DSK_PATH_MAX.
// They must come AFTER common/scummsys.h to avoid min/max macro conflicts.
#include "theclou/disk/disk.h"
#include "theclou/disk/disk.eh"
#include "theclou/error/error.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef DIR_SEP
#  ifdef WIN32
#    define DIR_SEP "\\"
#  else
#    define DIR_SEP "/"
#  endif
#endif

namespace TheClou {

// ---------------------------------------------------------------------------
// Constructor / Destructor
// ---------------------------------------------------------------------------

DiskManager::DiskManager()
	: _rootPath("."), _savePath("."), _savePathSet(false) {
}

DiskManager::~DiskManager() {
}

// ---------------------------------------------------------------------------
// Path management
// ---------------------------------------------------------------------------

void DiskManager::setRootPath(const char *path) {
	_rootPath = path ? path : ".";
	if (!_savePathSet)
		_savePath = _rootPath;
	tcDebug(1, "TheClou: DiskManager::setRootPath -> '%s'", _rootPath.c_str());
}

char *DiskManager::getRootPath(char *result) const {
	Common::strlcpy(result, _rootPath.c_str(), DSK_PATH_MAX);
	return result;
}

void DiskManager::setSavePath(const char *path) {
	if (path && path[0]) {
		_savePath    = path;
		_savePathSet = true;
	}
}

char *DiskManager::getSavePath(char *result) const {
	Common::strlcpy(result, _savePath.c_str(), DSK_PATH_MAX);
	return result;
}

void DiskManager::initSaveDir() {
	char saveDataDisk[DSK_PATH_MAX];
	char gameDataDisk[DSK_PATH_MAX];

	snprintf(saveDataDisk, sizeof(saveDataDisk), "%s" DIR_SEP "datadisk",
	         _savePath.c_str());

	tcWarning("TheClou: DiskManager::initSaveDir saveDir='%s' rootDir='%s' savePathSet=%d",
	          saveDataDisk, _rootPath.c_str(), (int)_savePathSet);

	if (!tc_fs_isdir(saveDataDisk)) {
		int r = tc_fs_mkdir(saveDataDisk);
		tcWarning("TheClou: initSaveDir mkdir '%s' -> %s",
		          saveDataDisk, r == 0 ? "OK" : "FAILED");
	} else {
		tcWarning("TheClou: initSaveDir datadisk dir already exists");
	}

	/* Try UPPER then lower-case for the game's own DATADISK directory */
	snprintf(gameDataDisk, sizeof(gameDataDisk), "%s" DIR_SEP "DATADISK",
	         _rootPath.c_str());
	if (!tc_fs_isdir(gameDataDisk)) {
		snprintf(gameDataDisk, sizeof(gameDataDisk), "%s" DIR_SEP "datadisk",
		         _rootPath.c_str());
	}

	if (!tc_fs_isdir(gameDataDisk)) {
		tcWarning("TheClou: game DATADISK directory not found under '%s'",
		          _rootPath.c_str());
		return;
	}

	int copied = tc_fs_copy_dir_missing(gameDataDisk, saveDataDisk);
	tcWarning("TheClou: initSaveDir: copied %d file(s) from '%s' to '%s'",
	          copied, gameDataDisk, saveDataDisk);
}

// ---------------------------------------------------------------------------
// Path builder
// ---------------------------------------------------------------------------

static void strToUpper(char *s) {
	while (*s) { *s = (char)toupper((unsigned char)*s); ++s; }
}

static void strToLower(char *s) {
	while (*s) { *s = (char)tolower((unsigned char)*s); ++s; }
}

bool DiskManager::buildPathName(int check, const char *dir,
                                const char *file, char *result) const {
	/* Choose base: save path for DATADISK, game root for everything else. */
	const char *base = (scumm_stricmp(dir, DATADISK) == 0)
	                   ? _savePath.c_str() : _rootPath.c_str();

	char Dir [DSK_PATH_MAX];
	char File[DSK_PATH_MAX];
	int  step = 0;

	for (;;) {
		switch (step++) {
		case 0:
			Common::strlcpy(Dir,  dir,  DSK_PATH_MAX);
			Common::strlcpy(File, file, DSK_PATH_MAX);
			break;
		case 1:
			strToUpper(Dir);
			strToUpper(File);
			break;
		case 2:
			strToLower(Dir);
			strToLower(File);
			break;
		default:
			snprintf(result, DSK_PATH_MAX, "%s" DIR_SEP "%s" DIR_SEP "%s",
			         base, dir, file);
			tcWarning("TheClou: buildPathName FAILED dir='%s' file='%s' base='%s'",
			          dir, file, base);
			return false;
		}

		if (check == DISK_CHECK_FILE) {
			snprintf(result, DSK_PATH_MAX, "%s" DIR_SEP "%s" DIR_SEP "%s",
			         base, Dir, File);
		} else {
			snprintf(result, DSK_PATH_MAX, "%s" DIR_SEP "%s", base, Dir);
		}

		int found = tc_fs_exists(result);
		tcDebug(2, "TheClou: buildPathName probe '%s' -> %s",
		        result, found ? "OK" : "miss");
		if (found)
			break;
	}

	if (check == DISK_CHECK_DIR) {
		Common::strlcat(result, DIR_SEP,  DSK_PATH_MAX);
		Common::strlcat(result, File,     DSK_PATH_MAX);
	}
	return true;
}

// ---------------------------------------------------------------------------
// File I/O
// ---------------------------------------------------------------------------

TC_FILE *DiskManager::openFile(const char *path, const char *mode) const {
	DebugMsg(ERR_DEBUG, ERROR_MODULE_DISK, "Opening :%s (%s)", path, mode);
	TC_FILE *fp = tc_fopen(path, mode);
	if (!fp) {
		DebugMsg(ERR_WARNING, ERROR_MODULE_DISK, "Open :%s", path);
	}
	return fp;
}

void DiskManager::closeFile(TC_FILE *fp) const {
	tc_fclose(fp);
}

void *DiskManager::loadFile(const char *path) const {
	uint8 *ptr;
	size_t size = BUFSIZ;
	size_t pos  = 0;

	ptr = (uint8 *)malloc(size);
	if (!ptr)
		return nullptr;

	TC_FILE *fp = openFile(path, "rb");
	if (fp) {
		size_t nread;
		while ((nread = tc_fread(ptr + pos, 1, BUFSIZ, fp)) == BUFSIZ) {
			uint8 *tmp;
			pos  += nread;
			tmp   = (uint8 *)realloc(ptr, size + BUFSIZ);
			if (!tmp) { free(ptr); closeFile(fp); return nullptr; }
			ptr   = tmp;
			size += nread;
		}
		pos += nread;

		if (pos) {
			uint8 *tmp = (uint8 *)realloc(ptr, pos);
			if (!tmp) { free(ptr); closeFile(fp); return nullptr; }
			ptr = tmp;
		}
		closeFile(fp);
		return (void *)ptr;
	}

	free(ptr);
	return nullptr;
}

void DiskManager::saveFile(const char *path, void *src, size_t size) const {
	TC_FILE *fp = openFile(path, "wb");
	if (fp) {
		writeBytes(fp, src, size);
		closeFile(fp);
	}
}

size_t DiskManager::fileLength(const char *path) const {
	return tc_fs_filesize(path);
}

// ---------------------------------------------------------------------------
// Read helpers
// ---------------------------------------------------------------------------

void DiskManager::readBytes(TC_FILE *fp, void *dest, size_t size) const {
	if (tc_fread(dest, 1, size, fp) != size)
		ErrorMsg(Disk_Defect, ERROR_MODULE_DISK, ERR_DISK_READ_FAILED);
}

void DiskManager::readU8(TC_FILE *fp, uint8 *x) const {
	uint8 tmp; readBytes(fp, &tmp, sizeof(tmp)); *x = tmp;
}

void DiskManager::readS8(TC_FILE *fp, int8 *x) const {
	int8 tmp; readBytes(fp, &tmp, sizeof(tmp)); *x = tmp;
}

void DiskManager::readU16LE(TC_FILE *fp, uint16 *x) const {
	uint8 tmp[2]; readBytes(fp, tmp, sizeof(tmp));
	*x = (uint16)((uint16)tmp[0] | ((uint16)tmp[1] << 8));
}

void DiskManager::readS16LE(TC_FILE *fp, int16 *x) const {
	uint8 tmp[2]; readBytes(fp, tmp, sizeof(tmp));
	*x = (int16)((uint16)tmp[0] | ((uint16)tmp[1] << 8));
}

void DiskManager::readU32LE(TC_FILE *fp, uint32 *x) const {
	uint8 tmp[4]; readBytes(fp, tmp, sizeof(tmp));
	*x = (uint32)tmp[0] | ((uint32)tmp[1] << 8)
	   | ((uint32)tmp[2] << 16) | ((uint32)tmp[3] << 24);
}

void DiskManager::readS32LE(TC_FILE *fp, int32 *x) const {
	uint8 tmp[4]; readBytes(fp, tmp, sizeof(tmp));
	*x = (int32)((uint32)tmp[0] | ((uint32)tmp[1] << 8)
	   | ((uint32)tmp[2] << 16) | ((uint32)tmp[3] << 24));
}

// ---------------------------------------------------------------------------
// Write helpers
// ---------------------------------------------------------------------------

void DiskManager::writeBytes(TC_FILE *fp, void *src, size_t size) const {
	if (tc_fwrite(src, 1, size, fp) != size)
		ErrorMsg(Disk_Defect, ERROR_MODULE_DISK, ERR_DISK_WRITE_FAILED);
}

void DiskManager::writeU8(TC_FILE *fp, uint8 *x) const {
	uint8 tmp = *x; writeBytes(fp, &tmp, sizeof(tmp));
}

void DiskManager::writeS8(TC_FILE *fp, int8 *x) const {
	int8 tmp = *x; writeBytes(fp, &tmp, sizeof(tmp));
}

void DiskManager::writeU16LE(TC_FILE *fp, uint16 *x) const {
	uint8 tmp[2];
	tmp[0] = (uint8)((*x) & 0xff);
	tmp[1] = (uint8)((*x >> 8) & 0xff);
	writeBytes(fp, tmp, sizeof(tmp));
}

void DiskManager::writeS16LE(TC_FILE *fp, int16 *x) const {
	uint8 tmp[2];
	tmp[0] = (uint8)((*x) & 0xff);
	tmp[1] = (uint8)((*x >> 8) & 0xff);
	writeBytes(fp, tmp, sizeof(tmp));
}

void DiskManager::writeU32LE(TC_FILE *fp, uint32 *x) const {
	uint8 tmp[4];
	tmp[0] = (uint8)((*x) & 0xff);
	tmp[1] = (uint8)((*x >> 8) & 0xff);
	tmp[2] = (uint8)((*x >> 16) & 0xff);
	tmp[3] = (uint8)((*x >> 24) & 0xff);
	writeBytes(fp, tmp, sizeof(tmp));
}

void DiskManager::writeS32LE(TC_FILE *fp, int32 *x) const {
	uint8 tmp[4];
	tmp[0] = (uint8)((*x) & 0xff);
	tmp[1] = (uint8)((*x >> 8) & 0xff);
	tmp[2] = (uint8)((*x >> 16) & 0xff);
	tmp[3] = (uint8)((*x >> 24) & 0xff);
	writeBytes(fp, tmp, sizeof(tmp));
}

// ---------------------------------------------------------------------------
// Line reader
// ---------------------------------------------------------------------------

bool DiskManager::getLine(char *s, int size, TC_FILE *fp) const {
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

} // End of namespace TheClou
