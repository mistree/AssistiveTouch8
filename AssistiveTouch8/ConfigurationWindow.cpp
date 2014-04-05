#include "ConfigurationWindow.h"

ConfigurationWindow* pConfig;

ConfigurationWindow::ConfigurationWindow(Configuration& Config)
: mConfig(Config)
{
	pConfig = this;
};

void ConfigurationWindow::Show()
{

};
