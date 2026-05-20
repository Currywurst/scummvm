MODULE := engines/theclou

MODULE_OBJS := \
	engine.o \
	metaengine.o \
	platform/tc_debug.o \
	platform/tc_fs.o \
	platform/tc_platform.o \
	platform/tc_quit.o \
	anim/sysanim.o \
	base/base.o \
	base/theclou_run.o \
	cdrom/cdrom.o \
	data/dataappl.o \
	data/database.o \
	data/datacalc.o \
	data/relation.o \
	dialog/dialog.o \
	dialog/talkappl.o \
	disk/disk.o \
	error/error.o \
	gameplay/gp.o \
	gameplay/gp_app.o \
	gameplay/loadsave.o \
	gameplay/tcreques.o \
	gfx/gfx.o \
	gfx/gfxnch4.o \
	inphdl/inphdl.o \
	landscap/access.o \
	landscap/hardware.o \
	landscap/init.o \
	landscap/landscap.o \
	landscap/raster.o \
	landscap/scroll.o \
	landscap/spot.o \
	list/list.o \
	living/bob.o \
	living/living.o \
	memory/memory.o \
	organisa/organisa.o \
	planing/graphics.o \
	planing/guards.o \
	planing/io.o \
	planing/main.o \
	planing/planer.o \
	planing/player.o \
	planing/prepare.o \
	planing/support.o \
	planing/sync.o \
	planing/system.o \
	present/interac.o \
	present/present.o \
	random/random.o \
	scenes/cars.o \
	scenes/dealer.o \
	scenes/done.o \
	scenes/evidence.o \
	scenes/inside.o \
	scenes/invest.o \
	scenes/scenes.o \
	scenes/tools.o \
	sound/buffer.o \
	sound/fmopl.o \
	sound/fx.o \
	sound/hsc.o \
	sound/newsound.o \
	story/story.o \
	text/text.o

# The derclou sources use paths relative to the engine root
# (e.g. #include "inphdl/inphdl.h").  CPPFLAGS is passed to both
# the C compiler (%.c) and the C++ compiler (%.cpp) by ScummVM's
# Makefile.common, so this covers all translation units in the engine.
engines/theclou/%.o: CPPFLAGS += -I$(srcdir)/engines/theclou

# Files that need SCUMMVM_ENGINE to exclude main() / resolve type conflicts.
# Use CXXFLAGS since all source files are now .cpp.
engines/theclou/base/base.o:      CXXFLAGS += -DSCUMMVM_ENGINE
engines/theclou/gfx/gfx.o:       CXXFLAGS += -DSCUMMVM_ENGINE
engines/theclou/inphdl/inphdl.o: CXXFLAGS += -DSCUMMVM_ENGINE
engines/theclou/sound/fx.o:      CXXFLAGS += -DSCUMMVM_ENGINE
engines/theclou/sound/newsound.o: CXXFLAGS += -DSCUMMVM_ENGINE
engines/theclou/sound/buffer.o:  CXXFLAGS += -DSCUMMVM_ENGINE
engines/theclou/cdrom/cdrom.o:   CXXFLAGS += -DSCUMMVM_ENGINE

# This module can be built as a plugin
ifeq ($(ENABLE_THECLOU), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk

# Detection objects
DETECT_OBJS += $(MODULE)/detection.o
