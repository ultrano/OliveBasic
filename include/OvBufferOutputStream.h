#pragma once
#include "OvOutputStream.h"

OvREF_POINTER( OvBuffer );
OvREF_POINTER( OvBufferOutputStream );
class OvBufferOutputStream : public OvOutputStream
{
public:
	OvBufferOutputStream( OvBuffer * buffer );
	OvBufferOutputStream( OvBufferSPtr buffer );

	virtual OvSize WriteBytes( OvByte * src, OvSize write_size ) override;
	
	OvSize Skip( OvSize offset ) ;

	OvBufferSPtr GetBuffer();

private:
	OvBufferSPtr m_buffer;
	OvSize		 m_write_caret;
};