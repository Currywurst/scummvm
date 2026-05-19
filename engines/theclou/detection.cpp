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

// Detection requires BOTH TCMAIN.DAT and TCSTORY.PC to be present.
// TCSTORY.PC is unique to Der Clou! (the .pc extension prevents false positives
// from other engines that also use .dat files).
//
// The files live in the DATA/ subdirectory of the game root.  We use
// directoryGlobs + _maxScanDepth=2 WITHOUT kADFlagMatchFullPaths so the
// AdvancedDetector recurses into DATA/ and indexes the files by name only.
// This is the standard ScummVM pattern (cf. BladeRunner, Sludge, …) and is
// more robust than full-path matching across different platforms/filesystems.
static const ADGameDescription gameDescriptions[] = {
	// German version (Der Clou! v1.0)
	{
		"theclou",
		"German",
		AD_ENTRY2s("TCMAIN.DAT", "0f825d65672e578bd3b021dec974f199", 10999,
		           "TCSTORY.PC", "b7f03493d6e3706a89206110f2edff25", 12559),
		Common::DE_DEU,
		Common::kPlatformDOS,
		ADGF_NO_FLAGS,
		GUIO1(GUIO_NOMIDI)
	},
	// English version (The Clou! v1.0)
	{
		"theclou",
		"English",
		AD_ENTRY2s("TCMAIN.DAT", "92839ac9027c80d1e6561cd6863eed73", 8324,
		           "TCSTORY.PC", "ce8419fbf5c69634c582e57d1ae281f6", 11132),
		Common::EN_ANY,
		Common::kPlatformDOS,
		ADGF_NO_FLAGS,
		GUIO1(GUIO_NOMIDI)
	},
	AD_TABLE_END_MARKER
};

} // End of namespace TheClou

// directoryGlobs: scanner recurses into DATA/ (and the other dirs) so it
// finds TCMAIN.DAT and TCSTORY.PC.  The user must select the game's ROOT
// directory (the folder that contains DATA/, DATADISK/, TEXTS/, etc.),
// NOT the DATA subdirectory itself.
// Directory names must match the on-disk capitalisation exactly because the
// glob map lookup is case-sensitive even on macOS.
static const char * const directoryGlobs[] = {
	"DATA",
	"DATADISK",
	"TEXTS",
	"PICTURES",
	"SOUNDS",
	nullptr
};

class TheClouMetaEngineDetection : public AdvancedMetaEngineDetection<ADGameDescription> {
public:
	TheClouMetaEngineDetection() : AdvancedMetaEngineDetection(TheClou::gameDescriptions, theClouGames) {
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
