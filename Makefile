# Makefile for ggee

lib.name = ggee

class.sources = control/constant.c \
            control/getdir.c \
            control/inv.c \
            control/qread.c \
            control/rl.c \
            control/rtout.c \
            control/serial_bird.c \
            control/serialize.c \
            control/serial_ms.c \
            control/serial_mt.c \
            control/shell.c \
            control/sinh.c \
            control/sl.c \
            control/stripdir.c \
            control/unserialize.c \
            control/unwonk.c \
            experimental/fofsynth~.c \
            experimental/pvocfreq.c \
            experimental/tabwrite4~.c \
            filters/bandpass.c \
            filters/equalizer.c \
            filters/highpass.c \
            filters/highshelf.c \
            filters/hlshelf.c \
            filters/lowpass.c \
            filters/lowshelf.c \
            filters/moog~.c \
            filters/notch.c \
            gui/button.c \
            gui/envgen.c \
            gui/fatom.c \
            gui/gcanvas.c \
            gui/image.c \
            gui/slider.c \
            gui/sliderh.c \
            gui/state.c \
            gui/ticker.c \
            gui/toddle.c \
            other/messages.c \
            other/vbap.c \
            signal/atan2~.c \
            signal/mixer~.c \
            signal/sfwrite~.c \
            signal/streamin~.c \
            signal/streamout~.c

datafiles = control/constant-help.pd \
            control/cvs.pd \
            control/getdir-help.pd \
            control/inv-help.pd \
            control/qread-help.pd \
            control/rl-help.pd \
            control/rtout-help.pd \
            control/serial_bird-help.pd \
            control/serialctl-help.pd \
            control/serialctl.pd \
            control/serialize-help.pd \
            control/serial_ms-help.pd \
            control/serial_mt-help.pd \
            control/shell-help.pd \
            control/sinh-help.pd \
            control/sl-help.pd \
            control/stripdir-help.pd \
            control/unserialize-help.pd \
            control/unwonk-help.pd \
            experimental/fofsynth~-help.pd \
            experimental/pvocfreq-help.pd \
            experimental/tabwrite4~-help.pd \
            filters/bandpass-help.pd \
            filters/equalizer-help.pd \
            filters/filtgain.pd \
            filters/highpass-help.pd \
            filters/highshelf-help.pd \
            filters/hlshelf-help.pd \
            filters/lowpass-help.pd \
            filters/lowshelf-help.pd \
            filters/moog~-help.pd \
            filters/notch-help.pd \
            gui/button-help.pd \
            gui/envgen-help.pd \
            gui/gcanvas-help.pd \
            gui/image-help.pd \
            gui/slider-help.pd \
            gui/sliderh-help.pd \
            gui/state-help.pd \
            gui/ticker-help.pd \
            gui/toddle-help.pd \
            other/vbap-help.pd \
            signal/atan2~-help.pd \
            signal/mixer~-help.pd \
            signal/sfwrite~-help.pd \
            signal/streamin~-help.pd \
            signal/streamout~-help.pd \
            ggee-meta.pd \
            LICENSE.txt \
            README.txt

# This Makefile is based on the Makefile from pd-lib-builder written by
# Katja Vetter. You can get it from:
# https://github.com/pure-data/pd-lib-builder

include ../../pd-lib-builder/Makefile.pdlibbuilder
