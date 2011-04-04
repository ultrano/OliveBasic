#include <WinSock2.h>
#include "OvIOCPOutputStream.h"
#include "OvIOCP.h"
#include "OvIOCPObject.h"

OvRTTI_IMPL(OvIOCPOutputStream);

OvIOCPOutputStream::OvIOCPOutputStream( OvIOCPObject * obj )
: m_object( obj )
{

}

OvIOCPOutputStream::~OvIOCPOutputStream()
{
	m_object = NULL;
}

OvSize OvIOCPOutputStream::WriteBytes( OvByte * write_buf, OvSize write_size )
{
	if ( ( m_object && m_object->sock != INVALID_SOCKET ) && (write_buf && write_size) )
	{
		WSABUF wsabuf = {write_size, (char*)write_buf};
		int ret = WSASend( m_object->sock
			, &(wsabuf)
			, 1
			, (DWORD*)&write_size
			, 0
			, (LPOVERLAPPED)(m_object->sended)
			, NULL );
		if ( ret == SOCKET_ERROR && m_object->iocp )
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
