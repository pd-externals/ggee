current: nt


# TARGETS += stk

VERSION = \"0.24\"

.SUFFIXES: .dll .obj
# ----------------------- NT ----------------------------



cd control
nmake
cd ..
cd experimental 
nmake
cd ..
cd filters
nmake
cd ..
cd gui
nmake
cd ..
cd other
nmake
cd ..
cd signal
nmake
cd ..


