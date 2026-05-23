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

/*
**	$Filename: data\database.ph
**	$Release:  1
**	$Revision: 0
**	$Date:     09-03-94
**
**	database private definitions for "Der Clou!"
**
** (c) 1994 ...and avoid panic by, Kaweh Kazemi
**	All Rights Reserved.
**
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#ifndef MODULE_DATABASE_PH
#define MODULE_DATABASE_PH

/* includes */


/* private defines */
#define dbGetObjectReal(key)  (((struct dbObject *)key)-1)
#define dbGetObjectKey(obj)   ((void *)(obj+1))
#define dbGetObjectHashNr(nr) ((U8)(((nr) % OBJ_HASH_SIZE)))

#define EOS                   ((char)'\0')


/* private definitions */
extern LIST *objHash[OBJ_HASH_SIZE];
extern char decodeStr[11];

extern U32 ObjectListType;
extern U32 ObjectListFlags;


/* private prototypes - RELATION */
int dbCompare(KEY key1, KEY key2);
char *dbDecode(KEY key);
KEY dbEncode(char *key);

/* private prototypes - OBJECT */
struct dbObject *dbFindRealObject(U32 realNr, U32 offset, U32 size);
#endif
