.PHONY: all geekboy clean gambatte

CC = clang++

SDL_INCLUDES = $(shell sdl-config --cflags)
SDL_LIBS = $(shell sdl-config --libs)

CCFLAGS = -std=c++11 -stdlib=libc++ $(SDL_INCLUDES)
LDFLAGS += -arch x86_64 $(SDL_LIBS) -lSDL_image

#SOURCES:= $(wildcard *.cpp)
#BINARIES:= $(foreach source, $(SOURCES), $(source:%.cpp=%.o) )
BINDIR := obj
BASESRC := src
SOURCE := \
	$(BASESRC) \
	$(BASESRC)/common \
	$(BASESRC)/core \
	$(BASESRC)/data \
	$(BASESRC)/debug \
	$(BASESRC)/systems \
	$(BASESRC)/gfx \
	$(BASESRC)/sfx \
	$(BASESRC)/views
SOURCES := $(patsubst $(BASESRC)/%, %, $(foreach dir, $(SOURCE), $(wildcard $(dir)/*.cpp)))
BINARIES := $(patsubst %.cpp, %.o, $(SOURCES))
EXECUTABLE := gcwemu

CORES:= geekboy gambatte
	# ohboy PocketSNES

all: $(EXECUTABLE) $(CORES)

geekboy:
	$(MAKE) -f Makefile.osx_lib -C geekboy $(MAKECMDGOALS)
	mv geekboy/geekboy.dylib cores/geekboy.dylib
	
gambatte:
	$(MAKE) -f Makefile.osx_lib -C gambatte/libgambatte $(MAKECMDGOALS)
	mv gambatte/libgambatte/gambatte.dylib cores/gambatte.dylib

ohboy:
	$(MAKE) -f Makefile.osx_lib -C ohboy $(MAKECMDGOALS)
	mv ohboy/ohboy.dylib cores/ohboy.dylib

PocketSNES:
	$(MAKE) -f Makefile.osx_lib -C PocketSNES $(MAKECMDGOALS)
	mv PocketSNES/PocketSNES.dylib cores/pocketsnes.dylib

$(EXECUTABLE): $(addprefix $(BINDIR)/, $(BINARIES))
	$(CC) $(CCFLAGS) $^ -o $@ $(LDFLAGS)

#$(LD) $(LDFLAGS) main.o -o $@ $(LIBS)

clean:
	$(MAKE) -f Makefile.osx_lib -C geekboy $(MAKECMDGOALS)
	$(MAKE) -f Makefile.osx_lib -C gambatte/libgambatte $(MAKECMDGOALS)
	
	rm -f gcwemu
	rm -rf $(BINDIR)
	rm -rf cores/*.dylib
#	$(foreach var,$(CORES),$(MAKE) -f Makefile.osx_lib -C $(var) clean;)

$(BINDIR)/%.o : $(BASESRC)/%.cpp
	@test -d $(@D) || mkdir -p $(@D)
	$(CC) -DMAKE -c $(CCFLAGS) $< -o $@