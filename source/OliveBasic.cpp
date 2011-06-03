#include "OliveBasic.h"

#include "OvMemoryMgr.h"
#include "OvObjectManager.h"

void OliveBasic::Startup()
{
	OvMemoryMgr::Startup();
	OvObjectManager::Startup();
}

void OliveBasic::Cleanup()
{
	OvObjectManager::Cleanup();
	OvMemoryMgr::Cleanup();
}
