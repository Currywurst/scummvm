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

#ifndef ENGINES_THECLOU_DATA_RELATION_H
#define ENGINES_THECLOU_DATA_RELATION_H

#include <string.h>
#include <stdlib.h>

#include "theclou.h"

#include "error/error.h"
#include "memory/memory.h"
#include "disk/disk.h"

#define RELATION           U32
#define KEY                void *
#define PARAMETER          U32
#define COMPARSION         U8

#define CMP_NO             ((COMPARSION)0)
#define CMP_EQUAL          ((COMPARSION)(1))
#define CMP_NOT_EQUAL      ((COMPARSION)(1 << 1))
#define CMP_HIGHER         ((COMPARSION)(1 << 2))
#define CMP_LOWER          ((COMPARSION)(1 << 3))

#define REL_FILE_MARK      "RELF"
#define REL_TABLE_MARK     "RTAB"

#define NO_PARAMETER       UINT32_MAX	/* stdint.h */


#define Set(leftKey,id,rightKey)  SetP (leftKey, id, rightKey, NO_PARAMETER)
#define Ask(leftKey,id,rightKey)  AskP (leftKey, id, rightKey, NO_PARAMETER, CMP_NO)


struct relationDef {
    struct relationDef *rd_next;

    RELATION rd_id;
    struct relation *rd_relationsTable;
};

struct relation {
    struct relation *r_next;

    KEY r_leftKey;
    KEY r_rightKey;
    PARAMETER r_parameter;
};


extern int (*CompareKey) (KEY, KEY);
extern KEY(*EncodeKey) (char *);
extern char *(*DecodeKey) (KEY);


RELATION AddRelation(RELATION);
RELATION CloneRelation(RELATION, RELATION);
RELATION RemRelation(RELATION);

RELATION SetP(KEY, RELATION, KEY, PARAMETER);
RELATION UnSet(KEY, RELATION, KEY);
PARAMETER GetP(KEY, RELATION, KEY);
RELATION AskP(KEY, RELATION, KEY, PARAMETER, COMPARSION);
void AskAll(KEY, RELATION, void (*)(void *));
void UnSetAll(KEY, void (*)(KEY));

int SaveRelations(char *, U32, U32, U16 disk_id);
int LoadRelations(char *, U16 disk_id);
void RemRelations(U32, U32);

#endif  // ENGINES_THECLOU_DATA_RELATION_H
