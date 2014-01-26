#include "emu_interface.h"

#include <SDL/SDL.h>

#include "dlfcn.h"

int main(int argc, char **argv)
{
	void *handle = dlopen("cores/ohboy.dylib", RTLD_LOCAL|RTLD_NOW);
	
	EmuInterface* (*retrieve)();
	*(void**) (&retrieve) = dlsym(handle, "retrieve");
	EmuInterface *interface = retrieve();
	
	interface->run(argc, argv);
	
	dlclose(handle);

	return 0;
}
