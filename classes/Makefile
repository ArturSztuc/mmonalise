CXX=g++
AR=ar
LD_SHARED=g++

ROOTLIBS := $(shell root-config --glibs --libs) 
ROOTINCLUDES := $(shell root-config --prefix)/include 
ROOTCFLAGS := $(shell root-config --cflags) -I../

INCDIR = -I. 
LIBDIR = ../lib

UNAME := $(shell uname)

ifneq ($(UNAME), Darwin)
EXPORT_DYNAMIC="-Wl,-export-dynamic"
endif

CXXFLAGS   = -Wall -O3 -g -fPIC $(ROOTCFLAGS) $(INCDIR) -Werror
SYSLIB     = -lm -lg2c
LINK_ARGS_BIN = $(SYSLIB) $(LIBDIR)
SOFLAGS= -shared $(EXPORT_DYNAMIC) $(ROOTCFLAGS)
LDFLAGS=
ARFLAGS= rcsv

CXXFLAGS += -fopenmp
OMP_DEFINES = -DMULTITHREAD

OBJECTS_DAT := datareader.o plotter.o
OBJECTS_PLT := plotter.o 

%.o:%.cpp
	g++ -c $(CXXFLAGS) $< $(OMP_DEFINES)

libCovariance: libDatareader.a $(LIBDIR)/libDatareader.so
libPlotter: libPlotter.a $(LIBDIR)/libPlotter.so

libDatareader.a: $(OBJECTS_DAT)
	$(AR) $(ARFLAGS) $@ $^

libPlotter.a: $(OBJECTS_PLT)
	$(AR) $(ARFLAGS) $@ $^

$(LIBDIR)/libDatareader.so: libDatareader.a $(OBJECTS_DAT)
	$(LD_SHARED) $(SOFLAGS) $^ -o $@ $(LIB_OBJECTS) $(ROOTLIBS) -L../lib

$(LIBDIR)/libPlotter.so: libPlotter.a $(OBJECTS_PLT)
	$(LD_SHARED) $(SOFLAGS) $^ -o $@ $(LIB_OBJECTS) $(ROOTLIBS) -L../lib

clean:
	rm -f ../lib/libDatareader.so	
	rm -f ../lib/libPlotter.so	
	rm -f *.o lib*

distclean:
	rm -fv *~ *.o *.so *.a lib*
