#include "OvValue.h"

//////////////////////////////////////////////////////////////////////////
OvRTTI_IMPL( OvValue );
void OvValue::Serialize( OvObjectOutputStream & output )
{
	__super::Serialize(output);

}

void OvValue::Deserialize( OvObjectInputStream & input )
{
	__super::Deserialize(input);

}


//////////////////////////////////////////////////////////////////////////
OvRTTI_IMPL( OvFloatVal );
OvFACTORY_OBJECT_IMPL(OvFloatVal);
void OvFloatVal::Serialize( OvObjectOutputStream & output )
{
	__super::Serialize(output);
	output.Write( val );
}

void OvFloatVal::Deserialize( OvObjectInputStream & input )
{
	__super::Deserialize(input);
	input.Read( val );
}

//////////////////////////////////////////////////////////////////////////
OvRTTI_IMPL( OvIntVal );
OvFACTORY_OBJECT_IMPL(OvIntVal);
void OvIntVal::Serialize( OvObjectOutputStream & output )
{
	__super::Serialize(output);
	output.Write( val );
}

void OvIntVal::Deserialize( OvObjectInputStream & input )
{
	__super::Deserialize(input);
	input.Read( val );
}

//////////////////////////////////////////////////////////////////////////
OvRTTI_IMPL( OvRectVal );
OvFACTORY_OBJECT_IMPL(OvRectVal);
void OvRectVal::Serialize( OvObjectOutputStream & output )
{
	__super::Serialize(output);
	output.Write( val );
}

void OvRectVal::Deserialize( OvObjectInputStream & input )
{
	__super::Deserialize(input);
	input.Read( val );
}

//////////////////////////////////////////////////////////////////////////
OvRTTI_IMPL( OvStringVal );
OvFACTORY_OBJECT_IMPL(OvStringVal);
void OvStringVal::Serialize( OvObjectOutputStream & output )
{
	__super::Serialize(output);
	output.Write( val );
}

void OvStringVal::Deserialize( OvObjectInputStream & input )
{
	__super::Deserialize(input);
	input.Read( val );
}

//////////////////////////////////////////////////////////////////////////
OvRTTI_IMPL( OvVector2Val );
OvFACTORY_OBJECT_IMPL(OvVector2Val);
void OvVector2Val::Serialize( OvObjectOutputStream & output )
{
	__super::Serialize(output);
	output.Write( val );
}

void OvVector2Val::Deserialize( OvObjectInputStream & input )
{
	__super::Deserialize(input);
	input.Read( val );
}

//////////////////////////////////////////////////////////////////////////
OvRTTI_IMPL( OvVector3Val );
OvFACTORY_OBJECT_IMPL(OvVector3Val);
void OvVector3Val::Serialize( OvObjectOutputStream & output )
{
	__super::Serialize(output);
	output.Write( val );
}

void OvVector3Val::Deserialize( OvObjectInputStream & input )
{
	__super::Deserialize(input);
	input.Read( val );
}

//////////////////////////////////////////////////////////////////////////
OvRTTI_IMPL( OvObjectIDVal );
OvFACTORY_OBJECT_IMPL(OvObjectIDVal);
void OvObjectIDVal::Serialize( OvObjectOutputStream & output )
{
	__super::Serialize(output);
	output.Write( val );
}

void OvObjectIDVal::Deserialize( OvObjectInputStream & input )
{
	__super::Deserialize(input);
	input.Read( val );
}

//////////////////////////////////////////////////////////////////////////
OvRTTI_IMPL(OvQuaternionVal);
OvFACTORY_OBJECT_IMPL(OvQuaternionVal);
void OvQuaternionVal::Serialize( OvObjectOutputStream & output )
{
	__super::Serialize(output);
	output.Write( val );
}

void OvQuaternionVal::Deserialize( OvObjectInputStream & input )
{
	__super::Deserialize(input);
	input.Read( val );
}

//////////////////////////////////////////////////////////////////////////
OvRTTI_IMPL(OvMatrixVal);
OvFACTORY_OBJECT_IMPL(OvMatrixVal);
void OvMatrixVal::Serialize( OvObjectOutputStream & output )
{
	__super::Serialize(output);
	output.Write( val );
}

void OvMatrixVal::Deserialize( OvObjectInputStream & input )
{
	__super::Deserialize(input);
	input.Read( val );
}

//////////////////////////////////////////////////////////////////////////
OvRTTI_IMPL(OvBoolVal);
OvFACTORY_OBJECT_IMPL(OvBoolVal);
void OvBoolVal::Serialize( OvObjectOutputStream & output )
{
	__super::Serialize(output);
	output.Write( val );
}

void OvBoolVal::Deserialize( OvObjectInputStream & input )
{
	__super::Deserialize(input);
	input.Read( val );
}

//////////////////////////////////////////////////////////////////////////
OvRTTI_IMPL(OvColorVal);
OvFACTORY_OBJECT_IMPL(OvColorVal);
void OvColorVal::Serialize( OvObjectOutputStream & output )
{
	__super::Serialize(output);
	output.Write( val );
}

void OvColorVal::Deserialize( OvObjectInputStream & input )
{
	__super::Deserialize(input);
	input.Read( val );
}