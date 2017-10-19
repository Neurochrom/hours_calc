BINDIR  := .
SRCDIR  := .

DBGOBJDIR  := obj-debug
RELOBJDIR  := obj-release
DBGAPPNAME := hours_calc-debug
RELAPPNAME := hours_calc

$(shell mkdir -p $(DBGOBJDIR))
$(shell mkdir -p $(RELOBJDIR))
$(shell mkdir -p $(BINDIR))

SOURCES := $(wildcard $(SRCDIR)/*.cpp)
DBGOBJECTS := $(patsubst $(SRCDIR)/%.cpp,$(DBGOBJDIR)/%.o, $(SOURCES))
RELOBJECTS := $(patsubst $(SRCDIR)/%.cpp,$(RELOBJDIR)/%.o, $(SOURCES))
DBGDEPS := $(patsubst $(SRCDIR)/%.cpp,$(DBGOBJDIR)/%.d, $(SOURCES))
RELDEPS := $(patsubst $(SRCDIR)/%.cpp,$(RELOBJDIR)/%.d, $(SOURCES))

INCLUDE :=
LIBPATH :=
LIBS    :=

DBGCXXFLAGS := -Wall -ggdb --std=c++11 -D_DEBUG
RELCXXFLAGS := -Wall -o2 --std=c++11
CXX         := g++
DBGLDFLAGS  := -Wall -ggdb --std=c++11 -D_DEBUG
RELLDFLAGS  := -Wall -o2 --std=c++11
LD          := g++
DBGDEPFLAGS := -D_DEBUG -MM -MQ
RELDEPFLAGS := -MM -MQ

all: debug release

debug: $(BINDIR)/$(DBGAPPNAME)

release: $(BINDIR)/$(RELAPPNAME)

$(BINDIR)/$(DBGAPPNAME): $(DBGOBJECTS) $(DBGDEPS)
	$(LD) $(DBGLDFLAGS) $(INCLUDE) $(DBGOBJECTS) -o $(BINDIR)/$(DBGAPPNAME) $(LIBPATH) $(LIBS)

$(BINDIR)/$(RELAPPNAME): $(RELOBJECTS) $(RELDEPS)
	$(LD) $(RELLDFLAGS) $(INCLUDE) $(RELOBJECTS) -o $(BINDIR)/$(RELAPPNAME) $(LIBPATH) $(LIBS)

include $(wildcard $(patsubst %,$(DBGOBJDIR)/%.d,$(basename $(SOURCES))))
include $(wildcard $(patsubst %,$(RELOBJDIR)/%.d,$(basename $(SOURCES))))

$(DBGOBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(DBGCXXFLAGS) $(INCLUDE) -c $< -o $@

$(DBGOBJDIR)/%.d: $(SRCDIR)/%.cpp
	$(CXX) $(DBGDEPFLAGS) $(subst .d,.o,$@) $(INCLUDE) $< -o $@

$(RELOBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(RELCXXFLAGS) $(INCLUDE) -c $< -o $@

$(RELOBJDIR)/%.d: $(SRCDIR)/%.cpp
	$(CXX) $(RELDEPFLAGS) $(subst .d,.o,$@) $(INCLUDE) $< -o $@

clean: clean_debug clean_release
sterile: sterile_debug sterile_release

clean_debug:
	rm -rf $(DBGOBJDIR)/*.o
	rm -rf $(DBGOBJDIR)/*.d
	rmdir $(DBGOBJDIR)

clean_release:
	rm -rf $(RELOBJDIR)/*.o
	rm -rf $(RELOBJDIR)/*.d
	rmdir $(RELOBJDIR)

sterile_debug: clean_debug
	rm -f $(BINDIR)/$(DBGAPPNAME)
	#rmdir --ignore-fail-on-non-empty $(BINDIR)  # while BINDIR is . it makes no sense

sterile_release: clean_release
	rm -f $(BINDIR)/$(RELAPPNAME)
	#rmdir --ignore-fail-on-non-empty $(BINDIR)  # while BINDIR is . it makes no sense
