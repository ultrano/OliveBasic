#pragma once
#pragma comment(lib,"WS2_32")
#pragma comment(lib,"MSWSock")

#include "OvRefObject.h"
#include "OvUtility.h"
#include "OvSocketInputStream.h"
#include "OvSocketOutputStream.h"

namespace OliveNet
{
	OvBool Startup();
	OvBool Cleanup();

	SOCKET Connect( const string& ip, OvShort port );
	SOCKET Bind( const OvString& ip, OvShort port );
	SOCKET Accept( SOCKET listen );
	void   CloseSock( SOCKET sock );

}