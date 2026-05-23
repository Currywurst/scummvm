/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * Original game code (c) 1993 K. Kazemi, H. Gaberschek
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

/**
 * @file theclou.h
 * @brief Global type aliases, constants and forward declarations for Der Clou!
 *
 * Include order note:
 *   This header intentionally does NOT include any ScummVM headers.
 *   Many legacy game .cpp files include this header first.  Including
 *   common/scummsys.h here would pull in common/forbidden.h and ban
 *   functions (strcpy, printf, …) that are still used in legacy code.
 *
 *   Pure C++ files that also need ScummVM headers must include
 *   common/scummsys.h BEFORE theclou.h, or use FORBIDDEN_SYMBOL_EXCEPTION_*
 *   defines at the top of their .cpp file.
 */

#ifndef ENGINES_THECLOU_THECLOU_H
#define ENGINES_THECLOU_THECLOU_H

/* -------------------------------------------------------------------------
 * Standard C headers — included before ScummVM so that the legacy game code
 * can still call memcpy, memset, strlen etc. directly.  Pure C++ files that
 * include common/scummsys.h first do not need these.
 * ---------------------------------------------------------------------- */
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>  /* PRIu32, SCNu32, … */
#include <stdbool.h>
#include <stdio.h>     /* snprintf, sscanf, puts, … */
#include <ctype.h>     /* isspace, isdigit, … */
#include <string.h>

/* -------------------------------------------------------------------------
 * Game type aliases
 * These mirror ScummVM's uint8/int8/uint16/int16/uint32/int32 so that the
 * legacy game code keeps its original names.  New code should prefer the
 * ScummVM types directly.
 * ---------------------------------------------------------------------- */
typedef uint8_t  U8;
typedef int8_t   S8;
typedef uint16_t U16;
typedef int16_t  S16;
typedef uint32_t U32;
typedef int32_t  S32;

typedef U8  ubyte;
/* NOTE: 'byte' intentionally NOT defined here — conflicts with ScummVM's
 * own typedef (unsigned char) in common/scummsys.h.
 * Use ubyte or U8 instead. */
typedef U16 uword;
typedef S16 word;

/* -------------------------------------------------------------------------
 * min / max / clamp
 * Guard with #ifndef so that ScummVM's template versions (or <algorithm>)
 * take precedence when this header is included after common/scummsys.h.
 * ---------------------------------------------------------------------- */
#ifndef MAX
#  define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#  define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

/* Legacy lower-case aliases — only define when not already provided. */
#ifndef max
#  define max(a, b) MAX(a, b)
#endif
#ifndef min
#  define min(a, b) MIN(a, b)
#endif

#define clamp(x, lower, upper)  \
                                ((x) < (lower) ? \
                                    (lower) : \
                                    (x) > (upper) ? (upper) : (x))

#define array_len(array)	(sizeof(array) / sizeof((array)[0]))

#define EOS                     ((char)'\0')

#define	DIR_SEP                 "/"

/*
 * some version defines
 */

/* COMPUTER VERSION DEFINES */
/* #define THECLOU_500_VERSION */
/* define THECLOU_4000_VERSION */
/* define THECLOU_PC_VERSION */
/* define THECLOU_OS2_VERSION */
#define THECLOU_SDL_VERSION

/*  ADDITIONAL VERSION DEFINES */
/* define THECLOU_CDROM_VERSION */
#define THECLOU_PROFIDISK

/* LANGUAGE DEFINES */
#define THECLOU_VERSION_GERMAN
/* define THECLOU_VERSION_ENGLISH */
/* define THECLOU_VERSION_SLOWAKISCH */

/* PASSWORD DEFINES */
#define PASSWORD_BLOCKED
#define PASSWORD_LEN             1118
#define PASSWORD_POS            86096
#define PASSWORD                   14

/* SPECIALS DEFINES */
#define THECLOU_JOYSTICK_DISABLED
/* define THECLOU_DEBUG */

/*  PROTECTION DEFINES */
#ifdef THECLOU_OS2_VERSION
#define PASSWORD_BLOCKED
#endif

#ifndef PASSWORD_BLOCKED
#ifdef THECLOU_CDROM_VERSION
#define THECLOU_CDPROTECTION
#else
#define THECLOU_CHKSUM
#endif
#endif


