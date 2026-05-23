/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * Original game code copyright (c) 1993-2001 respective authors
 * (see individual files for details).
 * Portions copyright (c) 2005 Vasco Alexandre da Silva Costa
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

#ifndef ENGINES_THECLOU_BASE_BASE_H
#define ENGINES_THECLOU_BASE_BASE_H

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <fcntl.h>

#include "theclou.h"
#include "error/error.h"
#include "list/list.h"
#include "memory/memory.h"
#include "disk/disk.h"
#include "inphdl/inphdl.h"

#include "gfx/gfx.h"
#include "text/text.h"
#include "anim/sysanim.h"
#include "present/interac.h"
#include "data/relation.h"

#include "data/database.h"
#include "gameplay/gp.h"
#include "sound/newsound.h"
#include "sound/fx.h"

#include "cdrom/cdrom.h"

/*
 *
 *
 */
struct Setup {
    bool FullScreen;
    bool Profidisk;
    bool CDRom;
    S32 SfxVolume;
    S32 MusicVolume;
    unsigned Debug;
    bool CDAudio;
    bool CDAudioFromCD;
    bool CDAudioFromWav;
    unsigned Scale;
    bool ScaleOverride;
};

extern struct Setup setup;


/*
 * Verwendungszweck:
 * StdBuffer0: geladenes Bild
 *
 * StdBuffer1: entpacktes Bild
 */

#define STD_BUFFER0_SIZE  (320 * 140)
#define STD_BUFFER1_SIZE  (61 * 1024)	/* weniger geht nicht! (Bild + CMAP) */

extern char prgname[255];

extern void *StdBuffer0, *StdBuffer1;

extern void tcClearStdBuffer(void *p_Buffer);

extern void tcSetPermanentColors(void);
void tcDone(void);

U32 tcGetProcessorType(void);

#define PROC_80286		1L
#define PROC_80386		2L
#define PROC_80486		3L

/* ScummVM integration entry point (see base.c) */
void theclou_run(const char *rootPath);

#endif  // ENGINES_THECLOU_BASE_BASE_H
