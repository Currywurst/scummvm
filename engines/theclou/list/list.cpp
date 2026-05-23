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

/**
 * @file list/list.cpp
 * @brief Intrusive doubly-linked list implementation for Der Clou!
 *
 * We use Common::strlcpy instead of strcpy.
 * All void* ↔ NODE* casts use static_cast<>.
 */

/* Common::strlcpy is needed for CreateNode's name copy. */
#define FORBIDDEN_SYMBOL_EXCEPTION_strcpy
#include "common/scummsys.h"
#include "common/str.h"

#include "list/list.h"
#include "memory/memory.h"
#include "disk/disk.h"
#include "platform/tc_fs.h"

LIST *CreateList(void)
{
    LIST *list = static_cast<LIST *>(TCAllocMem(sizeof(*list), true));

    if (list) {
        NODE_SUCC(INNER_HEAD(list)) = INNER_TAIL(list);
        NODE_PRED(INNER_HEAD(list)) = nullptr;
        NODE_NAME(INNER_HEAD(list)) = nullptr;

        NODE_SUCC(INNER_TAIL(list)) = nullptr;
        NODE_PRED(INNER_TAIL(list)) = INNER_HEAD(list);
        NODE_NAME(INNER_TAIL(list)) = nullptr;
    }

    return list;
}

void RemoveList(LIST *list)
{
    RemoveNode(list, nullptr);
    FreeList(list);
}

void FreeList(LIST *list)
{
    TCFreeMem(list, sizeof(*list));
}

void *AddNode(LIST *list, void *node, void *predNode)
{
    if (!predNode)
	predNode = INNER_HEAD(list);

    NODE_SUCC(node) = NODE_SUCC(predNode);
    NODE_PRED(node) = static_cast<NODE *>(predNode);

    NODE_PRED(NODE_SUCC(predNode)) = static_cast<NODE *>(node);
    NODE_SUCC(predNode) = static_cast<NODE *>(node);
    return node;
}

void *AddTailNode(LIST *list, void *node)
{
    return AddNode(list, node, LIST_TAIL(list));
}

void *AddHeadNode(LIST *list, void *node)
{
    return AddNode(list, node, INNER_HEAD(list));
}

void *RemNode(void *node)
{
    NODE_SUCC(NODE_PRED(node)) = NODE_SUCC(node);
    NODE_PRED(NODE_SUCC(node)) = NODE_PRED(node);

    /* just to be safe */
    NODE_SUCC(node) = nullptr;
    NODE_PRED(node) = nullptr;

    return node;
}

void *RemHeadNode(LIST *list)
{
    if (!LIST_EMPTY(list))
	return RemNode(LIST_HEAD(list));
    else
	return nullptr;
}

void *RemTailNode(LIST *list)
{
    if (!LIST_EMPTY(list))
	return RemNode(LIST_TAIL(list));
    else
	return nullptr;
}

void *CreateNode(LIST *list, size_t size, const char *name)
{
    NODE *node = nullptr;
    size_t len = 0;

    if (!size)
	size = sizeof(NODE);

    if (name)
	len = strlen(name) + 1;

    if (size >= sizeof(NODE)) {
	if ((node = static_cast<NODE *>(TCAllocMem(size + len, true)))) {
	    NODE_SUCC(node) = nullptr;
	    NODE_PRED(node) = nullptr;
	    NODE_SIZE(node) = size;

	    if (name) {
		char *nameBuf = reinterpret_cast<char *>(node) + size;
		Common::strlcpy(nameBuf, name, len);
		NODE_NAME(node) = nameBuf;
	    } else {
		NODE_NAME(node) = nullptr;
	    }

	    if (list)
		AddTailNode(list, node);
	}
    } else {
	ErrorMsg(Internal_Error, ERROR_MODULE_MEMORY, 666);
    }

    return static_cast<void *>(node);
}

