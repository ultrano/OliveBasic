#include "OliveBasic.h"

#include "OvMemoryMgr.h"
#include "OvObjectManager.h"

#include "OvValue.h"
#include "OvActObject.h"
#include "OvWinMsgComponent.h"

void OliveBasic::Startup()
{
	OvMemoryMgr::Startup();
	OvObjectManager::Startup();

	/// rising rtti
	OvFloatVal			::FACTORY();
	OvIntVal			::FACTORY();
	OvRectVal			::FACTORY();
	OvStringVal			::FACTORY();
	OvVector2Val		::FACTORY();
	OvVector3Val		::FACTORY();
	OvObjectIDVal		::FACTORY();
	OvQuaternionVal		::FACTORY();
	OvMatrixVal			::FACTORY();
	OvActObject			::FACTORY();
	OvWinMsgComponent	::FACTORY();
	////
}

void OliveBasic::Cleanup()
{
	OvObjectManager::Cleanup();
	OvMemoryMgr::Cleanup();
}
