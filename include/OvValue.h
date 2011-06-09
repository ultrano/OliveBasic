#pragma once
#include "OvObject.h"

OvREF_POINTER(OvValue);
class OvValue : public OvObject
{
	OvRTTI_DECL( OvValue );

	/// Stream
	virtual void Serialize( OvObjectOutputStream & output ) OVERRIDE;
	virtual void Deserialize( OvObjectInputStream & input ) OVERRIDE;

};

class OvFloatVal : public OvValue
{
	OvRTTI_DECL( OvFloatVal );

	/// Stream
	virtual void Serialize( OvObjectOutputStream & output ) OVERRIDE;
	virtual void Deserialize( OvObjectInputStream & input ) OVERRIDE;

public:
	OvFloat m_val;
};

class OvIntegerVal : public OvValue
{
	OvRTTI_DECL( OvIntegerVal );

	/// Stream
	virtual void Serialize( OvObjectOutputStream & output ) OVERRIDE;
	virtual void Deserialize( OvObjectInputStream & input ) OVERRIDE;

public:
	OvInt m_val;
};