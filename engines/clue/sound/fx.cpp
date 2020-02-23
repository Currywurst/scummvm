/****************************************************************************
  Copyright (c) 2005 Vasco Alexandre da Silva Costa
  Portions copyright (c) 2005 Jens Granseuer

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/
#include "clue/base/base.h"
#include "clue/clue.h"

#include "common/stream.h"
#include "audio/mixer.h"
#include "audio/audiostream.h"
#include "audio/decoders/raw.h"
#include "audio/decoders/voc.h"

#include "clue/sound/buffer.h"
#include "clue/sound/fx.h"
#include "clue/sound/hsc.h"

namespace Clue {

struct FXBase FXBase;

static bool SfxChannelOn = false;
static bool MusicChannelOn = true;


void InitAudio(void) {
	FXBase.us_AudioOk = 1;
	if (!g_clue->_mixer->isReady()) {
		DebugMsg(ERR_WARNING, ERROR_MODULE_SOUND, "Mixer error");
		FXBase.us_AudioOk = 0;
		return;
	}

	FXBase.pSfxBuffer = sndCreateBuffer(SND_BUFFER_SIZE);
	FXBase.pMusicBuffer = sndCreateBuffer(SND_BUFFER_SIZE);
}

void RemoveAudio(void) {
	g_clue->_mixer->stopAll();
	FXBase.us_AudioOk = 0;
}

Audio::SoundHandle sfx;
Audio::AudioStream *sfxFile;

void sndInitFX(void) {
	SfxChannelOn = false;
	g_clue->_mixer->stopHandle(sfx);
	sfxFile = NULL;
}

void sndDoneFX(void) {
	SfxChannelOn = false;
	g_clue->_mixer->stopHandle(sfx);
	sfxFile = NULL;
}

void sndPrepareFX(const char *name) {
	sndDoneFX();

	if (FXBase.us_AudioOk) {
		// TODO: Use proper Dsk functions
		char fileName[DSK_PATH_MAX];

		dskBuildPathName(DISK_CHECK_FILE, SAMPLES_DIRECTORY, name, fileName);
		Common::Stream *file = dskOpen(fileName, 0);
		Common::SeekableReadStream *stream = dynamic_cast<Common::SeekableReadStream *>(file);
		sfxFile = Audio::makeVOCStream(stream, Audio::FLAG_UNSIGNED, DisposeAfterUse::YES);
	}
}

void sndPlayFX(void) {
	SfxChannelOn = true;
	if (sfxFile) {
		g_clue->_mixer->playStream(Audio::Mixer::kSFXSoundType, &sfx, sfxFile);
	}
}

} // End of namespace Clue
