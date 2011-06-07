#include "OvBufferOutputStream.h"
#include "OvBuffer.h"

OvBufferOutputStream::OvBufferOutputStream( OvBuffer * buffer )
: m_buffer( buffer )
, m_write_caret( 0 )
{

}

OvBufferOutputStream::OvBufferOutputStream( OvBufferSPtr buffer )
: m_buffer( buffer )
, m_write_caret( 0 )
{

}

OvSize OvBufferOutputStream::WriteBytes( OvByte * src, OvSize write_size )
{
	OvSize src_size = m_buffer->Size() - m_write_caret;
	if ( src_size < write_size )
	{
		OvSize increment = m_buffer->Increment();
		OvUInt times = ( write_size / increment );
		times += (write_size % increment)? 1:0;
		m_buffer->Establish( times );
	}
	if ( OvByte * buf = m_buffer->Pointer() )
	{
		memcpy( buf + m_write_caret, src, write_size );
		m_write_caret += write_size;
		return write_size;
	}
	return 0;
}

OvSize OvBufferOutputStream::Skip( OvInt offset )
{
	OvInt src_size = 0;
	if ( offset > 0 )
	{
		src_size = m_buffer->Size() - m_write_caret;
		offset = min( src_size, offset );
		if ( src_size < offset )
		{
			OvSize increment = m_buffer->Increment();
			m_buffer->Establish( ( offset / increment ) + 1 );
		}
	}
	else
	{
		src_size = m_write_caret;
		src_size = -src_size;
		offset = max( src_size, offset );
	}

	m_write_caret += offset;
	return offset;
}

void OvBufferOutputStream::Reset( OvBufferSPtr buffer )
{
	m_buffer = buffer;
	m_write_caret = 0;
}

OvBufferSPtr OvBufferOutputStream::GetBuffer()
{
	return m_buffer;
}

OvByte * OvBufferOutputStream::GetBuffer( OvSize & size )
{
	if ( m_buffer )
	{
		size = m_buffer->Size();
		return m_buffer->Pointer();
	}
	size = 0;
	return NULL;
}
