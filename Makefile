.DELETE_ON_ERROR:

ifndef CLASTOOL
    $(error "Please set the variable CLASTOOL")
endif

MKDIR_P := mkdir -p

BINDIR := ./bin
SRCDIR := ./src

ROOTCONFIG := root-config
ROOTCFLAGS := $(shell $(ROOTCONFIG) --cflags)
ROOTLDFLAGS := $(shell $(ROOTCONFIG) --ldflags)
ROOTGLIBS := $(shell $(ROOTCONFIG) --glibs)

CXX := c++
LD := c++

ifeq ($(shell echo $${HOSTNAME}),)
	CXXFLAGS := -g -O0 -Wall -fPIC $(ROOTCFLAGS) # O0
	LDFLAGS := -g -O0 $(ROOTLDFLAGS) # O0
else ifeq ($(shell echo $${HOSTNAME}),ui01.hpc.utfsm.cl)
	CXXFLAGS := -g -O2 -Wall -fPIC $(ROOTCFLAGS)
	LDFLAGS := -g -O2 $(ROOTLDFLAGS)
else ifeq ($(shell echo $${HOSTNAME}),ui02.hpc.utfsm.cl)
	CXXFLAGS := -g -O2 -Wall -fPIC $(ROOTCFLAGS)
	LDFLAGS := -g -O2 $(ROOTLDFLAGS)
else
	CXXFLAGS := -g -O0 -Wall -fPIC $(ROOTCFLAGS) # O0
	LDFLAGS := -g -O0 $(ROOTLDFLAGS) # O0
endif

INCLUDES := -I$(ANALYSER)/include\
               -I$(CLASTOOL)/include -I$(PRODIR)/include
LIBS := $(ROOTGLIBS) -lRooFit -lRooFitCore -lMinuit\
               -L$(CLASTOOL)/slib/${OS_NAME} -lClasTool -lClasBanks -lVirtualReader -lDSTReader -lMathMore -lMatrix\
               -L$(ANALYSER)/slib/ -lTIdentificator -lSpectrum -lEG

PROG := MakePlots GetQuantiles CheckGSIMCards SystPi0Mass SystPi0Compare MakeMR-sim MakePlots-DvsR DiffractiveProcesses\
	MakeRooFits MakeMR-nbs MakePlots-2D PlotParams MakeMR-bs MakeSimFits SystPi0Fits SystPi0MR SystPi0Diff MakeMR-sn GetElectronNumber\
	GetSimpleTuple MakePlots-DvsD FilterNCombine DalitzPlots GetEvents Acceptance # Test MakePlots_old
LIST := $(addprefix $(BINDIR)/, $(PROG))

.PHONY: all clean

all: $(LIST)

$(BINDIR)/%: $(BINDIR)/%.o
	@echo "Doing application" $@ 
	$(LD) $(LDFLAGS) $(LIBS) -o $@ $^

$(BINDIR)/%.o: $(SRCDIR)/%.cxx
	$(MKDIR_P) $(BINDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(LIST:%=%.o) $(LIST)
