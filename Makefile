.PHONY: all ohboy PocketSNES clean

CC = clang++

SDL_INCLUDES = $(shell sdl-config --cflags)
SDL_LIBS = $(shell sdl-config --libs)

CCFLAGS = -std=c++11 -stdlib=libc++ $(SDL_INCLUDES)
LDFLAGS += -arch x86_64 $(SDL_LIBS)

#SOURCES:= $(wildcard *.cpp)
#BINARIES:= $(foreach source, $(SOURCES), $(source:%.cpp=%.o) )
SOURCES:= main.cpp utils.cpp loader.cpp
BINARIES:= main.o utils.o loader.o
EXECUTABLE:= gcwemu

CORES:= ohboy PocketSNES

all: $(EXECUTABLE) $(CORES)

ohboy:
	$(MAKE) -f Makefile.osx_lib -C ohboy
	mv ohboy/ohboy.dylib cores/ohboy.dylib

PocketSNES:
	$(MAKE) -f Makefile.osx_lib -C PocketSNES
	mv PocketSNES/PocketSNES.dylib cores/pocketsnes.dylib

$(EXECUTABLE): $(BINARIES)
	$(CC) $(CCFLAGS) $(BINARIES) -o $@ $(LDFLAGS)

#$(LD) $(LDFLAGS) main.o -o $@ $(LIBS)

clean:
	rm -f gcw-emu *.o
	$(foreach var,$(CORES),$(MAKE) -f Makefile.osx_lib -C $(var) clean;)

.cpp.o:
	$(CC) -c $(CCFLAGS) $< -o $@