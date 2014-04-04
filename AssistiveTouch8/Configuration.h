#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include "Libs.h"

class Configuration
{
private:
	HINSTANCE       mDll;
	wchar_t         mFullPath[256];

public:
	Configuration(HINSTANCE Dll);
	~Configuration();

	struct Global
	{
		int Alpha;
		int Click;
	};

};

#endif //CONFIGURATION_H_