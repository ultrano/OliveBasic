#include <WinSock2.h>
#include "OvIOCPInputStream.h"
#include "OvIOCP.h"
#include "OvIOCPObject.h"
OvRTTI_IMPL(OvIOCPInputStream)

OvIOCPInputStream::OvIOCPInputStream( OvIOCPObject * obj )
: m_object( obj )
{

}

OvIOCPInputStream::~OvIOCPInputStream()
{
	m_object = NULL;
}

OvSize OvIOCPInputStream::ReadBytes( OvByte * dest, OvSize dest_size )
{
	if (  ( m_object && m_object->sock != INVALID_SOCKET ) && (dest && dest_size) )
	{
		WSABUF wsabuf = {dest_size, (char*)dest};
		dest_size = 0;
		int ret = WSARecv( m_object->sock
			, &(wsabuf)
			, 1
			, (DWORD*)&dest_size
			, (DWORD*)&dest_size
			, (LPOVERLAPPED)( m_object->recved )
			, NULL );
		if ( ret == SOCKET_ERROR )
		{
			HANDLE iocp = m_object->iocp->GetIOCPHandle();
			int err = WSAGetLastError();
			if ( err == WSAECONNRESET  )
			{
				m_object->iocp = NULL;
				PostQueuedCompletionStatus( iocp, 0, (DWORD)m_object->sock, (LPOVERLAPPED)m_object->recved );
				return 0;
			}
			else if ( err != WSAEWOULDBLOCK &&
				err != WSA_IO_PENDING )
			{
				m_object->iocp = NULL;
				PostQueuedCompletionStatus( iocp, err, (DWORD)m_object->sock, (LPOVERLAPPED)m_object->erroccured );
				return 0;
			}
		}
		return ret;
	}
	return 0;
}
