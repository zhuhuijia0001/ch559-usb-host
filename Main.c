#include "Type.h"
#include "Task.h"

int main()
{
	InitSystem();

	while (TRUE)
	{
		ProcessUsbHostPort();

		ProcessPs2Port();

        ProcessIpPs2Port();
        
		ProcessKeyboardLed();
	}
	
	return 0;
}

