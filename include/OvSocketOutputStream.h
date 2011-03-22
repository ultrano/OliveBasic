#pragma once
#include "OvOutputStream.h"


OvREF_POINTER( OvSocket );
class OvSocketOutputStream : public OvOutputStream
{
	OvRTTI_DECL( OvSocketOutputStream );
public:

	OvSocketOutputStream( OvSocketSPtr sock );
	~OvSocketOutputStream();

	virtual OvSize WriteBytes( OvByte * write_buf, OvSize write_size ) override;

private:

	OvSocketSPtr m_socket;

};