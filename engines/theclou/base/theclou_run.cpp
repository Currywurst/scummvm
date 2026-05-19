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
 * theclou_run.cpp — ScummVM entry point for the Der Clou! game loop.
 *
 * This file is intentionally C++ (not C) so that it can catch the
 * TheClou::QuitException thrown by tc_QuitGame().  All game-logic
 * functions it calls are compiled as C and forward-declared here with
 * extern "C" linkage.
 *
 * Call sequence (from TheClouEngine::run() in engine.cpp):
 *
 *   engine.cpp                 theclou_run.cpp          game C code
 *   ──────────                 ───────────────          ──────────
 *   TheClouEngine::run()
 *     └─ theclou_run(path) ──► parseOptions(0,NULL)
 *                               rndInit()
 *                               inpClearKbBuffer()
 *                               dskSetRootPath(path)
 *                               dskInitSaveDir()
 *                               try {
 *                                 tcInit() ──────────► game initialisation
 *                                 tcDo()  ──────────► main game loop
 *                                                       │
 *                                           tc_QuitGame() called
 *                                                       │
 *                                           throw QuitException
 *                               } catch (QuitException) { }
 *                               tcDone() ─────────── ► cleanup / free mem
 *                             ◄── returns normally
 *     └─ return kNoError
 */

#include <cstddef>
#include "theclou/platform/tc_quit.h"

/* Forward-declare every C function this file needs.                       */
/* We do NOT include the legacy base/base.h because it pulls in SDL compat */
/* headers that conflict with ScummVM types when compiled as C++.          */
extern "C" {
	/* base/base.c */
	void parseOptions(int argc, char **argv);
	void rndInit(void);
	bool tcInit(void);
	void tcDo(void);
	void tcDone(void);

	/* inphdl/inphdl.c */
	void inpClearKbBuffer(void);

	/* disk/disk.c */
	void dskSetRootPath(const char *path);
	void dskInitSaveDir(void);
}

/**
 * ScummVM entry point — called by TheClouEngine::run() in engine.cpp.
 *
 * Runs the full game: initialisation → main loop → cleanup.
 * Returns normally when the game exits (user quit, error, or
 * Return-to-Launcher), so TheClouEngine::run() can return kNoError
 * to the ScummVM launcher.
 *
 * @param rootPath  Absolute path to the game's root directory
 *                  (the folder that contains DATA/, DATADISK/, etc.).
 *                  Passed through from ConfMan "path" in engine.cpp.
 */
extern "C" void theclou_run(const char *rootPath) {
	/* Initialise setup defaults (volumes, debug flags, etc.).
	 * In the standalone build this is done by parseOptions(argc, argv).
	 * Inside ScummVM there is no argv, so we call with argc=0 which skips
	 * all argument parsing but still sets every field to its default
	 * (including SfxVolume = MusicVolume = SND_MAX_VOLUME).             */
	parseOptions(0, NULL);

	rndInit();
	inpClearKbBuffer();

	dskSetRootPath(rootPath ? rootPath : ".");

	/* Ensure <savepath>/datadisk/ exists and contains the save-slot
	 * template files (GAMES.LST / ORIGIN.LST) copied from the game
	 * directory.  Must be called AFTER dskSetRootPath.                  */
	dskInitSaveDir();

	/* Run the game.  tc_QuitGame() — called from the error handler,
	 * input handler, or quit menu — throws QuitException to break out
	 * of tcInit()/tcDo() without calling exit().  tcDone() is always
	 * reached so that all game resources are freed before we return.    */
	try {
		if (tcInit())
			tcDo();
	} catch (const TheClou::QuitException &) {
		/* Normal quit path — fall through to tcDone() below. */
	}

	tcDone();
}
