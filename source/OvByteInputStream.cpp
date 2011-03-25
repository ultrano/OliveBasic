#include "OvByteInputStream.h"

OvByteInputStream::OvByteInputStream()
: m_raw_buf( NULL )
, m_raw_buf_size( 0 )
, m_read_caret( 0 )
{

}

OvByteInputStream::OvByteInputStream( OvByte * buf, OvSize size )
: m_raw_buf( buf )
, m_raw_buf_size( size )
, m_read_caret( 0 )
{

}

void OvByteInputStream::Reset( OvByte * buf, OvSize size )
{
	m_raw_buf = buf;
	m_raw_buf_size = size;
}

OvSize OvByteInputStream::ReadBytes( OvByte * dest, OvSize dest_size )
{
	if ( m_raw_buf && m_raw_buf_size )
	{
		OvSize src_size = m_raw_buf_size - m_read_caret;
		if ( OvSize cpy_size = min( src_size, dest_size ) )
		{
			memcpy( dest, m_raw_buf + m_read_caret, cpy_size );
			m_read_caret += cpy_size;
			return cpy_size;
		}
	}
	return 0;
}

OvByte * OvByteInputStream::GetBuffer( OvSize & size )
{
	size = m_raw_buf_size;
	return m_raw_buf;
}

OvByte * OvByteInputStream::GetPtr()
{
	return m_raw_buf;
}

OvSize OvByteInputStream::GetSize()
{
	return m_raw_buf_size;
}
