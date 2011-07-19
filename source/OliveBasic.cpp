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
	OvFloatVal			::RTTI();
	OvIntVal			::RTTI();
	OvRectVal			::RTTI();
	OvStringVal			::RTTI();
	OvVector2Val		::RTTI();
	OvVector3Val		::RTTI();
	OvObjectIDVal		::RTTI();
	OvQuaternionVal		::RTTI();
	OvMatrixVal			::RTTI();
	OvActObject			::RTTI();
	OvWinMsgComponent	::RTTI();
	////
}

void OliveBasic::Cleanup()
{
	OvObjectManager::Cleanup();
	OvMemoryMgr::Cleanup();
}
