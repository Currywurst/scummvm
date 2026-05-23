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

#ifndef ENGINES_THECLOU_ENGINE_H
#define ENGINES_THECLOU_ENGINE_H

#include "engines/engine.h"
#include "common/error.h"
#include "common/scummsys.h"

struct ADGameDescription;

// Forward-declare subsystems so the header stays lean.
namespace TheClou {
class DiskManager;
}

namespace TheClou {

/**
 * TheClouEngine — the ScummVM engine class for Der Clou! / The Clou!
 *
 * Ownership model (ScummVM convention):
 *   Subsystem objects are heap-allocated in the constructor and deleted in
 *   the destructor.  Every subsystem pointer starts as nullptr so the
 *   destructor can safely delete even on a partially-constructed engine.
 *
 * Global singletons (e.g. g_diskManager in disk/DiskManager.h) are set to
 * point at the respective member in run() and cleared on destruction.
 * This ensures C game code that accesses subsystems via the global still works.
 */
class TheClouEngine : public Engine {
public:
	TheClouEngine(OSystem *syst, const ADGameDescription *gd);
	~TheClouEngine() override;

	Common::Error run() override;
	bool hasFeature(EngineFeature f) const override;

	const ADGameDescription *_gameDescription;

	// ----------------------------------------------------------------
	// Subsystem accessors
	// ----------------------------------------------------------------

	/** Returns the disk/path manager.  Never nullptr after construction. */
	DiskManager *getDiskManager() { return _diskManager; }

protected:
	void pauseEngineIntern(bool pause) override;

private:
	// ----------------------------------------------------------------
	// Owned subsystems
	// ----------------------------------------------------------------

	DiskManager *_diskManager; ///< Path management and file I/O
};

} // End of namespace TheClou

#endif // ENGINES_THECLOU_ENGINE_H
