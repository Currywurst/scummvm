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

#include "platform/tc_sdl_compat.h"

#include "disk/disk.h"

#include "base/base.h"

#include "sound/fx.h"
#include "sound/newsound.h"

#include "sound/hsc.h"

char currSoundName[DSK_PATH_MAX];


void sndInit(void)
{
    currSoundName[0] = '\0';

    hscInit();
}

void sndDone(void)
{
    sndAudioLock();
    hscDone();
    sndAudioUnlock();
}

void sndPlaySound(const char *name, U32 mode)
{
    char path[DSK_PATH_MAX];

    if (strcmp(currSoundName, name) != 0) {
	strcpy(currSoundName, name);

	if (FXBase.us_AudioOk) {
	    dskBuildPathName(DISK_CHECK_FILE, SOUND_DIRECTORY, name, path);

            sndAudioLock();

	    hscLoad(path);

            sndAudioUnlock();
	}
    }
}

char *sndGetCurrSoundName(void)
{
    return currSoundName;
}


void sndFading(short int targetVol)
{
    static int restoreVolume = -1;
    const int fadeStep = 4;
    const int fadeDelayMs = 10;
    int currentVolume, destVolume, step;

    if (!FXBase.us_AudioOk) {
        return;
    }

    sndAudioLock();
    currentVolume = clamp(setup.MusicVolume, 0, SND_MAX_VOLUME);
    sndAudioUnlock();

    if (restoreVolume < 0) {
        restoreVolume = currentVolume;
    }

    if (targetVol <= 0) {
        destVolume = restoreVolume;
    } else {
        restoreVolume = currentVolume; /* remember level to restore later */
        destVolume = clamp(targetVol, 0, SND_MAX_VOLUME);
    }

    if (destVolume == currentVolume) {
        return;
    }

    step = (destVolume > currentVolume) ? fadeStep : -fadeStep;

    while (currentVolume != destVolume) {
        currentVolume += step;

        if ((step > 0 && currentVolume > destVolume) ||
            (step < 0 && currentVolume < destVolume)) {
            currentVolume = destVolume;
        }

        sndAudioLock();
        setup.MusicVolume = currentVolume;
        sndAudioUnlock();

        SDL_Delay(fadeDelayMs);
    }
}

void sndStopSound(U8 dummy)
{
    if (FXBase.us_AudioOk) {
        sndAudioLock();

        hscReset();

        sndAudioUnlock();
    }
}
