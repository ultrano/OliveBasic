#pragma once
#include "OvRefObject.h"
#include "OvSocket.h"
#include "OvBuffer.h"
class OvIOCPObject;
interface_class OvIOCPCallback : OvRefObject
{
public:
	struct CallbackObject : OvMemObject
	{
		CallbackObject( const OvIOCPObject* obj );
		const OvIOCPObject*	iocp_obj;
		OvSocketSPtr	sock;
		void *			user_data;
		void			Sending( OvByte * buf, OvSize bufsize );
	};


	virtual void OnConnect( CallbackObject * obj ) = 0;
	virtual void OnDisconnect( CallbackObject * obj ) = 0;
	virtual void OnReceive( CallbackObject * obj, OvBufferSPtr buf ) = 0;
};