#pragma once
#include "OvOutputStream.h"

class OvByteOutputStream : public OvOutputStream
{
public:
	OvByteOutputStream();
	OvByteOutputStream( OvByte * buf, OvSize size );

	void	Reset( OvByte * buf, OvSize size );

	virtual OvSize WriteBytes( OvByte * src, OvSize write_size ) override;

	OvByte * GetBuffer( OvSize & size );

private:
	OvByte *	 m_raw_buf;
	OvSize		 m_raw_buf_size;
	OvSize		 m_write_caret;
};
