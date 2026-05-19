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

#ifndef THECLOU_TC_FS_H
#define THECLOU_TC_FS_H

/*
 * tc_fs.h — C-callable wrappers around ScummVM's Common::FSNode,
 * Common::File, and Common::DumpFile.
 *
 * C source files in the engine cannot use Common::File/FSNode directly
 * (C++ linkage and templates).  These thin wrappers expose the portable
 * ScummVM file-system and I/O APIs to every .c file.
 *
 * File system:
 *   tc_fs_exists(path)          — test if a path exists
 *   tc_fs_isdir(path)           — test if it is a directory
 *   tc_fs_mkdir(path)           — create a directory
 *   tc_fs_filesize(path)        — get file size without opening
 *   tc_fs_copy_if_missing(s,d)  — copy src to dst when dst absent
 *
 * File I/O (TC_FILE replaces FILE):
 *   tc_fopen(path, mode)        — open for "r"/"rb" or "w"/"wb"
 *   tc_fclose(f)                — close file
 *   tc_fread(buf, sz, n, f)     — read n items of sz bytes
 *   tc_fwrite(buf, sz, n, f)    — write n items of sz bytes
 *   tc_fgetc(f)                 — read one byte (returns -1 at EOF)
 *   tc_fgets(s, n, f)           — read a line (NULL at EOF)
 *   tc_fprintf(f, fmt, ...)     — formatted write
 */

#include <stddef.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/* File-system utilities                                               */
/* ------------------------------------------------------------------ */

/** Returns 1 if path exists (file or directory), 0 otherwise. */
int    tc_fs_exists(const char *path);

/** Returns 1 if path exists and is a directory, 0 otherwise. */
int    tc_fs_isdir(const char *path);

/** Creates a directory at path.  Returns 0 on success, -1 on failure. */
int    tc_fs_mkdir(const char *path);

/** Returns file size in bytes, 0 if path is absent or is a directory. */
size_t tc_fs_filesize(const char *path);

/**
 * Copies the file at src to dst.
 * If dst already exists the function is a no-op (idempotent).
 * Returns 0 on success, -1 if src cannot be opened or dst cannot be
 * created.
 */
int    tc_fs_copy_if_missing(const char *src, const char *dst);

/* ------------------------------------------------------------------ */
/* Opaque file handle — replaces FILE*                                 */
/* ------------------------------------------------------------------ */

/** Opaque handle; defined in tc_fs.cpp, invisible to C callers. */
typedef struct TC_FILE TC_FILE;

/**
 * Open a file.  mode must be one of:
 *   "r" / "rb"  — read-only  (uses Common::File via FSNode)
 *   "w" / "wb"  — write/create, truncate  (uses Common::DumpFile)
 * Returns NULL on failure.
 */
TC_FILE *tc_fopen(const char *path, const char *mode);

/** Close a file opened with tc_fopen.  Safe to call with NULL. */
void     tc_fclose(TC_FILE *f);

/**
 * Read count items of size bytes each from f into buf.
 * Returns the number of complete items successfully read.
 */
size_t   tc_fread(void *buf, size_t size, size_t count, TC_FILE *f);

/**
 * Write count items of size bytes each from buf to f.
 * Returns count on success, 0 on error.
 */
size_t   tc_fwrite(const void *buf, size_t size, size_t count, TC_FILE *f);

/**
 * Read and return the next byte from f as an unsigned char cast to int,
 * or -1 (EOF) at end-of-file.
 */
int      tc_fgetc(TC_FILE *f);

/**
 * Read at most n-1 characters from f into s, stopping at '\\n' or EOF.
 * Always NUL-terminates s.  Returns s on success, NULL at EOF.
 */
char    *tc_fgets(char *s, int n, TC_FILE *f);

/**
 * Write a formatted string to f.  Uses Common::String::vformat internally
 * (no direct stdio).  Returns the number of bytes written, or -1 on error.
 */
int      tc_fprintf(TC_FILE *f, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* THECLOU_TC_FS_H */
