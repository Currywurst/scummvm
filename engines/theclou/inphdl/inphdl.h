/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * Original game code (c) 1993 ...and avoid panic by
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

/** @file inphdl/inphdl.h — Input handling for Der Clou! */

#ifndef ENGINES_THECLOU_INPHDL_INPHDL_H
#define ENGINES_THECLOU_INPHDL_INPHDL_H

#include "theclou.h"
#include "gfx/gfx.h"
#include "anim/sysanim.h"

#define INP_AS_FAST_AS_POSSIBLE		1

#define INP_UP                (1)
#define INP_DOWN              (1<<1)
#define INP_LEFT              (1<<2)
#define INP_RIGHT             (1<<3)
#define INP_ESC               (1<<4)   /* set by default */

/* these two defines have been exchanged by kaweh */
#define INP_LBUTTONP          (1<<5)	/* left button pressed */
#define INP_LBUTTONR          (1<<6)	/* left button released */

#define INP_RBUTTONP          (1<<7)   /* right button pressed */
#define INP_RBUTTONR          (1<<8)   /* right button released */
#define INP_NO_ESC            (1<<10)  /* Esc Taste wird in WaitFor gesperrt */
#define INP_TIME              (1<<11)
#define INP_KEYBOARD          (1<<12)  /* look at IH.keyCode for further information */
#define INP_FUNCTION_KEY      (1<<13)
#define INP_SPACE             (1<<14)
#define INP_MOUSE             (1<<15)

#define INP_BUTTON      (INP_LBUTTONP+INP_RBUTTONP)
#define INP_MOVEMENT    (INP_UP+INP_DOWN+INP_LEFT+INP_RIGHT)
#define INP_ALL_MODES   (INP_UP+INP_DOWN+INP_LEFT+INP_RIGHT+INP_ESC+INP_LBUTTONP+INP_LBUTTONR+INP_RBUTTONP+INP_RBUTTONR+INP_TIME+INP_MOUSE+INP_KEYBOARD+INP_SPACE)

/* global functions */

extern void inpOpenAllInputDevs(void);
extern void inpCloseAllInputDevs(void);

extern void inpMousePtrOn(void);
extern void inpMousePtrOff(void);

extern S32 inpWaitFor(S32 l_Mask);	/* retourniert was passiert ist
					 * (siehe defines) */
extern void inpSetWaitTicks(U32 l_Ticks);

void inpTurnESC(bool us_NewStatus);	    /* 0 means off, 1 means on */
void inpTurnFunctionKey(bool us_NewStatus); /* 0 means off, 1 means on */
void inpTurnMouse(bool us_NewStatus);	    /* 0 means off, 1 means on */

extern void inpDelay(S32 l_Ticks);
extern void inpSetKeyRepeat(unsigned char rate);

extern void inpClearKbBuffer(void);

#endif // ENGINES_THECLOU_INPHDL_INPHDL_H
