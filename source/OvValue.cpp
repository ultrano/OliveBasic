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