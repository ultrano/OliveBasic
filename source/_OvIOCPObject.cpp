#include <WinSock2.h>
#include "_OvIOCPObject.h"


OvIOCPObject::OvIOCPObject( OvSocketSPtr s )
: sock( s )
, cb( this )
{
	OvAutoSection lock(m_cs);
	cb.sock = s ;
	recv_port.object = this;
}

OvIOCPObject::~OvIOCPObject()
{
	DeleteAllSending();
	sock = NULL;
	cb.sock = NULL;
}

void OvIOCPObject::Send( OvByte * buf, OvSize bufsize )
{
	OvAutoSection lock(m_cs);
	OvBufferOutputStream bos( OvBuffer::CreateBuffer( bufsize ) );
	bos.WriteBytes( buf, bufsize );

	SSendedPort* sendport = OvNew SSendedPort;
	sendport->buffer = bos.GetBuffer();
	sendport->sended_size = 0;
	sendport->object = this;

	sending_requests.insert( sendport );

	sendport->Notify( 0 );
}

void OvIOCPObject::DeleteSending( SSendedPort* port )
{
	OvAutoSection lock(m_cs);
	if ( sending_requests.end() != sending_requests.find( port ) )
	{
		sending_requests.erase( port );
		OvDelete port;
	}
}

void OvIOCPObject::DeleteAllSending()
{
	OvAutoSection lock(m_cs);
	if ( sending_requests.size() )
	{
		for each ( SSendedPort * send in sending_requests )
		{
			OvDelete send;
		}
		sending_requests.clear();
	}
}

SCompletePort::SCompletePort()
{
	ZeroMemory( &overlapped, sizeof( overlapped ) );
}

SRecvedPort::SRecvedPort() : recved_size( 0 )
{
	OvAutoSection lock(m_cs);
	port = CP_Recv;
}

OvBufferSPtr SRecvedPort::Notify( OvSize recved )
{
	OvAutoSection lock(m_cs);
	recved_size = recved;
	OvBufferSPtr ret = NULL;
	if ( recved_size )
	{
		OvBufferOutputStream bos( OvBuffer::CreateBuffer( recved_size ) );
		bos.WriteBytes( buffer, recved_size );
		ret = bos.GetBuffer();
	}
	DWORD nofb = 0;
	DWORD flag = 0;
	WSABUF buf;
	buf.buf = (char*)&buffer[0];
	buf.len = MAX_RECV_BUFFER_SIZE;
	WSARecv( object->sock->GetSock(), &buf, 1, &nofb, &flag, (LPOVERLAPPED)this, NULL );
	return ret;
}

SSendedPort::SSendedPort() : buffer( NULL ), sended_size( 0 )
{
	OvAutoSection lock(m_cs);
	port = CP_Send;
}

SSendedPort::~SSendedPort()
{

}

void SSendedPort::Notify( OvSize sended )
{
	OvAutoSection lock(m_cs);
	if ( buffer )
	{
		sended_size += sended;
		OvByte * ptr = buffer->Pointer();
		OvSize bufsize = buffer->Size();
		if ( sended_size < bufsize )
		{
			DWORD nofb = 0;
			WSABUF buf;
			buf.buf = (char*)(ptr + sended_size);
			buf.len = bufsize - sended_size;
			WSASend( object->sock->GetSock(), &buf, 1, &nofb, 0, (LPOVERLAPPED)this, NULL );
		}
		else
		{
			object->DeleteSending( this );
		}
	}
}

void OvIOCPCallback::CallbackObject::Sending( OvByte * buf, OvSize bufsize )
{
	const_cast<OvIOCPObject*>(iocp_obj)->Send(buf,bufsize);
}

OvIOCPCallback::CallbackObject::CallbackObject( const OvIOCPObject* obj ) 
: iocp_obj(obj)
, user_data(NULL)
{
};