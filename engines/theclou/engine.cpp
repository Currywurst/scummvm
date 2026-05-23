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
 *
 */

#include "theclou/engine.h"
#include "theclou/disk/DiskManager.h"

#include "common/config-manager.h"
#include "common/debug.h"
#include "common/fs.h"
#include "common/str.h"
#include "common/system.h"
#include "common/savefile.h"
#include "engines/util.h"

// Forward-declare entry points from other translation units.
// theclou_run / tc_SetPaused / tc_InitPlatform are defined with
// extern "C" in their respective .cpp files so the symbols are
// unmangled and callable from here.
extern "C" {
	void theclou_run(const char *rootPath);    /* base/theclou_run.cpp */
	void tc_SetPaused(int paused);             /* platform/tc_platform.cpp */
	void tc_InitPlatform(void);                /* platform/tc_platform.cpp */
}

namespace TheClou {

TheClouEngine::TheClouEngine(OSystem *syst, const ADGameDescription *gd)
	: Engine(syst), _gameDescription(gd),
	  _diskManager(new DiskManager()) {
	/* Publish the DiskManager as the global singleton so C game code
	 * (which still calls dskSetRootPath / dskOpen etc.) works without changes. */
	g_diskManager = _diskManager;
}

TheClouEngine::~TheClouEngine() {
	/* Clear the global before deleting so dangling use-after-free is obvious. */
	g_diskManager = nullptr;
	delete _diskManager;
	_diskManager = nullptr;
}

bool TheClouEngine::hasFeature(EngineFeature f) const {
	return (f == kSupportsReturnToLauncher);
}

void TheClouEngine::pauseEngineIntern(bool pause) {
	// Let the base class pause/resume the mixer
	Engine::pauseEngineIntern(pause);
	// Signal our platform layer so tc_Delay() spin-waits while paused
	tc_SetPaused(pause ? 1 : 0);
}

Common::Error TheClouEngine::run() {
	// The original game renders at 320x200 in 8-bit palette mode.
	initGraphics(320, 200);

	// Game data path (read-only game assets).
	Common::String rootPath = ConfMan.get("path");
	debug(1, "TheClou: starting, root path = '%s'", rootPath.c_str());

	// Quick sanity-check: the DATA sub-directory must exist under rootPath.
	// If not, the path is misconfigured (user pointed at the wrong folder).
	{
		Common::FSNode rootNode(Common::Path(rootPath, Common::Path::kNativeSeparator));
		Common::FSNode dataNode = rootNode.getChild("DATA");
		if (!dataNode.exists()) {
			dataNode = rootNode.getChild("data");
		}
		if (!dataNode.exists()) {
			warning("TheClou: DATA sub-directory not found under '%s'."
			        " Make sure the game path points to the DER CLOU! root"
			        " (the folder that contains DATA/, DATADISK/, TEXTS/ etc.).",
			        rootPath.c_str());
		} else {
			debug(1, "TheClou: DATA directory found at '%s'", dataNode.getPath().toString(Common::Path::kNativeSeparator).c_str());
		}
	}

	// Redirect save files to ScummVM's dedicated save directory so saves
	// appear in the launcher and work with cloud sync / ScummVM backups.
	Common::String savePath = ConfMan.get("savepath");
	if (!savePath.empty()) {
		_diskManager->setSavePath(savePath.c_str());
		debug(1, "TheClou: save path = '%s'", savePath.c_str());
	}

	// Record this thread as the main game thread so tc_Delay() can
	// distinguish it from background threads (e.g. the audio mix thread).
	// Background threads must NOT pump events or call tc_QuitGame.
	tc_InitPlatform();

	// Blocking call — runs the full game loop; returns when the game exits
	// (either via the in-game quit menu or ScummVM's Return-to-Launcher).
	theclou_run(rootPath.c_str());

	return Common::kNoError;
}

} // End of namespace TheClou
