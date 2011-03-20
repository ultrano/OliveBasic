#include "OvOlive.h"

#include "OvMemoryMgr.h"
#include "OvObjectManager.h"

void OvOliveBasic::Startup()
{
	OvMemoryMgr::GetInstance();
	OvObjectManager::GetInstance();
}

void OvOliveBasic::Cleanup()
{

}
