#pragma once
#include "OvOutputStream.h"

OvDescSPtr( class, OvBuffer );
OvDescSPtr( class, OvBufferOutputStream );
class OvBufferOutputStream : public OvOutputStream
{
public:
	OvBufferOutputStream( OvBuffer * buffer );
	OvBufferOutputStream( OvBufferSPtr buffer );

	virtual OvSize WriteBytes( OvByte * src, OvSize write_size ) OVERRIDE;
	
	OvSize	Skip( OvInt offset ) ;
	void	Reset( OvBufferSPtr buffer );

	OvBufferSPtr GetBuffer();
	OvByte *	 GetBuffer( OvSize & size );

private:
	OvBufferSPtr m_buffer;
	OvSize		 m_write_caret;
};