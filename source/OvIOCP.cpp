#include <WinSock2.h>
#include <MSWSock.h>
#include "OvIOCP.h"
#include "OvBuffer.h"
#include "OvBitFlags.h"
#include <process.h>
#define WorkerThreadCount 4

//////////////////////////////////////////////////////////////////////////

enum IOCP_Signal{ Connected, Recved, Sended, };

//////////////////////////////////////////////////////////////////////////

struct OvIOCPObject;

struct SOverlapped : OvMemObject
{
	OVERLAPPED			overlapped;
	SOverlapped(OvIOCPObject* obj, IOCP_Signal sig) 
		: iobj( obj ), signal( sig ) 
	{ ZeroMemory(&overlapped,sizeof(overlapped)); };
	OvIOCPObject*		iobj;
	IOCP_Signal			signal;
};
//////////////////////////////////////////////////////////////////////////

struct OvIOCPObject : public OvMemObject
{
	OvIOCPObject( SOCKET s )
		: sock			( s )
		, connected		( this, Connected )
		, recved		( this, Recved )
		, sended		( this, Sended )
		, user_data		( NULL )
	{
		ZeroMemory( &recv_buf, sizeof(recv_buf) );
	}
	~OvIOCPObject(){};
	OvCriticalSection	cs;
	SOCKET				sock;
	SOverlapped			connected;
	SOverlapped			recved;
	SOverlapped			sended;
	void*				user_data;
	WSABUF				recv_buf;
	char				discon_buf[1]; // 초기 한번, disconnect 를 감지 하기 위해 WSARecv 를 호출 할때 쓰는 버퍼.
};

//////////////////////////////////////////////////////////////////////////
OvIOCP::OvIOCP()
: m_iocp_handle( NULL )
, m_startup_complete( NULL )
, m_callback( NULL )
{
}

OvIOCP::~OvIOCP()
{

}

OvBool OvIOCP::Startup( const OvString & ip, OvShort port, OvIOCPCallback * callback )
{
	m_callback = callback;
	m_iocp_handle		= CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, NULL, NULL );
	m_startup_complete	= CreateEvent( NULL, true, false, NULL);
	m_on_cleaningup		= CreateEvent( NULL, true, true, NULL);
	m_listensock		= OliveNet::Bind(ip,port);
	m_terminate			= false;

	OvUInt count		= WorkerThreadCount;
	while ( count-- )
	{
		m_threads.push_back( (HANDLE)_beginthread( _worker, 0, this ) );
		m_threads.push_back( (HANDLE)_beginthread( _accepter, 0, this ) );
	}

	SetEvent(m_startup_complete);
	CloseHandle(m_startup_complete);
	return true;
}

void OvIOCP::Cleanup()
{
	ResetEvent( m_on_cleaningup ); // 일단 종료 상태를 신호한다.

	OliveNet::CloseSock(m_listensock);
	OvUInt threadcount = m_threads.size();
	while ( threadcount-- )
	{
		PostQueuedCompletionStatus(m_iocp_handle,0,0,0); // 워커 스레드에 종료 전송.
	}

	m_terminate = true; // 종료 플래그를 세운다
	SetEvent( m_on_cleaningup ); // 다시 워커스레드 진행, 종료 플래그에 의해 종료.

	WaitForMultipleObjects( m_threads.size(), &m_threads[0], true, INFINITE ); // 종료 되는 워크들을 모두 대기.

	// 모든 워크 스레드가 종료되어 더이상 추가 삭제가 발생 하지 않게 되었을때, 남아 있는 IOCP Object 들을 삭제.
	OvMTSet<OvIOCPObject*>::value_list objlist;
	m_overlaps.copy_value_list( objlist );
	for each ( OvIOCPObject * obj in objlist )
	{
		OvDelete obj;
	}

	m_threads.clear();
}

HANDLE OvIOCP::GetIOCPHandle()
{
	return m_iocp_handle;
}

//////////////////////////////////////////////////////////////////////////

void OvIOCP::_accepter( void * p )
{
	OvIOCP* iocp = (OvIOCP*)p;
	SOCKET listensock = iocp->m_listensock;
	HANDLE cleaningup_handle = iocp->m_on_cleaningup;
	WaitForSingleObject( iocp->m_startup_complete, INFINITE );

	HANDLE iocp_handle = iocp->m_iocp_handle;

	SOCKET connerted = INVALID_SOCKET;
	while ( INVALID_SOCKET != (connerted = OliveNet::Accept( listensock )) )
	{
		WaitForSingleObject( cleaningup_handle, INFINITE );
		if ( iocp->m_terminate ) return ;

		OvIOCPObject * iobj = OvNew OvIOCPObject(connerted);
		iocp->m_overlaps.insert( iobj );
		PostQueuedCompletionStatus( iocp_handle, 0, (DWORD)connerted, &iobj->connected.overlapped); // 워커 스레드에 종료 전송.
	}

}

