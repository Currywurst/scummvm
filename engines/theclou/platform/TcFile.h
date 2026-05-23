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

#ifndef ENGINES_THECLOU_PLATFORM_TCFILE_H
#define ENGINES_THECLOU_PLATFORM_TCFILE_H

/**
 * @file TcFile.h
 * @brief TheClou::TcFile — portable file I/O over ScummVM's stream layer.
 *
 * This class replaces the old TC_FILE opaque struct + extern "C" wrappers.
 * C source files still use the tc_fopen / tc_fclose / … wrappers declared
 * in platform/tc_fs.h; those wrappers are now trivial one-liner delegators
 * that forward every call to a TcFile instance.
 *
 * ScummVM conventions used here:
 *   - namespace TheClou {}
 *   - member variables prefixed with _
 *   - Common::NonCopyable base (no copy ctor / assignment)
 *   - Common::Path / Common::String instead of raw char*
 *   - Common::SeekableReadStream, Common::WriteStream, Common::DumpFile
 */

#include "common/scummsys.h"
#include "common/noncopyable.h"
#include "common/fs.h"
#include "common/file.h"
#include "common/stream.h"

namespace TheClou {

/**
 * Portable file handle.
 *
 * Wraps ScummVM's Common::File (read) and Common::DumpFile (write) behind
 * a single interface that is ABI-compatible with the legacy TC_FILE pointer
 * used throughout the C game code.
 *
 * Open modes:
 *   "r" / "rb"  — read-only  (Common::File via FSNode)
 *   "w" / "wb"  — write/create, truncate  (Common::DumpFile)
 */
class TcFile : public Common::NonCopyable {
public:
	TcFile();
	~TcFile();

	// ----------------------------------------------------------------
	// Open / close
	// ----------------------------------------------------------------

	/**
	 * Open a file for reading ("r"/"rb") or writing ("w"/"wb").
	 * Returns true on success, false if the file could not be opened.
	 */
	bool open(const char *path, const char *mode);

	/** Close the file.  Safe to call on an already-closed handle. */
	void close();

	/** Returns true if the file is currently open. */
	bool isOpen() const;

	// ----------------------------------------------------------------
	// Read operations
	// ----------------------------------------------------------------

	/**
	 * Read @p count items of @p size bytes each into @p buf.
	 * Returns the number of complete items read.
	 */
	size_t read(void *buf, size_t size, size_t count);

	/**
	 * Read and return the next byte as an unsigned char cast to int,
	 * or -1 at end-of-file.  Replaces fgetc().
	 * Named readByte() to avoid collision with common/forbidden.h 'getc' macro.
	 */
	int readByte();

	/**
	 * Read at most @p n - 1 characters into @p s, stopping at '\\n' or EOF.
	 * Always NUL-terminates.  Returns @p s on success, nullptr at EOF.
	 * Replaces fgets().
	 * Named readLine() to avoid collision with common/forbidden.h 'gets' macro.
	 */
	char *readLine(char *s, int n);

	/**
	 * Formatted read (like fscanf): reads one line and applies vsscanf.
	 * Returns the number of successfully assigned items, or -1 on error.
	 * Named readScanf() to avoid collision with common/forbidden.h 'scanf' macro.
	 */
	int readScanf(const char *fmt, ...);

	// ----------------------------------------------------------------
	// Write operations
	// ----------------------------------------------------------------

	/**
	 * Write @p count items of @p size bytes each from @p buf.
	 * Returns @p count on success, 0 on error.
	 */
	size_t write(const void *buf, size_t size, size_t count);

	/**
	 * Write a formatted string.  Returns bytes written, or -1 on error.
	 * Replaces fprintf().
	 */
	int printf(const char *fmt, ...) GCC_PRINTF(2, 3);

	// ----------------------------------------------------------------
	// Positioning
	// ----------------------------------------------------------------

	/**
	 * Return current byte offset from file start, or -1 on error.
	 * Replaces ftell().
	 *
	 * In write mode the position is tracked internally (DumpFile has no
	 * seek / tell).  In read mode it delegates to rstream->pos().
	 */
	long tell() const;

	/** Returns true if end-of-file has been reached (read mode only). */
	bool eof() const;

	/**
	 * Seek back one byte.  Returns the pushed-back byte on success, -1 on
	 * error.  Only one byte of push-back is guaranteed.  Replaces ungetc().
	 */
	int ungetc(int c);

	/** Seek to the beginning of the file.  Replaces rewind(). */
	void rewind();

private:
	Common::SeekableReadStream *_rstream; ///< non-null in read mode
	Common::WriteStream        *_wstream; ///< non-null in write mode
	long                        _wpos;   ///< bytes written (write mode tell)
};

} // End of namespace TheClou

// ---------------------------------------------------------------------------
// C compatibility typedef
//
// C source files forward-declare TC_FILE via platform/tc_fs.h.  In C++ mode
// TC_FILE is just an alias for TheClou::TcFile so no bridge struct is needed.
// ---------------------------------------------------------------------------
#ifdef __cplusplus
typedef TheClou::TcFile TC_FILE;
#endif

#endif // ENGINES_THECLOU_PLATFORM_TCFILE_H
