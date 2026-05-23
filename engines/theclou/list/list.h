/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * Original game code (c) 2000 Oliver Gantert <lucyg@t-online.de>
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
 * @file list/list.h
 * @brief Intrusive doubly-linked list for Der Clou!
 *
 * NODE / LIST are plain POD structs — in C++ no typedef is needed for
 * struct tags.  All existing code that uses NODE* / LIST* continues to
 * compile unchanged.
 *
 * Does NOT include common/scummsys.h so legacy game files can safely
 * include this without triggering common/forbidden.h bans.
 */

#ifndef ENGINES_THECLOU_LIST_LIST_H
#define ENGINES_THECLOU_LIST_LIST_H

#include <stddef.h>   // size_t

#include "theclou.h"

// ---------------------------------------------------------------------------
// Data structures
// ---------------------------------------------------------------------------

/** Intrusive list node — embed as first member in any list element. */
struct NODE {
    NODE   *Succ;   ///< Next node (nullptr for the tail sentinel's successor)
    NODE   *Pred;   ///< Previous node (nullptr for the head sentinel's predecessor)
    char   *Name;   ///< Optional name string (heap-allocated or nullptr)
    size_t  Size;   ///< Size of the enclosing allocation (for TCFreeMem)
};

/** Doubly-linked list with sentinel head/tail nodes. */
struct LIST {
    NODE Head;   ///< Sentinel head: Succ points to first real node
    NODE Tail;   ///< Sentinel tail: Pred points to last real node
};

// ---------------------------------------------------------------------------
// Accessor macros (pointer-cast versions kept for backward compatibility)
// ---------------------------------------------------------------------------

#define NODE_SUCC(node) (reinterpret_cast<NODE *>(node)->Succ)
#define NODE_PRED(node) (reinterpret_cast<NODE *>(node)->Pred)
#define NODE_NAME(node) (reinterpret_cast<NODE *>(node)->Name)
#define NODE_SIZE(node) (reinterpret_cast<NODE *>(node)->Size)

#define INNER_HEAD(list) (&(list)->Head)
#define INNER_TAIL(list) (&(list)->Tail)

#define LIST_HEAD(list)  (NODE_SUCC(INNER_HEAD((list))))
#define LIST_TAIL(list)  (NODE_PRED(INNER_TAIL((list))))

#define LIST_EMPTY(list) (!NODE_SUCC(LIST_HEAD((list))))

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

LIST *CreateList(void);
void  RemoveList(LIST *list);
void  FreeList(LIST *list);

void *AddNode(LIST *list, void *node, void *predNode);
void *AddTailNode(LIST *list, void *node);
void *AddHeadNode(LIST *list, void *node);
void *RemNode(void *node);
void *RemHeadNode(LIST *list);
void *RemTailNode(LIST *list);

void *CreateNode(LIST *list, size_t size, const char *name);
void  RemoveNode(LIST *list, const char *name);
void  FreeNode(void *node);

void *GetNode(LIST *list, const char *name);
void *GetNthNode(LIST *list, U32 nth);
U32   GetNrOfNodes(LIST *list);
U32   GetNodeNrByAddr(LIST *list, void *node);
U32   GetNodeNr(LIST *list, const char *name);

void  foreach(LIST *list, void (*processNode)(void *));

void  Link(LIST *list, void *node, void *predNode);
void *UnLinkByAddr(LIST *list, void *node, NODE **predNode);
void *UnLink(LIST *list, const char *name, NODE **predNode);

void  ReplaceNodeByAddr(LIST *list, void *node, NODE *newNode);
void  ReplaceNode(LIST *list, const char *name, NODE *newNode);

U32   ReadList(LIST *list, size_t size, char *fileName);
void  WriteList(LIST *list, char *fileName);

#endif // ENGINES_THECLOU_LIST_LIST_H
