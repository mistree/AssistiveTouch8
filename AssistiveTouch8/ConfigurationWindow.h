#ifndef CONFIGURATIONWINDOW_H_
#define CONFIGURATIONWINDOW_H_

#include <Windows.h>
#include "Configuration.h"

class ConfigurationWindow
{
private:
	Configuration& mConfig;

public:
	ConfigurationWindow(Configuration& Config);

	void Show();

};

#endif //CONFIGURATIONWINDOW_H_