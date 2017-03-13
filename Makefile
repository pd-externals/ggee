#!/usr/bin/make -f
# Makefile to the 'iemguts' library for Pure Data.
# Needs Makefile.pdlibbuilder as helper makefile for platform-dependent build
# settings and rules (https://github.com/pure-data/pd-lib-builder).

lib.name = ggee

# special file that does not provide a class
lib.setup.sources =

# all other C and C++ files in subdirs are source files per class
# (alternatively, enumerate them by hand)
class.sources = \
    control/constant.c \
    control/rl.c \
    control/serial_ms.c \
    control/sl.c \
    control/getdir.c \
    control/rtout.c \
    control/serial_mt.c \
    control/stripdir.c \
    control/inv.c \
    control/serial_bird.c \
    control/shell.c \
    control/unserialize.c \
    control/qread.c \
    control/serialize.c \
    control/sinh.c \
    control/unwonk.c \
    experimental/fofsynth~.c \
    experimental/tabwrite4~.c \
    experimental/pvocfreq.c \
    filters/bandpass.c \
    filters/highpass.c \
    filters/hlshelf.c \
    filters/lowshelf.c \
    filters/notch.c \
    filters/equalizer.c \
    filters/highshelf.c \
    filters/lowpass.c \
    filters/moog~.c \
    gui/button.c \
    gui/image.c \
    gui/sliderh.c \
    gui/ticker.c \
    gui/envgen.c \
    gui/gcanvas.c \
    gui/slider.c \
    gui/state.c \
    gui/toddle.c \
    signal/atan2~.c \
    signal/mixer~.c \
    signal/sfwrite~.c \
    signal/streamin~.c \
    signal/streamout~.c \
    $(nada)

class.sources += \
    gui/fatom.c

datafiles = \
 $(wildcard */*-help.pd) \
 LICENSE.txt \
 README.txt \
 gui/state.list \
 gui/logo100.gif \
 ggee-meta.pd \
 $(nada)

datadirs =

#cflags = -DVERSION=$(lib.version)

################################################################################
### pdlibbuilder ###############################################################
################################################################################

# This Makefile is based on the Makefile from pd-lib-builder written by
# Katja Vetter. You can get it from:
# https://github.com/pure-data/pd-lib-builder
PDLIBBUILDER_DIR=./
include $(PDLIBBUILDER_DIR)/Makefile.pdlibbuilder
