# 

EXT = pd_linux
DEFS =  -DHAVE_LIBM=1 -DHAVE_LIBPTHREAD=1 -DSTDC_HEADERS=1 -DHAVE_FCNTL_H=1 -DHAVE_SYS_TIME_H=1 -DHAVE_UNISTD_H=1 -DTIME_WITH_SYS_TIME=1 -DHAVE_UNISTD_H=1 -DHAVE_GETPAGESIZE=1 -DHAVE_MMAP=1 -DHAVE_SELECT=1 -DHAVE_SOCKET=1 -DHAVE_STRERROR=1  -DPD_VERSION_MINOR=32
CC = gcc
CXX = c++
LD = ld
AFLAGS = 
LFLAGS = -export_dynamic  -shared
WFLAGS =
IFLAGS = -I./include
INSTALL_PREFIX=/usr

VERSION = \"$(shell cat VERSION)\"
DISTVERSION = $(shell cat VERSION)

.SUFFIXES: .$(EXT)

PDCFLAGS = -g -O2 $(DEFS) $(IFLAGS) $(WFLAGS) $(LFLAGS) $(AFLAGS) -DVERSION=$(VERSION) 
CFLAGS = -g -O2 $(DEFS) $(IFLAGS) $(WFLAGS) -DVERSION=$(VERSION)
CXXFLAGS = $(CFLAGS)

STKPD= 
STK= 
BUILD_STK = 
#LIBS = -lc -lm 
LIBS = -lpthread -lm 
SOURCES = control/constant.c control/inv.c control/prepend.c control/qread.c control/rl.c control/rtout.c control/serial_bird.c control/serial_ms.c control/serial_mt.c control/serialctl.c control/serialize.c control/shell.c control/sinh.c control/sl.c control/stripdir.c control/unserialize.c control/unwonk.c experimental/fasor~.c experimental/fofsynth~.c experimental/pvocfreq.c filters/bandpass.c filters/equalizer.c filters/highpass.c filters/highshelf.c filters/hlshelf.c filters/lowpass.c filters/lowshelf.c filters/notch.c gui/envgen.c gui/hslider.c gui/slider.c gui/state.c gui/ticker.c gui/toddle.c other/messages.c other/vbap.c signal/atan2~.c signal/exp~.c signal/log~.c signal/mixer~.c signal/moog~.c signal/pipewrite~.c signal/rtin~.c signal/sfplay~.c signal/sfread~.c signal/sfwrite~.c signal/streamin~.c signal/streamout~.c tools/define_louds_routines.c tools/define_loudspeakers.c
TARGETS = $(SOURCES:.c=.$(EXT)) $(STKPD)

all:  $(STK) $(TARGETS) 

ggext: $(STK) $(TARGETS)
	cc -c $(CFLAGS) -DPD ggee.c
	$(LD) -export_dynamic  -shared -o ggext.pd_linux ggee.o */*.o $(LIBS)
	strip --strip-unneeded ggext.pd_linux

clean::
	-rm  */*.$(EXT) *.$(EXT) so_locations rm */*~ *~ *.o */*.o

distclean: clean
	-rm config.cache config.log config.status makefile


distcleancvs:
	-rm -r CVS reference/CVS


.c.o:
	$(CC) -c -o $@ $(CFLAGS) -DPD $*.c

# cp $@ $*_stat.o

.o.pd_linux:
	$(CC) -o $@ $(PDCFLAGS) -DPD $*.o


include makefile.stk


install::
	install -d $(INSTALL_PREFIX)/lib/pd/externs
	install -m 644 */*.pd_linux $(INSTALL_PREFIX)/lib/pd/externs
	-install -m 644 ggext.pd_linux $(INSTALL_PREFIX)/lib/pd/externs
	install -m 644 */*.pd $(INSTALL_PREFIX)/lib/pd/doc/5.reference


dist: distclean
	(cd ..;tar czvf ggee-$(DISTVERSION).tgz ggee)

ntdist: 
	(mkdir tmp; cd tmp; mkdir ggee; cd ggee; cp `find ../.. -name "*.pd"` .;\
	 cp `find ../.. -name "*.dll"` .; cd ..; zip ggee$(DISTVERSION).zip ggee)

	 
