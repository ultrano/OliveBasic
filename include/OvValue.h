#pragma once
#include "OvObject.h"

OvSmartPointer_Struct(OvValue);
struct OvValue : public OvObject
{
	OvRTTI_DECL( OvValue );

	/// Stream
	virtual void Serialize( OvObjectOutputStream & output ) OVERRIDE;
	virtual void Deserialize( OvObjectInputStream & input ) OVERRIDE;

};

struct OvFloatVal : public OvValue
{
	OvRTTI_DECL( OvFloatVal );
	OvFACTORY_OBJECT_DECL(OvFloatVal);
	OvFloatVal(factory){};
public:

	OvFloatVal() : val(0){};
	OvFloatVal( OvFloat _val ) : val(_val){};

	/// Stream
	virtual void Serialize( OvObjectOutputStream & output ) OVERRIDE;
	virtual void Deserialize( OvObjectInputStream & input ) OVERRIDE;

	OvFloat val;
};

struct OvIntVal : public OvValue
{
	OvRTTI_DECL( OvIntVal );
	OvFACTORY_OBJECT_DECL(OvIntVal);
	OvIntVal(factory){};
public:

	OvIntVal() : val(0){};
	OvIntVal( OvInt _val ) : val(_val){};

	/// Stream
	virtual void Serialize( OvObjectOutputStream & output ) OVERRIDE;
	virtual void Deserialize( OvObjectInputStream & input ) OVERRIDE;

	OvInt val;
};