void RemoveNode(LIST *list, const char *name)
{
    NODE *node;

    if (name) {
	if ((node = static_cast<NODE *>(GetNode(list, name)))) {
	    RemNode(node);
	    FreeNode(node);
	}
    } else {
	if (!LIST_EMPTY(list)) {
	    while ((node = static_cast<NODE *>(RemTailNode(list))))
		FreeNode(node);
	}
    }
}

void FreeNode(void *node)
{
    size_t size;

    size = NODE_SIZE(node);

    if (NODE_NAME(node))
	size += strlen(NODE_NAME(node)) + 1;

    TCFreeMem(node, size);
}

void *GetNode(LIST *list, const char *name)
{
    NODE *node;

    for (node = LIST_HEAD(list); NODE_SUCC(node); node = NODE_SUCC(node)) {
	if (strcmp(NODE_NAME(node), name) == 0)
	    return node;
    }

    return nullptr;
}

void *GetNthNode(LIST *list, U32 nth)
{
    NODE *node;

    for (node = LIST_HEAD(list); NODE_SUCC(node); node = NODE_SUCC(node)) {
	if (nth == 0)
	    return node;
	nth--;
    }

    return nullptr;
}

U32 GetNrOfNodes(LIST *list)
{
    NODE *node = nullptr;
    U32 i = 0;

    for (i = 0, node = LIST_HEAD(list); NODE_SUCC(node);
	 i++, node = NODE_SUCC(node));

    return i;
}

U32 GetNodeNrByAddr(LIST *list, void *node)
{
    NODE *s;
    U32 i;

    for (s = LIST_HEAD(list), i = 0; NODE_SUCC(s) && (s != node);
	 s = NODE_SUCC(s), i++);

    return i;
}

U32 GetNodeNr(LIST *list, const char *name)
{
    return GetNodeNrByAddr(list, GetNode(list, name));
}

void foreach(LIST *list, void (*processNode) (void *))
{
    NODE *node;

    for (node = LIST_HEAD(list); NODE_SUCC(node); node = NODE_SUCC(node))
	processNode(node);
}

void Link(LIST *list, void *node, void *predNode)
{
    if (predNode)
	AddNode(list, node, predNode);
    else
	AddTailNode(list, node);
}

void *UnLinkByAddr(LIST *list, void *node, NODE **predNode)
{
    if (!node)
        return nullptr;

    if (predNode)
	*predNode = NODE_PRED(node);

    return RemNode(node);
}

void *UnLink(LIST *list, const char *name, NODE **predNode)
{
    return UnLinkByAddr(list, GetNode(list, name), predNode);
}

void ReplaceNodeByAddr(LIST *list, void *node, NODE *newNode)
{
    NODE *predNode;

    if ((node = UnLinkByAddr(list, node, &predNode))) {
	Link(list, newNode, predNode);
	FreeNode(node);
    }
}

void ReplaceNode(LIST *list, const char *name, NODE *newNode)
{
    ReplaceNodeByAddr(list, GetNode(list, name), newNode);
}

U32 ReadList(LIST *list, size_t size, char *fileName)
{
    TC_FILE *fh;
    U32 i = UINT32_C(0);
    char buffer[256];

    if ((fh = dskOpen(fileName, "rb"))) {
	while (dskGetLine(buffer, sizeof(buffer), fh)) {
	    if (buffer[0] != ';')	/* skip comments */
	    {
		if (!CreateNode(list, size, buffer)) {
		    RemoveNode(list, nullptr);
		    tc_fclose(fh);
		    return 0;
		}

		i++;
	    }
	}

	tc_fclose(fh);
    }
    return i;
}

void WriteList(LIST *list, char *fileName)
{
    TC_FILE *fh = nullptr;
    NODE *node = nullptr;

    if ((fh = dskOpen(fileName, "wb"))) {
	for (node = LIST_HEAD(list); NODE_SUCC(node); node = NODE_SUCC(node))
	    tc_fprintf(fh, "%s\r\n", NODE_NAME(node));

	tc_fclose(fh);
    }
}
