#pragma once
#include "OvSocket.h"
#include "OvBuffer.h"
#include "OvIOCPCallback.h"

class OvIOCPObject;

struct SCompletePort : OvMemObject
{
	enum CP { CP_Recv, CP_Send };
	SCompletePort();

	OVERLAPPED	overlapped;
	CP			port;
	OvIOCPObject* object;
	OvCriticalSection m_cs;
};

struct SRecvedPort : SCompletePort
{
	enum { MAX_RECV_BUFFER_SIZE = 512 };
	SRecvedPort();

	OvByte		 buffer[MAX_RECV_BUFFER_SIZE];
	OvSize		 recved_size;

	OvBufferSPtr Notify( OvSize recved );
};

struct SSendedPort : SCompletePort
{
	SSendedPort();
	~SSendedPort();
	OvBufferSPtr			buffer;
	OvSize					sended_size;

	void Notify( OvSize sended );
};

class OvIOCPObject : public OvMemObject
{
public:
	OvIOCPObject( OvSocketSPtr s );
	~OvIOCPObject();

	OvSocketSPtr		sock;
	SRecvedPort			recv_port;
	OvIOCPCallback::CallbackObject cb;

	void Send( OvByte * buf, OvSize bufsize );
	void DeleteSending( SSendedPort* port );
	void DeleteAllSending();

private:

	OvCriticalSection m_cs;
	OvSet<SSendedPort*>	sending_requests;

};