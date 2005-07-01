

VERSION=$(shell cat VERSION)

compile:
	make -f ../makefile.sub -C control
	make -f ../makefile.sub -C filters 
	make -f ../makefile.sub -C gui
	make -f ../makefile.sub -C signal
	make -f ../makefile.sub -C experimental

clean:
	rm -r ggee-$(VERSION)
	make -f ../makefile.sub -C control clean
	make -f ../makefile.sub -C filters clean
	make -f ../makefile.sub -C gui clean
	make -f ../makefile.sub -C signal clean
	make -f ../makefile.sub -C experimental clean
					

package:
	-mkdir ggee-$(VERSION)
	-cp `find . -name "*.pd_linux"` ggee-$(VERSION)
	-cp `find . -name "*.pd_darwin"` ggee-$(VERSION)
	-cp `find . -name "*.dll"` ggee-$(VERSION)
	cp `find . -name "*help.pd"` ggee-$(VERSION)
