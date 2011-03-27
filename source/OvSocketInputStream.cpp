#include "OvSocketInputStream.h"
#include "OliveNet.h"
#include "OvBuffer.h"
OvRTTI_IMPL( OvSocketInputStream );

OvSocketInputStream::OvSocketInputStream( SOCKET sock )
: m_socket( sock )
{

}

OvSocketInputStream::~OvSocketInputStream()
{

}

OvSize OvSocketInputStream::ReadBytes( OvByte * dest, OvSize dest_size )
{
	if ( INVALID_SOCKET == m_socket )
	{
		return 0;
	}

	OvSize remain = dest_size;

	while ( remain )
	{
		OvSize ret = 0;
		if ( SOCKET_ERROR == (ret = recv( m_socket, (char*)dest, remain, 0 )) )
			return 0;
		else if( ret == 0 )
			break;

		remain  -= ret;
		dest	+= ret;
	}
	return dest_size - remain;
}
