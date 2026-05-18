/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
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
 *
 */

#include "theclou/engine.h"

#include "engines/advancedDetector.h"
#include "base/plugins.h"
#include "common/config-manager.h"
#include "common/fs.h"
#include "common/savefile.h"
#include "common/str.h"
#include "common/system.h"
#include "common/translation.h"

// The game stores save-slot names in datadisk/games.lst — one name per line.
// Slot index = line number (0-based).  Empty or unchanged slots keep their
// original "origin.lst" name, meaning "not saved yet".
#define TC_MAX_SAVE_SLOTS 10
#define TC_DATADISK_DIR   "datadisk"
#define TC_GAMES_LST      "games.lst"
#define TC_ORIGIN_LST     "origin.lst"

class TheClouMetaEngine : public AdvancedMetaEngine<ADGameDescription> {
public:
	const char *getName() const override {
		return "theclou";
	}

	Common::Error createInstance(OSystem *syst, Engine **engine,
	                             const ADGameDescription *desc) const override {
		*engine = new TheClou::TheClouEngine(syst, desc);
		return Common::kNoError;
	}

	// ------------------------------------------------------------------
	// Save-game listing: read datadisk/games.lst and datadisk/origin.lst
	// from the save path.  A slot is "used" when its games.lst entry
	// differs from the matching origin.lst entry (i.e. the player saved).
	// ------------------------------------------------------------------
	SaveStateList listSaves(const char *target) const override {
		SaveStateList list;

		Common::String savePath = ConfMan.get("savepath", target);
		if (savePath.empty())
			savePath = ConfMan.get("path", target);
		if (savePath.empty())
			return list;

		// Build path: savePath/datadisk/games.lst
		Common::FSNode saveDir(Common::Path(savePath, Common::Path::kNativeSeparator));
		Common::FSNode datadisk = saveDir.getChild(TC_DATADISK_DIR);
		Common::FSNode gamesLst = datadisk.getChild(TC_GAMES_LST);
		Common::FSNode originLst = datadisk.getChild(TC_ORIGIN_LST);

		if (!gamesLst.exists() || !originLst.exists())
			return list;

		Common::SeekableReadStream *gStream = gamesLst.createReadStream();
		Common::SeekableReadStream *oStream = originLst.createReadStream();
		if (!gStream || !oStream) {
			delete gStream;
			delete oStream;
			return list;
		}

		int slot = 0;
		while (!gStream->eos() && !oStream->eos() &&
		       slot < TC_MAX_SAVE_SLOTS) {
			Common::String gLine = gStream->readLine();
			Common::String oLine = oStream->readLine();
			gLine.trim();
			oLine.trim();
			// Slot is used when the player has overwritten the origin name
			if (!gLine.empty() && gLine != oLine) {
				list.push_back(SaveStateDescriptor(this, slot, gLine));
			}
			slot++;
		}

		delete gStream;
		delete oStream;
		return list;
	}

	int getMaximumSaveSlot() const override {
		return TC_MAX_SAVE_SLOTS - 1;
	}

	bool hasFeature(MetaEngineFeature f) const override {
		return (f == kSupportsListSaves);
	}
};

#if PLUGIN_ENABLED_DYNAMIC(THECLOU)
	REGISTER_PLUGIN_DYNAMIC(THECLOU, PLUGIN_TYPE_ENGINE, TheClouMetaEngine);
#else
	REGISTER_PLUGIN_STATIC(THECLOU, PLUGIN_TYPE_ENGINE, TheClouMetaEngine);
#endif
