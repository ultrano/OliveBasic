#include "OliveBasic.h"

#include "OvMemoryMgr.h"
#include "OvObjectManager.h"

#include "OvActObject.h"
#include "OvWinMsgComponent.h"

OliveBasic::OliveBasic()
{
	OvMemoryMgr::Startup();
	OvObjectManager::Startup();

}

OliveBasic::~OliveBasic()
{
	OvObjectManager::Cleanup();
	OvMemoryMgr::Cleanup();
}
