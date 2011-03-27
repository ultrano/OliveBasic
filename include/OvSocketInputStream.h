#pragma once
#include "OvInputStream.h"

class OvSocketInputStream : public OvInputStream
{
	OvRTTI_DECL( OvSocketInputStream );
public:

	OvSocketInputStream( SOCKET sock );
	~OvSocketInputStream();

	virtual OvSize ReadBytes( OvByte * dest, OvSize dest_size ) override;

private:

	SOCKET m_socket;

};