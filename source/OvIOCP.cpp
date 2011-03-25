#include <WinSock2.h>
#include "OvIOCP.h"
#include "OvBuffer.h"
#include "OvBitFlags.h"
#include <process.h>


struct SCompletePort
{
	enum CP { CP_Recv, CP_Send };
	SCompletePort() { ZeroMemory( &overlapped, sizeof( overlapped ) ); };
	OVERLAPPED	overlapped;
	CP			port;
	OvIOCP::IOCPObject* object;
};
struct SRecvedPort : SCompletePort
{
	enum { MAX_RECV_BUFFER_SIZE = 512 };
	SRecvedPort() : recved_size( 0 ) { port = CP_Recv; };
	OvByte		 buffer[MAX_RECV_BUFFER_SIZE];
	OvSize		 recved_size;
};

struct SSendedPort : SCompletePort
{
	SSendedPort( ) : sened_size( 0 ) { port = CP_Send; };
	
	OvBufferSPtr			buffer;
	OvSize					sened_size;
};

struct OvIOCP::IOCPObject : OvMemObject
{
	enum { MAX_SEND_BUFFER_COUNT = 16 };

	IOCPObject()
	{
		recv_port.object = this;

		unsigned count = MAX_SEND_BUFFER_COUNT;
		while ( count-- ) send_port[count].object = this;

		m_idle_buf.Clear( true );
	};

	OvSocketSPtr sock;

	SRecvedPort recv_port;
	SSendedPort send_port[MAX_SEND_BUFFER_COUNT];

	Ov16SetFlags m_idle_buf;

	OvIOCPCallback::CallbackObject cb;

};

//////////////////////////////////////////////////////////////////////////

OvIOCP::OvIOCP()
: m_callback( NULL )
{

}

OvIOCP::~OvIOCP()
{

}

void OvIOCP::Startup( const OvString & ip, OvShort port, OvIOCPCallback * callback )
{
	m_callback = callback;
	m_iocp = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );
	m_listener = OvSocket::Bind( ip, port );

	_beginthread( _accept_thread, 0, this );
	OvUInt count = 4;
	while ( count-- )
	{
		_beginthread( _worker_thread, 0, this );
	}
}

void OvIOCP::Cleanup()
{

}

void OvIOCP::_accept_thread( void * ptr )
{
	OvIOCP * iocp = (OvIOCP *)ptr;
	iocp->_accept();
}

void OvIOCP::_accept()
{
	OvSocketSPtr con = NULL;
	while ( con = OvSocket::Accept( m_listener ) )
	{
		IOCPObject * obj = OvNew IOCPObject;
		obj->sock	= con;
		obj->cb.sock = con;

		OnConnect( obj );

	}
}

void OvIOCP::_worker_thread( void * ptr )
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

		SCompletePort * port = (SCompletePort *)over;
		IOCPObject * obj = port->object;
		if ( 0 == num_of_byte )
		{
			iocp->OnDisconnect( obj );
			obj = NULL;
			continue;
		}

		printf( "%s : %d byte\n", (port->port==SCompletePort::CP_Recv)? "Recv":"Send", num_of_byte );

		switch ( port->port )
		{
		case SCompletePort::CP_Recv :
			{
				obj->recv_port.recved_size = num_of_byte;
				iocp->OnRecv( obj );
			}
			break;
		case SCompletePort::CP_Send :
			{
				iocp->OnSend( obj );
			}
			break;
		}
	}

}

void OvIOCP::_worker()
{

}

void OvIOCP::OnConnect( IOCPObject * obj )
{
	SOCKET sock = obj->sock->GetSock();
	m_iocp = CreateIoCompletionPort( (HANDLE)sock, m_iocp, (DWORD)sock, 4 );
	m_iocp_table[ obj->sock.GetRear() ] = obj;
	if( m_callback ) m_callback->OnConnect( &( obj->cb ) );

	DWORD numofbyte = 0;
	DWORD flag = 0;
	WSABUF buf;
	buf.buf = (char*)&obj->recv_port.buffer[0];
	buf.len = SRecvedPort::MAX_RECV_BUFFER_SIZE;
	int ret = WSARecv( obj->sock->GetSock(), &buf, 1, &numofbyte, &flag, (LPOVERLAPPED)&obj->recv_port, NULL );

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
	obj->cb.input.Reset( obj->recv_port.buffer, obj->recv_port.recved_size );

	if( m_callback ) m_callback->OnRecv( &( obj->cb ) );

	obj->recv_port.recved_size = 0;

	DWORD numofbyte = 0;
	DWORD flag = 0;
	WSABUF buf;
	buf.buf = (char*)&obj->recv_port.buffer[0];
	buf.len = SRecvedPort::MAX_RECV_BUFFER_SIZE;
	int ret = WSARecv( obj->sock->GetSock()
					 , &buf
					 , 1
					 , &numofbyte
					 , &flag
					 , (LPOVERLAPPED)&obj->recv_port
					 , NULL );
}

void OvIOCP::OnSend( IOCPObject * obj )
{
	if( m_callback ) m_callback->OnSend( &( obj->cb ) );
}