#define THECLOU_NODE               50

/*
 * Textdateien, die auch als Daten verwendet werden
 *
 */

#define LOCATIONS_TXT		"location.lst"
#define COLL_LIST_TXT		"coll.lst"
#define PICT_LIST_TXT		"pict.lst"
#define GAMES_LIST_TXT		"games.lst"
#define GAMES_ORIG_TXT		"origin.lst"

#define LIV_ANIM_TEMPLATE_LIST	"template.lst"
#define LIV_LIVINGS_LIST        "livings.lst"

#define SND_SOUND_LIST          "sounds.list"
#define SND_SOUND_DIRECTORY     "sounds"

/*
 * Namen der anderen Dateien
 *
 * Diese Dateien befinden sich im Verzeichnis theclou:data
 *
 */

#define STORY_DAT               "tcstory.pc"
#define STORY_DAT_DEMO          "tcstory.dem"

/*
 * Nummern der Textdateien in denen die Menüs stehen
 *
 * siehe Texts.list im Verzeichnis Texts
 *
 */

#define MENU_TXT		UINT32_C( 0)
#define OBJECTS_TXT		UINT32_C( 1)
#define BUSINESS_TXT 		UINT32_C( 2)
#define HOUSEDESC_TXT		UINT32_C( 3)
#define THECLOU_TXT		UINT32_C( 4)
#define INVESTIGATIONS_TXT 	UINT32_C( 5)
#define CDROM_TXT 		UINT32_C( 6)
#define OBJECTS_ENUM_TXT	UINT32_C( 7)
#define ANIM_TXT		UINT32_C( 8)
#define PRESENT_TXT		UINT32_C( 9)
#define STORY_0_TXT		UINT32_C(10)
#define PLAN_TXT		UINT32_C(11)
#define TOOLS_TXT 		UINT32_C(12)
#define LOOK_TXT		UINT32_C(13)
#define ABILITY_TXT		UINT32_C(14)
#define STORY_1_TXT		UINT32_C(15)
#define TALK_0_TXT		UINT32_C(16)
#define TALK_1_TXT              UINT32_C(17)

/*
 * Disk Nr.
 *
 */

/*
 * Defines betreffend Disketten und Verzeichnisse
 *
 */

#define PICTURE_DIRECTORY	"pictures"
#define TEXT_DIRECTORY		"texts"
#define INCLUDE_DIRECTORY	"include"
#define DATA_DIRECTORY		"data"
#define SOUND_DIRECTORY 	"sounds"
#define SAMPLES_DIRECTORY	"samples"
#define AUDIO_DIRECTORY		"audio"

#define DATADISK                "datadisk"

#define BUILD_DATA_NAME 	"tcbuild"
#define MAIN_DATA_NAME		"tcmain"
#define GAME_DATA_EXT		".dat"
#define GAME_REL_EXT 		".rel"
#define TAXI_LOC_EXT 		".loc"

#define STORY_DATA_NAME 	"tcstory"

/*
 * defines for Error Module
 *
 */

#define ERR_OPEN_FUNC           tc_fopen
#define ERR_CLOSE_FUNC          tc_fclose
#define ERR_HANDLING_FUNC	ShowTheClouRequester

U32 ShowTheClouRequester(S32 error_class);	/* siehe Base.c */

/*
 * defines for Disk Module
 *
 */

#define DISK_ALLOC_FUNC         TCAllocMem
#define DISK_FREE_FUNC          TCFreeMem
#define DISK_ERR_FUNC           ErrorMsg

void ShowInsertDiskReq(U16 disk_id);

/*
 * some defines ...
 */

#define MATT_PICTID             UINT16_C(  7)
#define OLD_MATT_PICTID         UINT16_C(125)
#define BIG_SHEET               UINT16_C(  5)

#define BGD_LONDON              UINT16_C( 21)
#define BGD_PLANUNG             UINT16_C( 23)
#define BGD_EINBRUCH            UINT16_C( 23)
#define BGD_CLEAR               UINT16_C(  0)

#define RADIO_BUBBLE            UINT16_C( 12)
#define SPEAK_BUBBLE            UINT16_C( 12)
#define THINK_BUBBLE            UINT16_C( 13)

#define GFX_COLL_PARKING        UINT16_C( 27)

#endif
