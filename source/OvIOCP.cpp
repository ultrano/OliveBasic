#include <WinSock2.h>
#include "OvIOCP.h"
#include "OvBuffer.h"
#include "OvBitFlags.h"
#include "OvIOCPCallback.h"
#include "_OvIOCPObject.h"
#include <process.h>


//////////////////////////////////////////////////////////////////////////

OvIOCP::OvIOCP()
: m_callback( NULL )
, m_iocp( NULL )
{
	_set_shutdown(false);
}

OvIOCP::~OvIOCP()
{

}

OvBool OvIOCP::Startup( const OvString & ip, OvShort port, OvIOCPCallback * callback )
{
	m_iocp = NULL;
	m_callback = NULL;
	if ( m_listener = OvSocket::Bind( ip, port ) )
	{
		m_callback = callback;
		m_iocp = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );
		OvSocket::Address addr;
		m_listener->GetSockAddr(addr);
		printf("bind : %s:%d", addr.ip.c_str(), addr.port );
		HANDLE handle;
		handle = ( HANDLE )_beginthread( _accept_thread, 0, this );
		OvUInt count = 4;
		while ( count-- )
		{
			handle = ( HANDLE )_beginthread( _worker_thread, 0, this );
			m_threads.push_back( handle  );
		}
		return true;
	}
	return false;
}

void OvIOCP::Cleanup()
{
	m_cs.Lock();

	_set_shutdown(true);
	m_callback = NULL;

	if ( m_listener )
	{
		m_listener->Close();
	}

	if ( OvUInt count = m_threads.size() )
	{
		while ( count-- )
		{
			PostQueuedCompletionStatus( m_iocp, 0, 0, 0 );
		}
		WaitForMultipleObjects(m_threads.size(), &m_threads[0],true,INFINITE);
	}
	if ( m_iocp ) CloseHandle( m_iocp );

	for each ( OvIOCPObject * obj in m_iocp_objects )
	{
		OvDelete obj;
	}
	m_iocp_objects.clear();
	m_cs.Unlock();
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
		_on_connect( con );
	}
}

void OvIOCP::_worker_thread( void * ptr )
{
	OvIOCP * iocp = (OvIOCP *)ptr;
	iocp->_worker();

}

void OvIOCP::_worker()
{

	DWORD num_of_byte	= 0;
	DWORD completed_key	= 0;
	while ( true )
	{
		LPOVERLAPPED over;
		GetQueuedCompletionStatus( m_iocp
			, (DWORD*)&num_of_byte
			, (DWORD*)&completed_key
			, (LPOVERLAPPED*)&over
			, INFINITE);

		if ( !completed_key || !over ) return ;
		if (_is_shutdown()) return ;

		SCompletePort * port = (SCompletePort *)over;
		OvIOCPObject * obj = port->object;
		if ( 0 == num_of_byte )
		{
			_on_disconnect( obj );
			obj = NULL;
			continue;
		}

		switch ( port->port )
		{
		case SCompletePort::CP_Recv :
			{
				OvBufferSPtr buf = ((SRecvedPort*)port)->Notify( num_of_byte );
				if ( m_callback && !_is_shutdown() )
				{
					m_callback->OnReceive( &(obj->cb) ,buf );
				}
			}
			break;
		case SCompletePort::CP_Send :
			{
				if (_is_shutdown()) return ;
				((SSendedPort*)port)->Notify( num_of_byte );
			}
			break;
		}
	}

}

void OvIOCP::_on_connect( OvSocketSPtr sock )
{
	OvAutoSection lock(m_cs);
	if (_is_shutdown()) return ;
	OvIOCPObject * obj = OvNew OvIOCPObject( sock );
	m_iocp_objects.insert( obj );

	m_iocp = CreateIoCompletionPort( (HANDLE)sock->GetSock(), m_iocp, (DWORD)sock->GetSock(), 4 );

	obj->recv_port.Notify( 0 );
}

void OvIOCP::_on_disconnect( OvIOCPObject * obj )
{
	OvAutoSection lock(m_cs);
	if (_is_shutdown()) return ;
	m_iocp_objects.erase(obj);
	OvDelete obj;
}

void OvIOCP::_set_shutdown( OvBool b )
{
	OvAutoSection lock(m_shutdown_cs);
	m_shutdown = b;
}

OvBool OvIOCP::_is_shutdown()
{
	OvAutoSection lock(m_shutdown_cs);
	OvBool ret = m_shutdown;
	return ret;
}
