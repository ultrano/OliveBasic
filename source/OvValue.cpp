#include "OvValue.h"
OvRTTI_IMPL( OvValue );

OvRTTI_IMPL( OvFloatVal );
void OvFloatVal::Serialize( OvObjectOutputStream & output )
{
	__super::Serialize(output);
	output.Write( m_val );
}

void OvFloatVal::Deserialize( OvObjectInputStream & input )
{
	__super::Deserialize(input);
	input.Read( m_val );
}

OvRTTI_IMPL( OvIntegerVal );
void OvIntegerVal::Serialize( OvObjectOutputStream & output )
{
	__super::Serialize(output);
	output.Write( m_val );
}

void OvIntegerVal::Deserialize( OvObjectInputStream & input )
{
	__super::Deserialize(input);
	input.Read( m_val );
}
