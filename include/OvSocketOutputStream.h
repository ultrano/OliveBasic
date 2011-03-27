#pragma once
#include "OvOutputStream.h"

class OvSocketOutputStream : public OvOutputStream
{
	OvRTTI_DECL( OvSocketOutputStream );
public:

	OvSocketOutputStream( SOCKET sock );
	~OvSocketOutputStream();

	virtual OvSize WriteBytes( OvByte * write_buf, OvSize write_size ) override;

private:

	SOCKET m_socket;

};