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

#include "engines/advancedDetector.h"
#include "base/plugins.h"

static const PlainGameDescriptor theClouGames[] = {
	{ "theclou", "The Clou! (Der Clou!)" },
	{ nullptr, nullptr }
};

namespace TheClou {

// Detection requires BOTH data/tcmain.dat AND data/tcstory.pc to be present.
// The .pc extension on tcstory.pc is unique to Der Clou! and prevents false
// positives from other engines (e.g. Level9) that also have .dat files.
static const ADGameDescription gameDescriptions[] = {
	// German version (Der Clou! v1.0) — files ship ALL-CAPS on the original DOS distribution.
	// kADFlagMatchFullPaths is set on the MetaEngine so subdirectory paths work.
	{
		"theclou",
		"German",
		AD_ENTRY2s("DATA/TCMAIN.DAT", "d1e8794f38161bd6a822d4b344433569", 10999,
		           "DATA/TCSTORY.PC", "54d219699d7afe637e24d2e1a1a8435f", 12559),
		Common::DE_DEU,
		Common::kPlatformDOS,
		ADGF_NO_FLAGS,
		GUIO1(GUIO_NOMIDI)
	},
	// English version (The Clou! v1.0)
	{
		"theclou",
		"English",
		AD_ENTRY2s("DATA/TCMAIN.DAT", "de4fa6c700d6e45233dea3a95b6090e0", 8324,
		           "DATA/TCSTORY.PC", "c1455134cd5cc2604bd61ef5de7cf3be", 11132),
		Common::EN_ANY,
		Common::kPlatformDOS,
		ADGF_NO_FLAGS,
		GUIO1(GUIO_NOMIDI)
	},
	AD_TABLE_END_MARKER
};

} // End of namespace TheClou

// directoryGlobs tells the AdvancedDetector which subdirectories to scan
// when building the file map for detection.  "data" MUST be listed so that
// the scanner recurses into DATA/ and finds TCMAIN.DAT / TCSTORY.PC.
// The user must select the game's ROOT directory (the folder that contains
// DATA/, DATADISK/, TEXTS/ etc.), not the DATA subdirectory itself.
static const char * const directoryGlobs[] = {
	"data",
	"datadisk",
	"texts",
	"pictures",
	"sounds",
	nullptr
};

class TheClouMetaEngineDetection : public AdvancedMetaEngineDetection<ADGameDescription> {
public:
	TheClouMetaEngineDetection() : AdvancedMetaEngineDetection(TheClou::gameDescriptions, theClouGames) {
		_flags = kADFlagMatchFullPaths;
		_maxScanDepth = 2;
		_directoryGlobs = directoryGlobs;
	}

	const char *getName() const override {
		return "theclou";
	}

	const char *getEngineName() const override {
		return "The Clou! (Der Clou!)";
	}

	const char *getOriginalCopyright() const override {
		return "(C) 1993-1994 neo Software Produktions GmbH";
	}
};

REGISTER_PLUGIN_STATIC(THECLOU_DETECTION, PLUGIN_TYPE_ENGINE_DETECTION, TheClouMetaEngineDetection);
