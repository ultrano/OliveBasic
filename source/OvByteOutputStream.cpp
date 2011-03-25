#include "OvByteOutputStream.h"

OvByteOutputStream::OvByteOutputStream()
: m_raw_buf( NULL )
, m_raw_buf_size( 0 )
, m_write_caret( 0 )
{

}

OvByteOutputStream::OvByteOutputStream( OvByte * buf, OvSize size )
: m_raw_buf( buf )
, m_raw_buf_size( size )
, m_write_caret( 0 )
{

}

void OvByteOutputStream::Reset( OvByte * buf, OvSize size )
{
	m_raw_buf = buf;
	m_raw_buf_size = size;
}

OvSize OvByteOutputStream::WriteBytes( OvByte * src, OvSize write_size )
{
	if ( m_raw_buf && m_raw_buf_size )
	{
		OvSize remain_size = 0;
		remain_size = m_raw_buf_size - m_write_caret;

		OvSize cpy_size = min( remain_size, write_size );
		if ( m_raw_buf && cpy_size )
		{
			memcpy( m_raw_buf + m_write_caret, src, cpy_size );
			m_write_caret += cpy_size;
			return cpy_size;
		}
	}
	return 0;
}

OvByte * OvByteOutputStream::GetBuffer( OvSize & size )
{
	size = m_raw_buf_size;
	return m_raw_buf;
}
