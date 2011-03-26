#include "OvOlive.h"

#include "OvMemoryMgr.h"
#include "OvObjectManager.h"

void OvOliveBasic::Startup()
{
	OvMemoryMgr::Startup();
	OvObjectManager::Startup();
}

void OvOliveBasic::Cleanup()
{
	OvObjectManager::Teardown();
	OvMemoryMgr::Teardown();
}
