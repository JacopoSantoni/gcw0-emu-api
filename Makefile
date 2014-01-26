.PHONY: all ohboy

CC = clang++
CCFLAGS = -std=c++11 -stdlib=libc++
LDFLAGS += -arch x86_64

#SOURCES:= $(wildcard *.cpp)
#BINARIES:= $(foreach source, $(SOURCES), $(source:%.cpp=%.o) )
SOURCES:= main.cpp 
BINARIES:= main.o
EXECUTABLE:= gcwemu

all: $(EXECUTABLE) ohboy

ohboy:
	$(MAKE) -f Makefile.osx_lib -C ohboy
	mv ohboy/ohboy.dylib cores/ohboy.dylib

$(EXECUTABLE): $(BINARIES)
	$(CC) $(CCFLAGS) $(BINARIES) -o $@ $(LDFLAGS) `sdl-config --libs`

#$(LD) $(LDFLAGS) main.o -o $@ $(LIBS)

clean:
	rm -f gcw-emu *.o

.cpp.o:
	$(CC) -c $(CCFLAGS) $< -o $@