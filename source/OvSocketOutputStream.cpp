#include "OvSocketOutputStream.h"
#include "OvSocket.h"

OvRTTI_IMPL( OvSocketOutputStream );

OvSocketOutputStream::OvSocketOutputStream( OvSocketSPtr sock )
: m_socket( sock )
{

}

OvSocketOutputStream::~OvSocketOutputStream()
{

}

OvSize OvSocketOutputStream::WriteBytes( OvByte * write_buf, OvSize write_size )
{
	if ( !m_socket )
	{
		return 0;
	}

	OvSize remain = write_size;

	while ( remain )
	{
		int ret = 0;
		if ( SOCKET_ERROR == (ret = send( m_socket->GetSock(), (char*)write_buf, write_size, 0 )) )
			return 0;

		remain	  -= ret;
		write_buf += ret;
	}
	return write_size - remain;
}
