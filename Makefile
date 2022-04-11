
GCC 		= gcc
CXX 		= g++
CXXFLAGS 	= -Wall -Werror -std=c++11 -DBOOST_LOG_DYN_LINK
LDFLAGS		= -lboost_log -lboost_thread -lpthread
SRCDIR		= .
BUILDDIR	= ../build
BINDIR		= $(BUILDDIR)
OBJECTDIR 	= $(BUILDDIR)/objs
INSTALLDIR	= ../bin
TARGET		= ProxyServer
INCLUDE		= -I../include 
#SRC			= $(wildcard *.cc *.c *.cpp)
#OBJECT		= $(patsubst %.cpp, %.o, $(patsubst %.c, %.o, $(patsubst %.cc, %.o, $(SRC)))) #$(SRC:%.cpp %.cc=%.o)
SRC 		=  $(shell find $(SRCDIR) -name '*.cpp' -printf '%T@ %p\n' | sort -k 1nr | cut -d ' ' -f 2)
SRC 		+= $(shell find $(SRCDIR) -name '*.c'   -printf '%T@ %p\n' | sort -k 1nr | cut -d ' ' -f 2)
SRC 		+= $(shell find $(SRCDIR) -name '*.cc'  -printf '%T@ %p\n' | sort -k 1nr | cut -d ' ' -f 2)
OBJECT 		:= $(SRC:$(SRCDIR)/%.cpp=$(OBJECTDIR)/%.o)
OBJECT 		:= $(OBJECT:$(SRCDIR)/%.c=$(OBJECTDIR)/%.o)
OBJECT 		:= $(OBJECT:$(SRCDIR)/%.cc=$(OBJECTDIR)/%.o)

$(OBJECTDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

$(OBJECTDIR)/%.o: $(SRCDIR)/%.c
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@ $(LDFLAGS)

$(OBJECTDIR)/%.o: $(SRCDIR)/%.cc
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@ $(LDFLAGS)

$(BINDIR)/$(TARGET) : $(OBJECT)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $(BINDIR)/$(TARGET) $^ $(LDFLAGS)

.PHONY: echo all build debug release install clean

echo:
	@echo $(SRC)
	@echo
	@echo $(OBJECT)

all: build $(BINDIR)/$(TARGET)

build:
	mkdir -p $(BUILDDIR)
	mkdir -p $(BINDIR)
	mkdir -p $(OBJECTDIR)
	mkdir -p $(INSTALLDIR)
	mkdir -p $(dir $(OBJECT))

debug: CXXFLAGS += -DDEBUG -g
debug: all 

release: CXXFLAGS += -o2
release: all 

install:
	ln $(BUILDDIR)/$(TARGET) -fs $(INSTALLDIR)/$(TARGET)

clean:
	rm -rf $(OBJECTDIR)/*
	rm -f $(BINDIR)/$(TARGET)
