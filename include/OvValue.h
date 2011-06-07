#pragma once
#include "OvObject.h"

OvREF_POINTER(OvValue);
class OvValue : public OvObject
{
	OvRTTI_DECL( OvValue );
	//! Stream
	virtual void Serialize( OvObjectOutputStream & output );
	virtual void Deserialize( OvObjectInputStream & input );
};

class OvFloatVal : public OvValue
{
	OvRTTI_DECL( OvFloatVal );
	//! Stream
	virtual void Serialize( OvObjectOutputStream & output );
	virtual void Deserialize( OvObjectInputStream & input );
public:
	OvFloat m_val;
};

class OvIntegerVal : public OvValue
{
	OvRTTI_DECL( OvIntegerVal );
	//! Stream
	virtual void Serialize( OvObjectOutputStream & output );
	virtual void Deserialize( OvObjectInputStream & input );
public:
	OvInt m_val;
};