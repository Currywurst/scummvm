/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * Original game code (c) 1994 Kaweh Kazemi
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

/** @file text/text.h — Text / localisation subsystem for Der Clou! */

#ifndef ENGINES_THECLOU_TEXT_TEXT_H
#define ENGINES_THECLOU_TEXT_TEXT_H

#include "theclou.h"
#include "error/error.h"
#include "list/list.h"
#include "memory/memory.h"


/* public defines */
typedef enum {
    TXT_LANG_GERMAN,
    TXT_LANG_ENGLISH,
    TXT_LANG_FRENCH,
    TXT_LANG_SPANISH,
    TXT_LANG_SLOWAKISCH,
    TXT_LANG_LAST
} TxtLanguageE;

#define TXT_KEY_LENGTH     256


#define txtGetFirstLine(id, key, dest) (txtGetNthString(id, key, 0, dest))


/* public prototypes - TEXT */
void txtInit(char lang);
void txtDone(void);

void txtLoad(U32 textId);
void txtUnLoad(U32 textId);

void txtPrepare(U32 textId);
void txtUnPrepare(U32 textId);

void txtReset(U32 textId);


/* public prototypes - KEY */
char *txtGetKey(U16 keyNr, char *key);
U32 txtGetKeyAsULONG(U16 keyNr, char *key);

LIST *txtGoKey(U32 textId, const char *key);
LIST *txtGoKeyAndInsert(U32 textId, const char *key, ...);

bool txtKeyExists(U32 textId, const char *key);
U32 txtCountKey(char *key);


/* public prototypes - STRING */
char *txtGetString(U32 textId, const char *key, char *dest);
char *txtGetNthString(U32 textId, const char *key, U32 nth, char *dest);
void txtPutCharacter(LIST *list, uword pos, U8 c);

#endif // ENGINES_THECLOU_TEXT_TEXT_H
