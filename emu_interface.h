#include <vector>
#include <string>

class EmuInterface
{
	public:
		virtual ~EmuInterface() { }
		virtual void run(int argc, char **argv) = 0;
};
