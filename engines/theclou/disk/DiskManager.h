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

#ifndef ENGINES_THECLOU_DISK_DISKMANAGER_H
#define ENGINES_THECLOU_DISK_DISKMANAGER_H

/**
 * @file DiskManager.h
 * @brief TheClou::DiskManager — path management and file I/O subsystem.
 *
 * DiskManager encapsulates all mutable disk-state that was previously stored
 * in file-scope globals in disk.cpp:
 *
 *   RootPathName[]   — path to the read-only game data directory
 *   SaveRootPath[]   — path for writable save-game data (datadisk/)
 *   SaveRootPathSet  — flag: was a separate save path configured?
 *
 * ScummVM conventions:
 *   - namespace TheClou {}
 *   - Common::NonCopyable base
 *   - member variables prefixed with _
 *   - Common::String / Common::Path instead of char[]
 *
 * Migration note
 * --------------
 * The legacy global free functions (dskSetRootPath, dskOpen, …) still exist
 * in disk.cpp; they now delegate to a module-level DiskManager instance
 * (g_diskManager).  In Schritt 4 that instance will be promoted to a member
 * of TheClouEngine so it follows proper RAII lifetime management.
 */

#include "common/scummsys.h"
#include "common/noncopyable.h"
#include "common/str.h"

/* TC_FILE forward declaration — avoid pulling scummsys.h into game headers
 * that define min/max before DiskManager.h is included. */
namespace TheClou { class TcFile; }
typedef TheClou::TcFile TC_FILE;

namespace TheClou {

class DiskManager : public Common::NonCopyable {
public:
	DiskManager();
	~DiskManager();

	// ----------------------------------------------------------------
	// Path management
	// ----------------------------------------------------------------

	/** Set the root path of the read-only game data directory. */
	void setRootPath(const char *path);

	/** Get the root path into @p result (must be at least DSK_PATH_MAX bytes). */
	char *getRootPath(char *result) const;

	/**
	 * Set a separate path for writable save-game data (datadisk/).
	 * If never called, the root path is used for saves as well.
	 */
	void setSavePath(const char *path);

	/** Get the effective save path into @p result. */
	char *getSavePath(char *result) const;

	/**
	 * Initialise the save directory:
	 *   - Creates <savepath>/datadisk/ if absent.
	 *   - Seeds it with all files from the game's own DATADISK directory
	 *     (idempotent — skips files that already exist).
	 * Call once after setRootPath() and setSavePath().
	 */
	void initSaveDir();

	// ----------------------------------------------------------------
	// Path builder
	// ----------------------------------------------------------------

	/**
	 * Build a fully qualified path from Directory + Filename, trying
	 * the original case, upper-case, and lower-case variants until one
	 * resolves on disk.
	 *
	 * @param check    DISK_CHECK_FILE or DISK_CHECK_DIR
	 * @param dir      Sub-directory name (e.g. "DATADISK", "DATA")
	 * @param file     File name within that directory
	 * @param result   Output buffer (at least DSK_PATH_MAX bytes)
	 * @returns true on success, false if no case variant was found.
	 */
	bool buildPathName(int check, const char *dir,
	                   const char *file, char *result) const;

	// ----------------------------------------------------------------
	// File I/O
	// ----------------------------------------------------------------

	/**
	 * Open a file.  Returns a new TC_FILE* on success, nullptr on failure.
	 * The caller is responsible for closing it with closeFile().
	 */
	TC_FILE *openFile(const char *path, const char *mode) const;

	/** Close a file returned by openFile(). */
	void closeFile(TC_FILE *fp) const;

	/**
	 * Load the entire contents of a file into a heap-allocated buffer.
	 * The caller owns the buffer and must free() it.
	 * Returns nullptr on failure.
	 */
	void *loadFile(const char *path) const;

	/** Save @p size bytes from @p src to the file at @p path. */
	void saveFile(const char *path, void *src, size_t size) const;

	/** Return the byte-length of the file at @p path (0 if absent). */
	size_t fileLength(const char *path) const;

	// ----------------------------------------------------------------
	// Read helpers (typed, little-endian)
	// Use ScummVM types (uint8/int8/uint16/int16/uint32/int32) so this
	// header stays independent of the game's U8/S8/U16/S16/U32/S32 aliases.
	// ----------------------------------------------------------------
	void readBytes(TC_FILE *fp, void *dest, size_t size) const;
	void readU8  (TC_FILE *fp, uint8  *x) const;
	void readS8  (TC_FILE *fp, int8   *x) const;
	void readU16LE(TC_FILE *fp, uint16 *x) const;
	void readS16LE(TC_FILE *fp, int16  *x) const;
	void readU32LE(TC_FILE *fp, uint32 *x) const;
	void readS32LE(TC_FILE *fp, int32  *x) const;

	// ----------------------------------------------------------------
	// Write helpers (typed, little-endian)
	// ----------------------------------------------------------------
	void writeBytes(TC_FILE *fp, void *src, size_t size) const;
	void writeU8  (TC_FILE *fp, uint8  *x) const;
	void writeS8  (TC_FILE *fp, int8   *x) const;
	void writeU16LE(TC_FILE *fp, uint16 *x) const;
	void writeS16LE(TC_FILE *fp, int16  *x) const;
	void writeU32LE(TC_FILE *fp, uint32 *x) const;
	void writeS32LE(TC_FILE *fp, int32  *x) const;

	// ----------------------------------------------------------------
	// Line reader
	// ----------------------------------------------------------------
	/**
	 * Read one CR/LF-terminated line from @p fp into @p s.
	 * Returns true if a line was read, false at EOF.
	 */
	bool getLine(char *s, int size, TC_FILE *fp) const;

private:
	/** Path to the read-only game data root directory. */
	Common::String _rootPath;

	/** Path for writable save data.  Equals _rootPath until setSavePath() is called. */
	Common::String _savePath;

	/** True once setSavePath() has been called with a non-empty path. */
	bool _savePathSet;
};

} // End of namespace TheClou

// ---------------------------------------------------------------------------
// Global singleton accessor
//
// The legacy free functions in disk.cpp delegate to this instance.
// In Schritt 4 it will be replaced by a TheClouEngine member.
// ---------------------------------------------------------------------------
extern TheClou::DiskManager *g_diskManager;

#endif // ENGINES_THECLOU_DISK_DISKMANAGER_H
