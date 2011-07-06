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
OvRTTI_IMPL( OvIntegerVal );
void OvIntegerVal::Serialize( OvObjectOutputStream & output )
{
	__super::Serialize(output);
	output.Write( val );
}

void OvIntegerVal::Deserialize( OvObjectInputStream & input )
{
	__super::Deserialize(input);
	input.Read( val );
}
