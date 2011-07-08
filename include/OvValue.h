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

OvSmartPointer_Struct(OvFloatVal);
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

OvSmartPointer_Struct(OvIntVal);
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