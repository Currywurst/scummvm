/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "common/system.h"

#include "engines/advancedDetector.h"

#include "clue/clue.h"

namespace Clue {

} // End of Namespace Clou

class ClueMetaEngine : public AdvancedMetaEngine {
public:
	const char *getName() const override {
		return "clue";
	}

	bool hasFeature(MetaEngineFeature f) const override;
	Common::Error createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const override;
};

bool ClueMetaEngine::hasFeature(MetaEngineFeature f) const {
	return false;
}

Common::Error ClueMetaEngine::createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const {
	*engine = new Clue::ClueEngine(syst, desc);
}

#if PLUGIN_ENABLED_DYNAMIC(CLUE)
	REGISTER_PLUGIN_DYNAMIC(CLUE, PLUGIN_TYPE_ENGINE, ClueMetaEngine);
#else
	REGISTER_PLUGIN_STATIC(CLUE, PLUGIN_TYPE_ENGINE, ClueMetaEngine);
#endif

