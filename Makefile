.PHONY: all ohboy PocketSNES geekboy clean

CC = clang++

SDL_INCLUDES = $(shell sdl-config --cflags)
SDL_LIBS = $(shell sdl-config --libs)

CCFLAGS = -std=c++11 -stdlib=libc++ $(SDL_INCLUDES)
LDFLAGS += -arch x86_64 $(SDL_LIBS) -lSDL_image

#SOURCES:= $(wildcard *.cpp)
#BINARIES:= $(foreach source, $(SOURCES), $(source:%.cpp=%.o) )
BINDIR := obj
BASESRC := src
SOURCE := $(BASESRC) $(BASESRC)/common $(BASESRC)/core $(BASESRC)/data $(BASESRC)/debug $(BASESRC)/systems $(BASESRC)/ui $(BASESRC)/views
SOURCES := $(patsubst $(BASESRC)/%, %, $(foreach dir, $(SOURCE), $(wildcard $(dir)/*.cpp)))
BINARIES := $(patsubst %.cpp, %.o, $(SOURCES))
EXECUTABLE := gcwemu

CORES:= geekboy
	# ohboy PocketSNES

all: $(EXECUTABLE) $(CORES)

geekboy:
	$(MAKE) -f Makefile.osx_lib -C geekboy
	mv geekboy/geekboy.dylib cores/geekboy.dylib

ohboy:
	$(MAKE) -f Makefile.osx_lib -C ohboy
	mv ohboy/ohboy.dylib cores/ohboy.dylib

PocketSNES:
	$(MAKE) -f Makefile.osx_lib -C PocketSNES
	mv PocketSNES/PocketSNES.dylib cores/pocketsnes.dylib

$(EXECUTABLE): $(addprefix $(BINDIR)/, $(BINARIES))
	$(CC) $(CCFLAGS) $^ -o $@ $(LDFLAGS)

#$(LD) $(LDFLAGS) main.o -o $@ $(LIBS)

clean:
	rm -f gcwemu
	rm -rf $(BINDIR)
	rm -rf cores/*.dylib
	$(foreach var,$(CORES),$(MAKE) -f Makefile.osx_lib -C $(var) clean;)

$(BINDIR)/%.o : $(BASESRC)/%.cpp
	@test -d $(@D) || mkdir -p $(@D)
	$(CC) -DMAKE -c $(CCFLAGS) $< -o $@