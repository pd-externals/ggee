current: nt


# TARGETS += stk

VERSION = \"0.16\"

.SUFFIXES: .dll .obj
# ----------------------- NT ----------------------------



cd control
nmake clean
cd ..
cd experimental 
nmake clean
cd ..
cd filters
nmake clean
cd ..
cd gui
nmake clean
cd ..
cd other
nmake clean
cd ..
cd signal
nmake clean
cd ..


