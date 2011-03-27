#include "OvSocketOutputStream.h"
#include "OliveNet.h"

OvRTTI_IMPL( OvSocketOutputStream );

OvSocketOutputStream::OvSocketOutputStream( SOCKET sock )
: m_socket( sock )
{

}

OvSocketOutputStream::~OvSocketOutputStream()
{

}

OvSize OvSocketOutputStream::WriteBytes( OvByte * write_buf, OvSize write_size )
{
	if ( INVALID_SOCKET == m_socket )
	{
		return 0;
	}

	OvSize remain = write_size;

	while ( remain )
	{
		int ret = 0;
		if ( SOCKET_ERROR == (ret = send( m_socket, (char*)write_buf, write_size, 0 )) )
			return 0;

		remain	  -= ret;
		write_buf += ret;
	}
	return write_size - remain;
}
