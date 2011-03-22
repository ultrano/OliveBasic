#include <WinSock2.h>
#include "OvIOCP.h"
#include "OvBuffer.h"
#include <process.h>

struct OvIOCP::IOCPObject : OvMemObject
{
	enum { MAX_BUFFER_SIZE = 512 };
	struct CompletePort
	{
		enum CP { CP_Recv, CP_Send };
		OVERLAPPED	overlapped;
		CP			port;
		IOCPObject* object;
	};
	IOCPObject()
	{
		ZeroMemory( recv, sizeof( recv ) );
		ZeroMemory( send, sizeof( send ) );
		ZeroMemory( &recv_port, sizeof( recv_port ) );
		ZeroMemory( &send_port, sizeof( send_port ) );
		recv_port.object = this;
		send_port.object = this;
		recv_port.port = CompletePort::CP_Recv;
		send_port.port = CompletePort::CP_Send;
	}
	OvSocketSPtr sock;

	CompletePort recv_port;	
	OvByte		 recv[MAX_BUFFER_SIZE];
	OvSize		 recv_size;

	CompletePort send_port;
	OvByte		 send[MAX_BUFFER_SIZE];
	OvSize		 send_size;

	OvIOCPCallback::CallbackObject cb;

};

void OvIOCP::Begin()
{
	m_iocp = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );
	m_listener = OvSocket::Bind( "*", 5020 );

	_beginthread( _accept, 0, this );
	OvUInt count = 4;
	while ( count-- )
	{
		_beginthread( _worker, 0, this );
	}

}

void OvIOCP::End()
{

}

void OvIOCP::_accept( void * ptr )
{
	OvIOCP * iocp = (OvIOCP *)ptr;

	OvSocketSPtr con = NULL;
	while ( con = OvSocket::Accept( iocp->m_listener ) )
	{
		IOCPObject * obj = OvNew IOCPObject;
		obj->sock	= con;
		obj->recv_size = 0;
		obj->send_size = 0;
		obj->cb.sock = con;

		iocp->OnConnect( obj );

	}
}

void OvIOCP::_worker( void * ptr )
{
	OvIOCP * iocp = (OvIOCP *)ptr;

	DWORD num_of_byte	= 0;
	DWORD completed_key	= 0;
	while ( true )
	{
		LPOVERLAPPED over;
		GetQueuedCompletionStatus( iocp->m_iocp
			, (DWORD*)&num_of_byte
			, (DWORD*)&completed_key
			, (LPOVERLAPPED*)&over
			, INFINITE);

		IOCPObject::CompletePort * port = (IOCPObject::CompletePort *)over;
		IOCPObject * obj = port->object;
		if ( 0 == num_of_byte )
		{
			iocp->OnDisconnect( obj );
			obj = NULL;
			continue;
		}

		switch ( port->port )
		{
		case IOCPObject::CompletePort::CP_Recv :
			{
				obj->recv_size += num_of_byte;
				iocp->OnRecv( obj );
			}
			break;
		case IOCPObject::CompletePort::CP_Send :
			{
				obj->send_size += num_of_byte;
				iocp->OnSend( obj );
			}
			break;
		}
	}

}

void OvIOCP::OnConnect( IOCPObject * obj )
{
	SOCKET sock = obj->sock->GetSock();
	m_iocp = CreateIoCompletionPort( (HANDLE)sock, m_iocp, (DWORD)&sock, 4 );
	m_iocp_table[ obj->sock.GetRear() ] = obj;
	if( m_callback ) m_callback->OnConnect( &( obj->cb ) );

	DWORD numofbyte = 0;
	DWORD flag = 0;
	WSABUF buf;
	buf.buf = (char*)&obj->recv[0];
	buf.len = IOCPObject::MAX_BUFFER_SIZE;
	WSARecv( obj->sock, &buf, 1, &numofbyte, &flag, (LPOVERLAPPED)&obj->recv_port, NULL );
}

void OvIOCP::OnDisconnect( IOCPObject * obj )
{
	OvSocket * sock = obj->sock.GetRear();
	OvIOCPCallback::CallbackObject cbobj = obj->cb;
	m_iocp_table.erase( sock );
	if( m_callback ) m_callback->OnDisconnect( &cbobj );
}

void OvIOCP::OnRecv( IOCPObject * obj )
{
	if( m_callback ) m_callback->OnRecv( &( obj->cb ) );
}

void OvIOCP::OnSend( IOCPObject * obj )
{
	if( m_callback ) m_callback->OnSend( &( obj->cb ) );
}

OvIOCP::OvIOCP()
: m_callback( NULL )
{

}

OvIOCP::~OvIOCP()
{

}
