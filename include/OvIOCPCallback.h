#pragma once
#include <windows.h>
#include "OvTypeDef.h"
#include "OvMemObject.h"

struct OvIOCPObject;
class OvIOCPCallback : public OvMemObject
{
public:
	virtual void OnConnected( OvIOCPObject * iobj, void** user ) = 0;
	virtual void OnDisconnected( OvIOCPObject * iobj ) = 0;
	virtual void OnRecved( OvIOCPObject * iobj, OvSize completed_byte ) = 0;
	virtual void OnSended( OvIOCPObject * iobj, OvSize completed_byte ) = 0;
	virtual void OnErrOccured( OvIOCPObject * iobj, OvInt err_code ) = 0;
};

namespace OU
{
	namespace iocp
	{
		SOCKET	GetSocket	( OvIOCPObject * iobj );
		void*	GetUserData	( OvIOCPObject * iobj );
		void	RecvData	( OvIOCPObject * iobj, OvByte * buf, OvSize len );
		void	SendData	( OvIOCPObject * iobj, OvByte * buf, OvSize len );
		void	RemoveObject( OvIOCPObject * iobj );
	}
}