#include "OvBuffer.h"
OvRTTI_IMPL(OvBuffer);

OvBufferSPtr OvBuffer::CreateBuffer( OvSize capacity, OvSize increment )
{
	OvBufferSPtr buffer = OvNew OvBuffer();
	buffer->m_buf = ( capacity )? (OvByte*)OvMemAlloc(capacity) : (OvByte*)NULL;
	buffer->m_size = capacity;
	buffer->m_increment = increment;
	return buffer;
}

OvBuffer::OvBuffer()
: m_buf( NULL )
, m_size( 0 )
, m_increment( 0 )
{

}

OvBuffer::~OvBuffer()
{
	if ( m_buf )
	{
		OvMemFree( m_buf );
	}
	m_size = 0;
	m_increment = 0;
}

OvByte * OvBuffer::Pointer()
{
	return m_buf;
}

OvSize OvBuffer::Size()
{
	return m_size;
}

OvSize OvBuffer::Increment()
{
	return m_increment;
}

void OvBuffer::Establish( OvUInt times )
{
	if ( times > 0 && m_increment > 0 )
	{
		OvSize old_size = m_size;
		OvSize new_size = m_size + ( m_increment * times );

		OvByte* old_buf = m_buf;
		if ( old_buf ) OvMemFree( old_buf );

		OvByte* new_buf = (OvByte*)OvMemAlloc( new_size );
		if ( old_size ) memcpy( new_buf, old_buf, old_size );

		m_size = new_size;
		m_buf = new_buf;
	}
}

OvByte * OvBuffer::GetBuffer( OvSize& size )
{
	size = Size();
	return Pointer();
}
