#include "OvOlive.h"

#include "OvMemoryMgr.h"
#include "OvObjectManager.h"

void OvOliveBasic::StartUp()
{
	OvMemoryMgr::GetInstance();
	OvObjectManager::GetInstance();
}

void OvOliveBasic::ShutDown()
{

}
