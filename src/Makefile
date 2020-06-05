SRCS = $(wildcard *.cpp)
TRGS = $(patsubst %.cpp, ../bin/%, $(SRCS))
DICT = $(patsubst %.cpp, %Dict.C, $(SRCS))


all : $(TRGS)

OPTS := -O3 -Wall -g -fPIC
#ROOTLIBS := $(shell root-config --glibs --libs ) -lRooFit -lRooFitCore -lHist -lTree -lCore -lMinuit -lgomp -lRooFitCore
ROOTLIBS := $(shell root-config --glibs --libs ) -lHist -lTree -lCore -lgomp
ROOTINCLUDES := $(shell root-config --prefix)/include 
#ROOTCFLAGS := $(shell root-config --cflags) -I../ -I../utils/agf/include -I../ -I/opt/cuda/include
ROOTCFLAGS := $(shell root-config --cflags) -I../

ifdef MULTITHREAD
OPTS += -fopenmp
OMP_DEFINES=-DMULTITHREAD
endif

BEAMLIBS := -L../lib -L/usr/lib64/gcc/x86_64-pc-linux-gnu/4.6.3 -lDatareader

$(TRGS) : $(SRCS)
$(DICT) : $(SRCS)

../bin/% : %.cpp %Dict.C
	g++ $(OPTS) $(ROOTCFLAGS) -I $(ROOTINCLUDES) -o $@ $< $(OMP_DEFINES) $(ROOTLIBS) $(BEAMLIBS) -fpermissive -Wl,-rpath,../lib 

clean:
	rm -fv $(ls ../bin/* | grep -v CVS)	
