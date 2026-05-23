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

#include "memory/memory.h"

#include "sound/buffer.h"

struct SndBuffer {
    unsigned char *data;	/* data */
    unsigned size;		/* size of data buffer */

    unsigned insertPos;		/* insert at (insertPos % size) */
    unsigned removePos;		/* remove from (removePos % size) */
};


SND_BUFFER *sndCreateBuffer(unsigned size)
{
    SND_BUFFER *buffer;
    unsigned char *data;

    buffer = (SND_BUFFER *)TCAllocMem(sizeof(*buffer), false);
    data = (unsigned char *)TCAllocMem(size, true);

    buffer->data = data;
    buffer->size = size;

    sndResetBuffer(buffer);

    return buffer;
}

void sndResetBuffer(SND_BUFFER *buffer)
{
    buffer->insertPos = 0;
    buffer->removePos = 0;
}

void sndFreeBuffer(SND_BUFFER *buffer)
{
    TCFreeMem(buffer->data, buffer->size);
    TCFreeMem(buffer, sizeof(*buffer));
}

unsigned sndLenBuffer(SND_BUFFER *buffer)
{
    return buffer->insertPos - buffer->removePos;
}

unsigned sndInsertBuffer(SND_BUFFER *buffer, const void *src, unsigned srcLen)
{
    const unsigned char *psrc = (const unsigned char *)src;
    unsigned len, pos;

    srcLen = min(srcLen, buffer->size - sndLenBuffer(buffer));

    pos = buffer->insertPos % buffer->size;
    len = min(srcLen, buffer->size - pos);

    /* insert to the end */
    memcpy(buffer->data + pos, psrc, len);

    /* insert to the start */
    memcpy(buffer->data, psrc + len, srcLen - len);

    buffer->insertPos += srcLen;

    return srcLen;
}

unsigned sndRemoveBuffer(SND_BUFFER *buffer, void *dst, unsigned dstLen)
{
    unsigned char *pdst = (unsigned char *)dst;
    unsigned len, pos;

    dstLen = min(dstLen, sndLenBuffer(buffer));

    pos = buffer->removePos % buffer->size;
    len = min(dstLen, buffer->size - pos);

    /* remove from the end */
    memcpy(pdst, buffer->data + pos, len);

    /* remove from the start */
    memcpy(pdst + len, buffer->data, dstLen - len);

    buffer->removePos += dstLen;

    return dstLen;
}

