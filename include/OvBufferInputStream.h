#pragma once
#include "OvInputStream.h"

OvDescSPtr( class, OvBuffer );
OvDescSPtr( class, OvBufferInputStream );
class OvBufferInputStream : public OvInputStream
{
	OvRTTI_DECL( OvBufferInputStream );
public:

	OvBufferInputStream( OvBuffer * buffer );
	OvBufferInputStream( OvBufferSPtr buffer );

	virtual OvSize ReadBytes( OvByte * dest, OvSize dest_size ) OVERRIDE;
	
	OvSize Skip( OvInt offset );

	OvBufferSPtr GetBuffer();

private:

	OvBufferSPtr m_buffer;
	OvSize		 m_read_caret;
};