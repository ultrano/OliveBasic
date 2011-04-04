#pragma once

#include "OvInputStream.h"

class OvIOCPObject;

class OvIOCPInputStream : public OvInputStream
{
	OvRTTI_DECL(OvIOCPInputStream);
public:

	OvIOCPInputStream( OvIOCPObject * obj );
	~OvIOCPInputStream();

	virtual OvSize ReadBytes( OvByte * dest, OvSize dest_size ) override;

private:

	OvIOCPObject * m_object;

};