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

/**
 * @file tc_fs.cpp
 *
 * Two distinct responsibilities:
 *
 *  1. TheClou::TcFile implementation
 *     Real file I/O backed by Common::File / Common::DumpFile.
 *
 *  2. Plain C-callable wrappers (extern "C")
 *     tc_fopen / tc_fread / … are trivial one-liners that delegate to
 *     a TcFile instance.  C source files never know about TcFile directly.
 *
 *  3. File-system utility stubs (tc_fs_exists, tc_fs_mkdir, …)
 *     Operate on paths via Common::FSNode; no TcFile equivalent.
 */

#define FORBIDDEN_SYMBOL_EXCEPTION_printf

#include "common/scummsys.h"
#include "common/fs.h"
#include "common/file.h"
#include "common/str.h"
#include "common/stream.h"

#include "theclou/platform/TcFile.h"
#include "theclou/platform/tc_fs.h"

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

// ---------------------------------------------------------------------------
// Helper
// ---------------------------------------------------------------------------

static inline Common::Path nativePath(const char *path) {
	return Common::Path(path, Common::Path::kNativeSeparator);
}

// ===========================================================================
// TheClou::TcFile — implementation
// ===========================================================================

namespace TheClou {

TcFile::TcFile()
	: _rstream(nullptr), _wstream(nullptr), _wpos(0) {
}

TcFile::~TcFile() {
	close();
}

bool TcFile::open(const char *path, const char *mode) {
	if (!path || !mode)
		return false;

	close();

	if (mode[0] == 'r') {
		Common::FSNode node(nativePath(path));
		Common::File *file = new Common::File();
		if (!file->open(node)) {
			delete file;
			return false;
		}
		_rstream = file;
	} else if (mode[0] == 'w') {
		Common::DumpFile *dump = new Common::DumpFile();
		if (!dump->open(nativePath(path))) {
			delete dump;
			return false;
		}
		_wstream = dump;
		_wpos    = 0;
	} else {
		return false;
	}

	return true;
}

void TcFile::close() {
	if (_rstream) {
		Common::File *file = dynamic_cast<Common::File *>(_rstream);
		if (file)
			file->close();
		else
			delete _rstream;
		_rstream = nullptr;
	}
	if (_wstream) {
		Common::DumpFile *dump = dynamic_cast<Common::DumpFile *>(_wstream);
		if (dump)
			dump->close();
		else
			delete _wstream;
		_wstream = nullptr;
	}
	_wpos = 0;
}

bool TcFile::isOpen() const {
	return (_rstream != nullptr) || (_wstream != nullptr);
}

// ---- Read -----------------------------------------------------------------

size_t TcFile::read(void *buf, size_t size, size_t count) {
	if (!_rstream || !buf || size == 0 || count == 0)
		return 0;
	uint32 total = (uint32)(size * count);
	uint32 got   = _rstream->read(buf, total);
	return (size_t)(got / size);
}

int TcFile::readByte() {
	if (!_rstream || _rstream->eos())
		return -1;
	byte b;
	if (_rstream->read(&b, 1) != 1)
		return -1;
	return (int)(unsigned char)b;
}

char *TcFile::readLine(char *s, int n) {
	if (!_rstream || n <= 0 || !s)
		return nullptr;
	if (_rstream->eos())
		return nullptr;

	int i = 0;
	while (i < n - 1) {
		byte b;
		if (_rstream->read(&b, 1) != 1)
			break;
		char c = (char)(unsigned char)b;
		s[i++] = c;
		if (c == '\n')
			break;
	}
	if (i == 0)
		return nullptr;
	s[i] = '\0';
	return s;
}

int TcFile::readScanf(const char *fmt, ...) {
	if (!_rstream)
		return -1;

	char buf[256];
	if (!readLine(buf, (int)sizeof(buf)))
		return -1;

	va_list args;
	va_start(args, fmt);
	int result = vsscanf(buf, fmt, args);
	va_end(args);
	return result;
}

// ---- Write ----------------------------------------------------------------

size_t TcFile::write(const void *buf, size_t size, size_t count) {
	if (!_wstream || !buf || size == 0 || count == 0)
		return 0;
	uint32 total   = (uint32)(size * count);
	uint32 written = _wstream->write(buf, total);
	_wpos += (long)written;
	return count;
}

int TcFile::printf(const char *fmt, ...) {
	if (!_wstream)
		return -1;

	va_list args;
	va_start(args, fmt);
	Common::String msg = Common::String::vformat(fmt, args);
	va_end(args);

	if (!msg.empty()) {
		_wstream->write(msg.c_str(), msg.size());
		_wpos += (long)msg.size();
	}
	return (int)msg.size();
}

// ---- Positioning ----------------------------------------------------------

long TcFile::tell() const {
	if (_rstream)
		return (long)_rstream->pos();
	if (_wstream)
		return _wpos;
	return -1L;
}

bool TcFile::eof() const {
	if (!_rstream)
		return true;
	return _rstream->eos();
}

int TcFile::ungetc(int c) {
	if (!_rstream)
		return -1;
	int64 pos = _rstream->pos();
	if (pos <= 0)
		return -1;
	if (!_rstream->seek(pos - 1))
		return -1;
	return (unsigned char)c;
}

void TcFile::rewind() {
	if (_rstream)
		_rstream->seek(0);
}

} // End of namespace TheClou

