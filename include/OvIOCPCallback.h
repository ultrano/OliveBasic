#pragma once

struct OvIOCPObject;
class OvIOCPCallback : public OvMemObject
{
public:
	virtual void OnConnected( void** user, OvByte** recv_buf, OvSize & buf_size ) = 0;
	virtual void OnDisconnected( void* user ) = 0;
	virtual void OnRecved( OvIOCPObject * iobj, OvSize completed_byte ) = 0;
	virtual void OnSended( OvIOCPObject * iobj, OvSize completed_byte ) = 0;
};

namespace OU
{
	namespace iocp
	{
		SOCKET	GetSocket	( OvIOCPObject * iobj );
		void*	GetUserData	( OvIOCPObject * iobj );
		void	SendData	( OvIOCPObject * iobj, OvByte * buf, OvSize len );
	}
}