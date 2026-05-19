MODULE := engines/theclou

MODULE_OBJS := \
	engine.o \
	metaengine.o \
	platform/tc_debug.o \
	platform/tc_fs.o \
	platform/tc_platform.o \
	anim/sysanim.o \
	base/base.o \
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

# The derclou C sources use paths relative to the engine root
# (e.g. #include "inphdl/inphdl.h"), so we add that directory to CFLAGS
# for every object in this module.
engines/theclou/%.o: CFLAGS += -I$(srcdir)/engines/theclou

# Build the legacy C files with SCUMMVM_ENGINE so that main() is excluded
# and type conflicts with ScummVM headers are avoided.
engines/theclou/base/base.o: CFLAGS += -DSCUMMVM_ENGINE
engines/theclou/gfx/gfx.o:  CFLAGS += -DSCUMMVM_ENGINE
engines/theclou/inphdl/inphdl.o: CFLAGS += -DSCUMMVM_ENGINE
engines/theclou/sound/fx.o:  CFLAGS += -DSCUMMVM_ENGINE
engines/theclou/sound/newsound.o: CFLAGS += -DSCUMMVM_ENGINE
engines/theclou/sound/buffer.o:   CFLAGS += -DSCUMMVM_ENGINE
engines/theclou/cdrom/cdrom.o:    CFLAGS += -DSCUMMVM_ENGINE

# This module can be built as a plugin
ifeq ($(ENABLE_THECLOU), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk

# Detection objects
DETECT_OBJS += $(MODULE)/detection.o
