#pragma once

#include "OvOutputStream.h"

class OvIOCPObject;

class OvIOCPOutputStream : public OvOutputStream
{
	OvRTTI_DECL( OvIOCPOutputStream );

public:

	OvIOCPOutputStream( OvIOCPObject * obj );
	~OvIOCPOutputStream();

	virtual OvSize WriteBytes( OvByte * write_buf, OvSize write_size );

private:

	OvIOCPObject * m_object;

};