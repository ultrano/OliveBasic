#include "OvXObject.h"

OvRTTI_IMPL( OvXObject );

OvXObject::OvXObject()
{

}

OvXObject::~OvXObject()
{

}

void OvXObject::Serialize( OvObjectOutputStream & output )
{
	__super::Serialize( output );

}

void OvXObject::Deserialize( OvObjectInputStream & input )
{
	__super::Deserialize( input );

}