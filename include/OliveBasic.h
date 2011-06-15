#pragma once
#include "OvTypeDef.h"

class OliveBasic
{
public:
	OliveBasic() { Startup(); };
	~OliveBasic() { Cleanup(); };
	static void Startup();
	static void Cleanup();
};