// ===========================================================================
// File-system utilities  (Common::FSNode based, no TcFile equivalent)
// ===========================================================================

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
		return 0;
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

	Common::FSNode dstNode(nativePath(dst));
	if (dstNode.exists()) return 0;

	Common::FSNode srcNode(nativePath(src));
	Common::File fin;
	if (!fin.open(srcNode)) return -1;

	Common::DumpFile fout;
	if (!fout.open(nativePath(dst))) {
		fin.close();
		return -1;
	}

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

extern "C" int tc_fs_copy_dir_missing(const char *srcDir, const char *dstDir) {
	if (!srcDir || !dstDir) return -1;

	Common::FSNode dstNode(nativePath(dstDir));
	if (!dstNode.exists())
		dstNode.createDirectory();

	Common::FSNode srcNode(nativePath(srcDir));
	if (!srcNode.exists() || !srcNode.isDirectory()) return -1;

	Common::FSList children;
	if (!srcNode.getChildren(children, Common::FSNode::kListFilesOnly))
		return 0;

	int copied = 0;
	for (Common::FSList::const_iterator it = children.begin();
	     it != children.end(); ++it) {
		Common::String name      = it->getName();
		Common::String nameLower = name;
		nameLower.toLowercase();

		Common::Path dstPath = nativePath(dstDir).appendComponent(nameLower);

		Common::FSNode dstFile(dstPath);
		if (dstFile.exists()) continue;

		Common::File fin;
		if (!fin.open(*it)) continue;

		Common::DumpFile fout;
		if (!fout.open(dstPath)) { fin.close(); continue; }

		byte buf[4096];
		uint32 n;
		while (!fin.eos()) {
			n = fin.read(buf, sizeof(buf));
			if (n > 0) fout.write(buf, n);
		}
		fin.close();
		fout.close();
		copied++;
	}
	return copied;
}

// ===========================================================================
// extern "C" wrappers — trivial delegators to TcFile
// ===========================================================================

extern "C" TC_FILE *tc_fopen(const char *path, const char *mode) {
	TC_FILE *f = new TC_FILE();
	if (!f->open(path, mode)) {
		delete f;
		return nullptr;
	}
	return f;
}

extern "C" void tc_fclose(TC_FILE *f) {
	if (f) {
		f->close();
		delete f;
	}
}

extern "C" size_t tc_fread(void *buf, size_t size, size_t count, TC_FILE *f) {
	if (!f) return 0;
	return f->read(buf, size, count);
}

extern "C" size_t tc_fwrite(const void *buf, size_t size, size_t count, TC_FILE *f) {
	if (!f) return 0;
	return f->write(buf, size, count);
}

extern "C" int tc_fgetc(TC_FILE *f) {
	if (!f) return -1;
	return f->readByte();
}

extern "C" char *tc_fgets(char *s, int n, TC_FILE *f) {
	if (!f) return nullptr;
	return f->readLine(s, n);
}

extern "C" int tc_fprintf(TC_FILE *f, const char *fmt, ...) {
	if (!f) return -1;
	/* We cannot call f->printf() from a variadic wrapper without forwarding
	 * the va_list, so we format here and delegate to write() so _wpos stays
	 * in sync. */
	va_list args;
	va_start(args, fmt);
	Common::String msg = Common::String::vformat(fmt, args);
	va_end(args);
	return (int)f->write(msg.c_str(), 1, msg.size());
}

extern "C" long tc_ftell(TC_FILE *f) {
	if (!f) return -1L;
	return f->tell();
}

extern "C" int tc_feof(TC_FILE *f) {
	if (!f) return 1;
	return f->eof() ? 1 : 0;
}

extern "C" int tc_ungetc(int c, TC_FILE *f) {
	if (!f) return -1;
	return f->ungetc(c);
}

extern "C" void tc_rewind(TC_FILE *f) {
	if (f) f->rewind();
}

extern "C" int tc_fscanf(TC_FILE *f, const char *fmt, ...) {
	if (!f) return -1;
	char buf[256];
	if (!f->readLine(buf, (int)sizeof(buf)))
		return -1;
	va_list args;
	va_start(args, fmt);
	int result = vsscanf(buf, fmt, args);
	va_end(args);
	return result;
}
