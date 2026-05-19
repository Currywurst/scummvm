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
 * This file is C++ (not C) so that it can forward-declare the C game
 * functions with extern "C" linkage and call them from a single clean
 * entry point.  The setjmp/longjmp quit mechanism is isolated in
 * platform/tc_quit.h — see that file for the design rationale.
 *
 * ScummVM builds with -fno-exceptions, so C++ throw/catch cannot be used.
 * longjmp is safe here because there are no C++ objects with non-trivial
 * destructors on the stack between the setjmp checkpoint and the longjmp
 * sites deep in the C game code.
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
 *                               g_tcQuitJmpValid = 1
 *                               setjmp(g_tcQuitJmp) ──► game init + loop
 *                                                         │
 *                                           tc_QuitGame() called
 *                                                         │
 *                                           longjmp back to setjmp
 *                               g_tcQuitJmpValid = 0
 *                               tcDone() ─────────────── cleanup
 *                             ◄── returns normally
 *     └─ return kNoError
 */

#include <cstddef>
#include "theclou/platform/tc_quit.h"

/* Forward-declare every C function this file needs.                        */
/* We do NOT include the legacy base/base.h because it pulls in SDL compat  */
/* headers that conflict with ScummVM types when compiled as C++.           */
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
 */
extern "C" void theclou_run(const char *rootPath) {
	/* Initialise setup defaults (volumes, debug flags, etc.).
	 * In the standalone build this is done by parseOptions(argc, argv).
	 * Inside ScummVM there is no argv, so we call with argc=0 which skips
	 * all argument parsing but still sets every field to its default.    */
	parseOptions(0, NULL);

	rndInit();
	inpClearKbBuffer();

	dskSetRootPath(rootPath ? rootPath : ".");

	/* Ensure <savepath>/datadisk/ exists and contains the save-slot
	 * template files (GAMES.LST / ORIGIN.LST).
	 * Must be called AFTER dskSetRootPath.                              */
	dskInitSaveDir();

	/* Arm the longjmp checkpoint.  tc_QuitGame() (called from the error
	 * handler, input handler, or quit menu) longjmps back here so the
	 * game loop can be exited without calling exit().  tcDone() is
	 * always reached so all game resources are freed before we return.  */
	g_tcQuitJmpValid = 1;
	if (setjmp(g_tcQuitJmp) == 0) {
		if (tcInit())
			tcDo();
	}
	/* longjmp or normal exit — always clean up. */
	g_tcQuitJmpValid = 0;
	tcDone();
}
