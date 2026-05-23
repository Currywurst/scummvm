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
 * Call sequence (from TheClouEngine::run() in engine.cpp):
 *
 *   engine.cpp                 theclou_run.cpp          game code
 *   ──────────                 ───────────────          ─────────
 *   TheClouEngine::run()
 *     └─ theclou_run(path) ──► parseOptions(0,NULL)
 *                               rndInit()
 *                               inpClearKbBuffer()
 *                               dskSetRootPath(path)
 *                               dskInitSaveDir()
 *                               tcInit() + tcDo()  ──►  game loop
 *                                                        │
 *                                        tc_QuitGame() called
 *                                        (sets g_tcShouldQuit,
 *                                         breaks all inner loops)
 *                                                        │
 *                                        tcDo() returns normally
 *                               tcDone() ─────────────── cleanup
 *                             ◄── returns normally
 *     └─ return kNoError
 */

#include <cstddef>

void parseOptions(int argc, char **argv);
void rndInit(void);
bool tcInit(void);
void tcDo(void);
void tcDone(void);
void inpClearKbBuffer(void);
void dskSetRootPath(const char *path);
void dskInitSaveDir(void);

extern "C" void theclou_run(const char *rootPath) {
	parseOptions(0, nullptr);
	rndInit();
	inpClearKbBuffer();

	dskSetRootPath(rootPath ? rootPath : ".");
	dskInitSaveDir();

	if (tcInit())
		tcDo();

	/* tcDo() returns either at natural game end or after tc_ShouldQuit()
	 * breaks all loops — always do orderly cleanup. */
	tcDone();
}
