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

#ifndef ENGINES_THECLOU_DATA_DATABASE_H
#define ENGINES_THECLOU_DATA_DATABASE_H

/* includes */
#include "theclou.h"

#include "data/relation.h"
#include "list/list.h"
#include "disk/disk.h"
#include "text/text.h"

#include "data/objstd/tcdata.h"


#define GET_OUT					((U8)-1)

/* public defines */
#define DB_LOAD_MODE_STD        1
#define DB_LOAD_MODE_NO_NAME	0

#define OLF_NORMAL            (0)
#define OLF_INCLUDE_NAME      (1)
#define OLF_INSERT_STAR       (1 << 1)
#define OLF_PRIVATE_LIST      (1 << 2)
#define OLF_ADD_PREV_STRING   (1 << 3)
#define OLF_ADD_SUCC_STRING   (1 << 4)
#define OLF_ALIGNED           (1 << 5)

#define OL_NAME(n)         ((char *)NODE_NAME((NODE *)n))
#define OL_NR(n)           (((struct ObjectNode *)n)->nr)
#define OL_TYPE(n)         (((struct ObjectNode *)n)->type)
#define OL_DATA(n)         (((struct ObjectNode *)n)->data)

#define OBJ_HASH_SIZE      31


/* public structures */
struct dbObject {
    NODE link;
    U32 nr;
    U32 type;
    U32 realNr;
};

struct dbObjectHeader {
    U32 nr;
    U32 type;
    U32 size;
};

struct ObjectNode {
    NODE Link;
    U32 nr;
    U32 type;
    void *data;
};


/* public global data */
extern LIST *ObjectList;
extern LIST *ObjectListPrivate;
extern U32 ObjectListWidth;
extern char *(*ObjectListPrevString) (U32, U32, void *);
extern char *(*ObjectListSuccString) (U32, U32, void *);

extern LIST *objHash[OBJ_HASH_SIZE];


/* public prototypes - OBJECTS */
U8 dbLoadAllObjects(char *fileName, U16 diskId);
U8 dbSaveAllObjects(char *fileName, U32 offset, U32 size, U16 diskId);
void dbDeleteAllObjects(U32 offset, U32 size);

U32 dbGetObjectCountOfDB(U32 offset, U32 size);

void dbSetLoadObjectsMode(U8 mode);

/* public prototypes - OBJECT */
void *dbNewObject(U32 nr, U32 type, U32 size, char *name, U32 realNr);
void dbDeleteObject(U32 nr);

void *dbGetObject(U32 nr);
U32 dbGetObjectNr(void *key);
char *dbGetObjectName(U32 nr, char *objName);

void *dbIsObject(U32 nr, U32 type);

/* public prototypes - OBJECTNODE */
struct ObjectNode *dbAddObjectNode(LIST * objectList, U32 nr, U32 flags);
void dbRemObjectNode(LIST * objectList, U32 nr);
struct ObjectNode *dbHasObjectNode(LIST * objectList, U32 nr);

void SetObjectListAttr(U32 flags, U32 type);
void BuildObjectList(void *key);
void ExpandObjectList(LIST * objectList, char *expandItem);

S16 dbStdCompareObjects(struct ObjectNode *obj1, struct ObjectNode *obj2);
S32 dbSortObjectList(LIST ** objectList,
		     S16(*processNode) (struct ObjectNode *,
					 struct ObjectNode *));
void dbSortPartOfList(LIST * objectList, struct ObjectNode *start,
		      struct ObjectNode *end,
		      S16(*processNode) (struct ObjectNode *,
					  struct ObjectNode *));


struct ObjectNode *dbAddObjectNode(LIST * objectList, U32 nr, U32 flags);
void dbRemObjectNode(LIST * objectList, U32 nr);

/* public prototypes */
void dbInit(void);
void dbDone(void);

#endif  // ENGINES_THECLOU_DATA_DATABASE_H
