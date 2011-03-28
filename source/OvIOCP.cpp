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
	OvIOCPObject()
		: connected		( this, Connected )
		, recved		( this, Recved )
		, sended		( this, Sended )
	{
	}
	~OvIOCPObject(){};
	OvCriticalSection	cs;
	SOCKET				sock;
	SOverlapped			connected;
	SOverlapped			recved;
	SOverlapped			sended;
	char				discon_buf[1]; // 초기 한번, disconnect 를 감지 하기 위해 WSARecv 를 호출 할때 쓰는 버퍼.
};

//////////////////////////////////////////////////////////////////////////
OvIOCP::OvIOCP()
: m_iocp_handle( NULL )
, m_startup_complete( NULL )
{
}

OvIOCP::~OvIOCP()
{

}

OvBool OvIOCP::Startup( const OvString & ip, OvShort port, OvIOCPCallback * callback )
{
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

		OvIOCPObject * iobj = OvNew OvIOCPObject;
		iobj->sock = connerted;
		iocp->m_overlaps.insert( iobj );

		PostQueuedCompletionStatus( iocp_handle, 0, (DWORD)connerted, &iobj->connected.overlapped); // 워커 스레드에 종료 전송.
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
			iobj->cs.Lock();
			if ( 0 == nofb && Connected != overlapped->signal )
			{
				// disconnected
				iocp->m_overlaps.erase( iobj );
				OliveNet::CloseSock( iobj->sock );
				iobj->cs.Unlock();
				OvDelete iobj;
				printf("disconnected\n");
				continue;
			}
			switch ( overlapped->signal )
			{
			case Recved :
				{
					// recv
				}
				break;
			case Sended :
				{
					// send
				}
				break;
			case Connected :
				{
					// connected
					printf("connected\n");

					// 초기 한번, disconnected 를 감지 하기 위해 리슨 상태로 만들어 놓는다.
					DWORD dumy = 0;
					WSABUF buf = {1,iobj->discon_buf};
					CreateIoCompletionPort( (HANDLE)iobj->sock, iocp_handle, (DWORD)iobj->sock, WorkerThreadCount );
					WSARecv( iobj->sock, &buf, 1, &dumy, &dumy, &iobj->recved.overlapped, NULL );
				}
				break;
			}
			iobj->cs.Unlock();
		}

	}
};
