current: nt


# TARGETS += stk

VERSION = \"0.16\"

.SUFFIXES: .dll .obj
# ----------------------- NT ----------------------------

NTOBJECTS = *.obj
NTDLLS = *.dll

PDNTCFLAGS = /W3 /WX /DNT /DPD /nologo

PDNTINCLUDE = /I. /I..\..\pd\src /I..\include
ProgramFiles = C:\Program Files
PDNTLDIR = "$(ProgramFiles)\Microsoft Visual Studio\Vc98\lib"
#PDNTLDIR = "C:\Programme\Msdev\Vc98\lib"

PDNTLIB = $(PDNTLDIR)\libc.lib \
	$(PDNTLDIR)\oldnames.lib \
	$(PDNTLDIR)\wsock32.lib \
	$(PDNTLDIR)\kernel32.lib \
	$(PDNTLDIR)\uuid.lib \
	..\..\pd\bin\pd.lib 

nt: $(NTOBJECTS) 
	-link /dll  $(PDNTLIB) sfplay~.obj /export:sfplay_tilde_setup
	-link /dll  $(PDNTLIB) atan2~.obj /export:atan2_tilde_setup
	-link /dll  $(PDNTLIB) exp~.obj /export:exp_tilde_setup
	-link /dll  $(PDNTLIB) log~.obj /export:log_tilde_setup
	-link /dll  $(PDNTLIB) mixer~.obj /export:mixer_tilde_setup
	-link /dll  $(PDNTLIB) sfwrite~.obj /export:swrite_tilde_setup
	-link /dll  $(PDNTLIB) streamin~.obj /export:streamin_tilde_setup
	-link /dll  $(PDNTLIB) streamout~.obj /export:streamout_tilde_setup
	-link /dll  $(PDNTLIB) moog~.obj /export:moog_tilde_setup

clean:
	del *.obj 
	del *.dll


.c.obj:
	-cl $(PDNTCFLAGS) $(PDNTINCLUDE) /c $*.c

.obj.dll:


	

