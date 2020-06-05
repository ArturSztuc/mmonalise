UNAME := $(shell uname)

# Because Apple computers are special
ifeq ($(UNAME), Darwin)
LIB_EXT="dylib"
else
LIB_EXT="so"
endif

##############################
# ----------------------------
# The default make
# ----------------------------
all:
# Check that bin directory is made
ifeq "$(wildcard bin)" ""
	make links
endif

ifeq "$(wildcard lib)" ""
	make links
endif

	make -j -C classes
	make -j -C src

links:
	@ [ -d lib ]     || mkdir lib
	@ [ -d bin ]     || mkdir bin

# The usual clean
clean:
	make clean -C classes

	rm -rf lib/lib*
	rm -rf bin/*
