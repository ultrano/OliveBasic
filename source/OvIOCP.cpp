#include <WinSock2.h>
#include <MSWSock.h>
#include "OvIOCP.h"
#include "OvBuffer.h"
#include "OvBitFlags.h"
#include "OvIOCPCallback.h"
#include <process.h>

//////////////////////////////////////////////////////////////////////////

struct SOverlapped : OvMemObject
{
	enum IOCP_Signal{ Recv, Send, Accept };
	OVERLAPPED			overlapped;
	SOverlapped() : sock(NULL), user_data(NULL){ZeroMemory(&overlapped,sizeof(overlapped));};
	SOverlapped( IOCP_Signal sg ) : sock(NULL), signal(sg), user_data(NULL) {ZeroMemory(&overlapped,sizeof(overlapped));};
	IOCP_Signal			signal;
	SOCKET				sock;
	OvCriticalSection	cs;
	void*				user_data;
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

	OvUInt threadcount	= 4;
	m_iocp_handle		= CreateIoCompletionPort( (HANDLE)m_listensock, m_iocp_handle, (DWORD)m_listensock, threadcount );
	OvUInt count		= threadcount;
	while ( count-- )
	{
		SOverlapped * overlapped = OvNew SOverlapped(SOverlapped::Accept);
		overlapped->sock = WSASocket( AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0 , WSA_FLAG_OVERLAPPED );
		m_iocp_handle = CreateIoCompletionPort( (HANDLE)overlapped->sock, m_iocp_handle, (DWORD)overlapped->sock, threadcount );
		char dumy;
		if ( ! AcceptEx( m_listensock, overlapped->sock, &dumy, 0, sizeof(sockaddr_in)+16, sizeof(sockaddr_in)+16, NULL, &overlapped->overlapped ) )
		{
			int err = WSAGetLastError();
			if ( err != WSA_IO_PENDING &&
				 err != WSAEWOULDBLOCK)
			{
				continue;
			}
		}
		HANDLE threadid = (HANDLE)_beginthread( _worker, 0, this );
		m_threads.push_back( threadid );
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
			overlapped->cs.Lock();
			if ( 0 == nofb && SOverlapped::Accept != overlapped->signal )
			{
			}
			switch ( overlapped->signal )
			{
			case SOverlapped::Recv :
				{
				}
				break;
			case SOverlapped::Send :
				{
				}
				break;
			case SOverlapped::Accept :
				{
				}
				break;
			}
			overlapped->cs.Unlock();
		}

	}
};
