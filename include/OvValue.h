#pragma once
#include "OvObject.h"
#include "OvRect.h"
#include "OvVector2.h"
#include "OvVector3.h"
#include "OvQuaternion.h"
#include "OvMatrix.h"

OvSmartPointer_Struct(OvValue);
typedef OvMap<OvString,OvValueSPtr> OvPropertyTable;

struct OvValue : public OvObject
{
	OvRTTI_DECL( OvValue );

	/// Stream
	virtual void Serialize( OvObjectOutputStream & output ) OVERRIDE;
	virtual void Deserialize( OvObjectInputStream & input ) OVERRIDE;

};

//////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////

OvSmartPointer_Struct(OvRectVal);
struct OvRectVal : public OvValue
{
	OvRTTI_DECL( OvRectVal );
	OvFACTORY_OBJECT_DECL(OvRectVal);
	OvRectVal(factory){};
public:

	OvRectVal() {};
	OvRectVal( const OvRect& _val ) : val(_val){};

	/// Stream
	virtual void Serialize( OvObjectOutputStream & output ) OVERRIDE;
	virtual void Deserialize( OvObjectInputStream & input ) OVERRIDE;

	OvRect val;
};

//////////////////////////////////////////////////////////////////////////

OvSmartPointer_Struct(OvStringVal);
struct OvStringVal : public OvValue
{
	OvRTTI_DECL( OvStringVal );
	OvFACTORY_OBJECT_DECL(OvStringVal);
	OvStringVal(factory){};
public:

	OvStringVal() {};
	OvStringVal( const OvString& _val ) : val(_val){};

	/// Stream
	virtual void Serialize( OvObjectOutputStream & output ) OVERRIDE;
	virtual void Deserialize( OvObjectInputStream & input ) OVERRIDE;

	OvString val;
};

//////////////////////////////////////////////////////////////////////////

OvSmartPointer_Struct(OvVector2Val);
struct OvVector2Val : public OvValue
{
	OvRTTI_DECL( OvVector2Val );
	OvFACTORY_OBJECT_DECL(OvVector2Val);
	OvVector2Val(factory){};
public:

	OvVector2Val() {};
	OvVector2Val( const OvVector2& _val ) : val(_val){};

	/// Stream
	virtual void Serialize( OvObjectOutputStream & output ) OVERRIDE;
	virtual void Deserialize( OvObjectInputStream & input ) OVERRIDE;

	OvVector2 val;
};

//////////////////////////////////////////////////////////////////////////

OvSmartPointer_Struct(OvVector3Val);
struct OvVector3Val : public OvValue
{
	OvRTTI_DECL( OvVector3Val );
	OvFACTORY_OBJECT_DECL(OvVector3Val);
	OvVector3Val(factory){};
public:

	OvVector3Val() {};
	OvVector3Val( const OvVector3& _val ) : val(_val){};

	/// Stream
	virtual void Serialize( OvObjectOutputStream & output ) OVERRIDE;
	virtual void Deserialize( OvObjectInputStream & input ) OVERRIDE;

	OvVector3 val;
};

//////////////////////////////////////////////////////////////////////////

OvSmartPointer_Struct(OvObjectIDVal);
struct OvObjectIDVal : public OvValue
{
	OvRTTI_DECL( OvObjectIDVal );
	OvFACTORY_OBJECT_DECL(OvObjectIDVal);
	OvObjectIDVal(factory){};
public:

	OvObjectIDVal() {};
	OvObjectIDVal( const OvObjectID& _val ) : val(_val){};

	/// Stream
	virtual void Serialize( OvObjectOutputStream & output ) OVERRIDE;
	virtual void Deserialize( OvObjectInputStream & input ) OVERRIDE;

	OvObjectID val;
};

//////////////////////////////////////////////////////////////////////////

OvSmartPointer_Struct(OvQuaternionVal);
struct OvQuaternionVal : public OvValue
{
	OvRTTI_DECL(OvQuaternionVal);
	OvFACTORY_OBJECT_DECL(OvQuaternionVal);
	OvQuaternionVal(factory){};
public:

	OvQuaternionVal() {};
	OvQuaternionVal( const OvQuaternion& _val ) : val(_val){};

	/// Stream
	virtual void Serialize( OvObjectOutputStream & output ) OVERRIDE;
	virtual void Deserialize( OvObjectInputStream & input ) OVERRIDE;

	OvQuaternion val;
};

//////////////////////////////////////////////////////////////////////////

OvSmartPointer_Struct(OvMatrixVal);
struct OvMatrixVal : public OvValue
{
	OvRTTI_DECL(OvMatrixVal);
	OvFACTORY_OBJECT_DECL(OvMatrixVal);
	OvMatrixVal(factory){};
public:

	OvMatrixVal() {};
	OvMatrixVal( const OvMatrix& _val ) : val(_val){};

	/// Stream
	virtual void Serialize( OvObjectOutputStream & output ) OVERRIDE;
	virtual void Deserialize( OvObjectInputStream & input ) OVERRIDE;

	OvMatrix val;
};

//////////////////////////////////////////////////////////////////////////

OvSmartPointer_Struct(OvBoolVal);
struct OvBoolVal : public OvValue
{
	OvRTTI_DECL(OvBoolVal);
	OvFACTORY_OBJECT_DECL(OvBoolVal);
	OvBoolVal(factory){};
public:

	OvBoolVal() {};
	OvBoolVal( const OvBool& _val ) : val(_val){};

	/// Stream
	virtual void Serialize( OvObjectOutputStream & output ) OVERRIDE;
	virtual void Deserialize( OvObjectInputStream & input ) OVERRIDE;

	OvBool val;
};

