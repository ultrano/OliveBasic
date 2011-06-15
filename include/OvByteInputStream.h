#pragma once
#include "OvInputStream.h"

OvSmartPointer_Class(OvByteInputStream);
class OvByteInputStream : public OvInputStream
{
public:
	OvByteInputStream();
	OvByteInputStream( OvByte * buf, OvSize size );

	void	Reset( OvByte * buf, OvSize size );

	virtual OvSize ReadBytes( OvByte * dest, OvSize dest_size ) OVERRIDE;

	OvByte * GetBuffer( OvSize & size );
	OvByte * GetPtr();
	OvSize	 GetSize();

private:

	OvByte *	 m_raw_buf;
	OvSize		 m_raw_buf_size;
	OvSize		 m_read_caret;
};