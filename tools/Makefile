all: define_loudspeakers

CFLAGS = -O2 -g  -I tich
LFLAGS = -Ltich

vbap.o: vbap.c
	cc -c $(CFLAGS) vbap.c


define_loudspeakers: define_loudspeakers.o define_louds_routines.o
	cc define_loudspeakers.o define_louds_routines.o -o define_loudspeakers -lm
define_loudspeakers.o: define_loudspeakers.c
	cc -c define_loudspeakers.c
define_loudsp_routines.o: define_louds_routines.c
	cc -c -O2  define_louds_routines.c

clean:
	rm *.o
	rm define_loudspeakers