//////////////////////////////////////////////////////////////////////////
void OvIOCP::_on_connected( OvIOCPObject * iobj )
{
	if ( !iobj ) return ;

	if ( m_callback )
	{
		OvAutoSection callback_lock( m_callback_cs );
		m_callback->OnConnected( &iobj->user_data, (OvByte**)&iobj->recv_buf.buf, (OvSize&)iobj->recv_buf.len );
	}

	printf("connected\n");

	if ( iobj->recv_buf.buf && iobj->recv_buf.len )
	{
		DWORD dumy = 0;
		CreateIoCompletionPort( (HANDLE)iobj->sock, m_iocp_handle, (DWORD)iobj->sock, WorkerThreadCount );
		WSARecv( iobj->sock, &iobj->recv_buf, 1, &dumy, &dumy, &iobj->recved.overlapped, NULL );
	}
	else
	{
		_on_disconnected( iobj );
	}
}
void OvIOCP::_on_disconnected( OvIOCPObject * iobj )
{
	if ( !iobj ) return ;
	iobj->cs.Lock();
	OliveNet::CloseSock( iobj->sock );
	m_overlaps.erase( iobj );
	iobj->cs.Unlock();
	OvAutoSection callback_lock( m_callback_cs );
	OvDelete iobj;
	printf("disconnected\n");
}
void OvIOCP::_on_sended( OvIOCPObject * iobj, OvSize completed_byte )
{
	if ( !iobj ) return ;
	OvAutoSection lock(iobj->cs);
	if ( m_callback )
	{
		OvAutoSection callback_lock( m_callback_cs );
		m_callback->OnSended( iobj, completed_byte );
	}
	printf("send\n");
}
void OvIOCP::_on_recved( OvIOCPObject * iobj, OvSize completed_byte )
{
	if ( !iobj ) return ;
	OvAutoSection lock(iobj->cs);

	if ( m_callback )
	{
		OvAutoSection callback_lock( m_callback_cs );
		m_callback->OnRecved( iobj, completed_byte );
	}

	printf("recv\n");

	if ( iobj->recv_buf.buf && iobj->recv_buf.len )
	{
		DWORD dumy = 0;
		WSARecv( iobj->sock, &iobj->recv_buf, 1, &dumy, &dumy, &iobj->recved.overlapped, NULL );
	}
	else
	{
		_on_disconnected( iobj );
	}

}
//////////////////////////////////////////////////////////////////////////
void OvIOCP::_worker( void * p )
{
	OvIOCP* iocp = (OvIOCP*)p;
	HANDLE cleaningup_handle = iocp->m_on_cleaningup;
	WaitForSingleObject( iocp->m_startup_complete, INFINITE );

	HANDLE iocp_handle = iocp->m_iocp_handle;

	while ( true )
	{
		DWORD nofb = 0;
		DWORD key  = 0;
		SOverlapped * overlapped = NULL;
		GetQueuedCompletionStatus( iocp_handle
								 , &nofb
								 , (DWORD*)&key
								 , (LPOVERLAPPED*)(&overlapped)
								 , INFINITE );
		WaitForSingleObject( cleaningup_handle, INFINITE );
		if ( iocp->m_terminate ) return ;
		if ( !key || !overlapped ) return ;

		{
			OvIOCPObject * iobj = overlapped->iobj;
			if ( 0 == nofb && Connected != overlapped->signal )
			{
				iocp->_on_disconnected( iobj );
				continue;
			}
			switch ( overlapped->signal )
			{
			case Recved :
				{
					iocp->_on_recved( iobj, nofb );
				}
				break;
			case Sended :
				{
					iocp->_on_sended( iobj, nofb );
				}
				break;
			case Connected :
				{
					iocp->_on_connected( iobj );
				}
				break;
			}
		}

	}
};

SOCKET OU::iocp::GetSocket( OvIOCPObject * iobj )
{
	if ( iobj )
	{
		return iobj->sock;
	}
	return INVALID_SOCKET;
}

void* OU::iocp::GetUserData( OvIOCPObject * iobj )
{
	if ( iobj )
	{
		return iobj->user_data;
	}
	return NULL;
}

void OU::iocp::SendData( OvIOCPObject * iobj, OvByte * buf, OvSize len )
{
	if ( iobj && (buf && len) )
	{
		WSABUF wsabuf = {len, (char*)buf};
		WSASend( iobj->sock
			, &wsabuf
			, 1
			, NULL
			, 0
			, &iobj->sended.overlapped
			, NULL );
	}
}
