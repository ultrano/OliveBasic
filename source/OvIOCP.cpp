#include <WinSock2.h>
#include <MSWSock.h>
#include "OvIOCP.h"
#include "OvIOCPCallback.h"
#include "OvBuffer.h"
#include "OvBitFlags.h"
#include <process.h>
#define WorkerThreadCount 4

//////////////////////////////////////////////////////////////////////////

enum IOCP_Signal{ Connected, Recved, Sended, ErrOccured };

//////////////////////////////////////////////////////////////////////////

struct OvIOCPObject;

struct SOverlapped : OvMemObject
{
	OVERLAPPED			overlapped;
	SOverlapped(OvIOCPObject* obj, IOCP_Signal sig) 
		: iobj( obj ), signal( sig ) 
	{ ZeroMemory(&overlapped,sizeof(overlapped)); };
	~SOverlapped()
	{
		iobj = NULL;
	}
	OvIOCPObject*		iobj;
	IOCP_Signal			signal;
};

//////////////////////////////////////////////////////////////////////////

struct OvIOCPObject : public OvMemObject
{
	OvIOCPObject( OvIOCP * _iocp, SOCKET s )
		: iocp			( _iocp )
		, sock			( s )
		, connected		( this, Connected )
		, recved		( this, Recved )
		, sended		( this, Sended )
		, erroccured	( this, ErrOccured )
		, user_data		( NULL )
	{
	}
	~OvIOCPObject()
	{
		iocp = NULL;
		sock = INVALID_SOCKET;
		user_data = NULL;
	};
	OvCriticalSection	cs;
	OvIOCP *			iocp;
	SOCKET				sock;
	SOverlapped			connected;
	SOverlapped			recved;
	SOverlapped			sended;
	SOverlapped			erroccured;
	void*				user_data;
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
	CloseHandle( m_iocp_handle );
	m_iocp_handle = INVALID_HANDLE_VALUE;
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

		OvIOCPObject * iobj = OvNew OvIOCPObject( iocp, connerted );
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
		CreateIoCompletionPort( (HANDLE)iobj->sock, m_iocp_handle, (DWORD)iobj->sock, WorkerThreadCount );
		m_callback->OnConnected( iobj, &(iobj->user_data) );
	}
}

void OvIOCP::_on_disconnected( OvIOCPObject * iobj )
{
	if ( !iobj ) return ;
	if ( !m_overlaps.is_contained( iobj ) ) return ;

	if ( m_callback )
	{
		OvAutoSection callback_lock( m_callback_cs );
		m_callback->OnDisconnected( iobj );
	}
	shutdown( iobj->sock, SD_BOTH );
	closesocket( iobj->sock );
	m_overlaps.erase( iobj );
	OvDelete iobj;
	printf("disconnected\n");
}

void OvIOCP::_on_sended( OvIOCPObject * iobj, OvSize completed_byte )
{
	if ( !iobj ) return ;
	if ( m_callback )
	{
		OvAutoSection callback_lock( m_callback_cs );
		m_callback->OnSended( iobj, completed_byte );
	}
}

void OvIOCP::_on_recved( OvIOCPObject * iobj, OvSize completed_byte )
{
	if ( !iobj ) return ;

	if ( m_callback )
	{
		OvAutoSection callback_lock( m_callback_cs );
		m_callback->OnRecved( iobj, completed_byte );
	}

}

void OvIOCP::_on_erroccured( OvIOCPObject * iobj, OvInt err_code )
{
	if ( !iobj ) return ;
	if ( m_callback )
	{
		OvAutoSection callback_lock( m_callback_cs );
		m_callback->OnErrOccured( iobj, err_code );
	}
}

//////////////////////////////////////////////////////////////////////////
void OvIOCP::_worker( void * p )
{
	OvIOCP* iocp = (OvIOCP*)p;
	HANDLE cleaningup_handle = iocp->m_on_cleaningup;
	WaitForSingleObject( iocp->m_startup_complete, INFINITE );

	HANDLE iocp_handle = iocp->m_iocp_handle;
	BOOL getresult = false;
	while ( true )
	{
		DWORD nofb = 0;
		DWORD key  = 0;
		SOverlapped * overlapped = NULL;
		getresult = GetQueuedCompletionStatus( iocp_handle
								 , &nofb
								 , (DWORD*)&key
								 , (LPOVERLAPPED*)(&overlapped)
								 , INFINITE );
		WaitForSingleObject( cleaningup_handle, INFINITE );
		if ( iocp->m_terminate ) return ;
		if ( !key || !overlapped ) return ;

		{
			volatile OvAutoSection lock(iocp->m_worker_cs);
// 			printf( "getresult : %s\n", getresult? "true":"false" );
// 			printf( "nofb : %d\n", nofb );
// 			printf( "overlapped->signal : %d\n", overlapped->signal );
			OvIOCPObject * iobj = overlapped->iobj;
			if ( !getresult || (0 == nofb && Connected != overlapped->signal) )
			{
				iocp->_on_disconnected( iobj );
				continue;
			}
			switch ( overlapped->signal )
			{
			case Connected :
				{
					iocp->_on_connected( iobj );
				}
				break;
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
			case ErrOccured :
				{
					iocp->_on_erroccured( iobj, nofb );
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

void OU::iocp::RecvData( OvIOCPObject * iobj, OvByte * buf, OvSize len )
{
	if ( iobj && (buf && len) )
	{
		DWORD dummy = 0;
		WSABUF wsabuf = {len, (char*)buf};
		int ret = WSARecv( iobj->sock
			, &(wsabuf)
			, 1
			, (DWORD*)&dummy 
			, (DWORD*)&dummy 
			, &(iobj->recved.overlapped)
			, NULL );
		if ( ret == SOCKET_ERROR )
		{
			int err = WSAGetLastError();
			if ( err != WSAEWOULDBLOCK &&
				 err != WSA_IO_PENDING )
			{
				PostQueuedCompletionStatus( iobj->iocp->GetIOCPHandle(), err, (DWORD)iobj->sock, &iobj->erroccured.overlapped );
			}
		}
	}
}

void OU::iocp::SendData( OvIOCPObject * iobj, OvByte * buf, OvSize len )
{
	if ( iobj && (buf && len) )
	{
		WSABUF wsabuf = {len, (char*)buf};
		int ret = WSASend( iobj->sock
			, &(wsabuf)
			, 1
			, (DWORD*)&len
			, 0
			, &(iobj->sended.overlapped)
			, NULL );
		if ( ret == SOCKET_ERROR )
		{
			int err = WSAGetLastError();
			if ( err == WSAECONNRESET  )
			{
				PostQueuedCompletionStatus( iobj->iocp->GetIOCPHandle(), 0, (DWORD)iobj->sock, &iobj->recved.overlapped );
			}
			else if ( err != WSAEWOULDBLOCK &&
				err != WSA_IO_PENDING )
			{
				PostQueuedCompletionStatus( iobj->iocp->GetIOCPHandle(), err, (DWORD)iobj->sock, &iobj->erroccured.overlapped );
			}
		}
	}
}

void OU::iocp::RemoveObject( OvIOCPObject * iobj )
{
	if ( iobj )
	{
		PostQueuedCompletionStatus( iobj->iocp->GetIOCPHandle(), 0, (DWORD)iobj->sock, &iobj->erroccured.overlapped );
	}
}
