.DELETE_ON_ERROR:

ifndef CLASTOOL
    $(error "Please set the variable CLASTOOL")
endif

ROOTCONFIG := root-config
ROOTCFLAGS := $(shell $(ROOTCONFIG) --cflags)
ROOTLDFLAGS := $(shell $(ROOTCONFIG) --ldflags)
ROOTGLIBS := $(shell $(ROOTCONFIG) --glibs)

CXX := c++
CXXFLAGS := -O2 -Wall -fPIC $(ROOTCFLAGS)
#CXXFLAGS := -g -O0 -Wall -fPIC $(ROOTCFLAGS) # debug info
LD := c++
LDFLAGS := -O2 $(ROOTLDFLAGS)
#LDFLAGS := -g -O0 $(ROOTLDFLAGS) # debug info


INCLUDES := -I$(ANALYSER)/include\
               -I$(CLASTOOL)/include
LIBS := $(ROOTGLIBS) \
               -L$(CLASTOOL)/slib/${OS_NAME} -lClasTool -lClasBanks -lVirtualReader -lDSTReader -lMathMore -lMatrix\
               -L$(ANALYSER)/slib/ -lTIdentificator -lSpectrum -lEG

FILES := GetSimpleTuple

.PHONY: all clean

all: $(FILES)

%: %.o
	@echo "Doing application " $@ 
	$(LD) $(LDFLAGS) $(LIBS) -o $@ $^

%.o: %.cxx
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(FILES:%=%.o) $(FILES)
