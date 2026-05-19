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

/*
 * tc_fs.cpp — C-callable bridges to ScummVM's portable file-system and
 * I/O APIs (Common::FSNode, Common::File, Common::DumpFile).
 *
 * C source files in the engine cannot call these C++ APIs directly.
 * These thin wrappers expose them via extern "C" so every .c file can
 * include platform/tc_fs.h and use TC_FILE* instead of FILE*.
 */

#define FORBIDDEN_SYMBOL_EXCEPTION_printf

#include "common/scummsys.h"
#include "common/fs.h"
#include "common/file.h"
#include "common/str.h"
#include "common/stream.h"

#include "theclou/platform/tc_fs.h"

#include <stdarg.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/* Internal TC_FILE struct definition                                  */
/* ------------------------------------------------------------------ */

struct TC_FILE {
	Common::SeekableReadStream  *rstream; /* non-null for read modes  */
	Common::WriteStream         *wstream; /* non-null for write modes */
};

/* Helper: construct a Common::Path from a native OS path string. */
static inline Common::Path nativePath(const char *path) {
	return Common::Path(path, Common::Path::kNativeSeparator);
}

/* ------------------------------------------------------------------ */
/* File-system utilities                                               */
/* ------------------------------------------------------------------ */

extern "C" int tc_fs_exists(const char *path) {
	if (!path || !path[0]) return 0;
	Common::FSNode node(nativePath(path));
	return node.exists() ? 1 : 0;
}

extern "C" int tc_fs_isdir(const char *path) {
	if (!path || !path[0]) return 0;
	Common::FSNode node(nativePath(path));
	return (node.exists() && node.isDirectory()) ? 1 : 0;
}

extern "C" int tc_fs_mkdir(const char *path) {
	if (!path || !path[0]) return -1;
	Common::FSNode node(nativePath(path));
	if (node.exists() && node.isDirectory())
		return 0; /* already there */
	return node.createDirectory() ? 0 : -1;
}

extern "C" size_t tc_fs_filesize(const char *path) {
	if (!path || !path[0]) return 0;
	Common::FSNode node(nativePath(path));
	if (!node.exists() || node.isDirectory()) return 0;
	Common::File f;
	if (!f.open(node)) return 0;
	int64 sz = f.size();
	f.close();
	return (sz > 0) ? (size_t)sz : 0;
}

extern "C" int tc_fs_copy_if_missing(const char *src, const char *dst) {
	if (!src || !dst) return -1;

	/* Skip if destination already exists */
	Common::FSNode dstNode(nativePath(dst));
	if (dstNode.exists()) return 0;

	/* Open source for reading */
	Common::FSNode srcNode(nativePath(src));
	Common::File fin;
	if (!fin.open(srcNode)) return -1;

	/* Open destination for writing */
	Common::DumpFile fout;
	if (!fout.open(nativePath(dst))) {
		fin.close();
		return -1;
	}

	/* Copy in chunks */
	byte buf[4096];
	uint32 n;
	while (!fin.eos()) {
		n = fin.read(buf, sizeof(buf));
		if (n > 0)
			fout.write(buf, n);
	}

	fin.close();
	fout.close();
	return 0;
}

/* ------------------------------------------------------------------ */
/* File I/O                                                            */
/* ------------------------------------------------------------------ */

extern "C" TC_FILE *tc_fopen(const char *path, const char *mode) {
	if (!path || !mode) return nullptr;

	TC_FILE *f = new TC_FILE();
	f->rstream = nullptr;
	f->wstream = nullptr;

	if (mode[0] == 'r') {
		/* Read mode: use Common::File via FSNode */
		Common::FSNode node(nativePath(path));
		Common::File *file = new Common::File();
		if (!file->open(node)) {
			delete file;
			delete f;
			return nullptr;
		}
		f->rstream = file;
	} else if (mode[0] == 'w') {
		/* Write mode: use Common::DumpFile (creates/truncates) */
		Common::DumpFile *dump = new Common::DumpFile();
		if (!dump->open(nativePath(path))) {
			delete dump;
			delete f;
			return nullptr;
		}
		f->wstream = dump;
	} else {
		/* Unsupported mode */
		delete f;
		return nullptr;
	}

	return f;
}

extern "C" void tc_fclose(TC_FILE *f) {
	if (!f) return;

	if (f->rstream) {
		/* Common::File is also a SeekableReadStream; cast and close */
		Common::File *file = dynamic_cast<Common::File *>(f->rstream);
		if (file)
			file->close();
		else
			delete f->rstream;
		f->rstream = nullptr;
	}
	if (f->wstream) {
		Common::DumpFile *dump = dynamic_cast<Common::DumpFile *>(f->wstream);
		if (dump)
			dump->close();
		else
			delete f->wstream;
		f->wstream = nullptr;
	}

	delete f;
}

extern "C" size_t tc_fread(void *buf, size_t size, size_t count, TC_FILE *f) {
	if (!f || !f->rstream || !buf || size == 0 || count == 0) return 0;
	uint32 total   = (uint32)(size * count);
	uint32 got     = f->rstream->read(buf, total);
	return (size_t)(got / size);
}

extern "C" size_t tc_fwrite(const void *buf, size_t size, size_t count, TC_FILE *f) {
	if (!f || !f->wstream || !buf || size == 0 || count == 0) return 0;
	uint32 total = (uint32)(size * count);
	f->wstream->write(buf, total);
	return count; /* DumpFile/WriteStream doesn't report partial writes */
}

extern "C" int tc_fgetc(TC_FILE *f) {
	if (!f || !f->rstream || f->rstream->eos()) return -1;
	return (int)(unsigned char)f->rstream->readByte();
}

extern "C" char *tc_fgets(char *s, int n, TC_FILE *f) {
	if (!f || !f->rstream || n <= 0 || !s) return nullptr;
	if (f->rstream->eos()) return nullptr;

	int i = 0;
	while (i < n - 1) {
		if (f->rstream->eos()) break;
		char c = (char)(unsigned char)f->rstream->readByte();
		s[i++] = c;
		if (c == '\n') break;
	}
	if (i == 0) return nullptr;
	s[i] = '\0';
	return s;
}

extern "C" int tc_fprintf(TC_FILE *f, const char *fmt, ...) {
	if (!f || !f->wstream) return -1;

	va_list args;
	va_start(args, fmt);
	Common::String msg = Common::String::vformat(fmt, args);
	va_end(args);

	if (!msg.empty())
		f->wstream->write(msg.c_str(), msg.size());
	return (int)msg.size();
}
