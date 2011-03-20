#pragma once
#include "OvInputStream.h"

OvREF_POINTER( OvSocket );
class OvSocketInputStream : public OvInputStream
{
	OvRTTI_DECL( OvSocketInputStream );
public:

	OvSocketInputStream( OvSocketSPtr sock );
	~OvSocketInputStream();

	virtual OvSize ReadBytes( OvByte * dest, OvSize dest_size ) override;
	virtual OvSize Skip( OvSize offset ) override;

private:

	OvSocketSPtr m_socket;

};