
EXTERNALS = $(shell ls *.c)
CPPEXTERNALS = $(shell ls *.cpp)

STRIP = strip --strip-unneeded -R .note -R .comment

all: $(EXTERNALS:.c=.pd_linux) $(CPPEXTERNALS:.cpp=.pd_linux)

# this might be a better way to do this:
# EXTERNALS    := ${patsubst %.c, %.o, ${wildcard *.c}} 
# all: ${EXTERNALS}

.SUFFIXES: .pd_linux

STK = $(shell ls -d stk*/)

CFLAGS = -I$(STK)include

# cross-platform default
CPU_FLAGS = -O2
OPTIM_FLAGS = -funroll-loops -fomit-frame-pointer $(CPU_FLAGS)

FLAGS = -fPIC -DPD -DUNIX $(OPTIM_FLAGS)  \
    -Wall -W -Wshadow -Wstrict-prototypes  \
    -Wno-unused -Wno-parentheses -Wno-switch $(CFLAGS)

INCLUDES = -I. -I.. -I../../build/include

%.pd_linux: %.c
	$(CC) $(FLAGS) $(INCLUDES) -o "$*.o" -c "$+"
	gcc -Wl,-export_dynamic  -shared -o "$@" "$*.o" -lc -lm
	chmod a-x "$@"
	$(STRIP) $@
	rm -f "$*.o" 

%.pd_linux: %.cpp
	$(CXX) $(FLAGS) $(INCLUDES) -o "$*.o" -c "$+"
	$(CXX) -Wl,-export_dynamic  -shared -o "$@" "$*.o" -lc -lm $(STK)src/libstk.a
	chmod a-x "$@"
	$(STRIP) $@
	rm -f "$*.o" 

clean:
	-rm *.pd_linux *~ *.o
	-rm link.stamp

