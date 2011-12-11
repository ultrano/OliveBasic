#include "OvObject.h"
#include "OvObjectManager.h"
#include "OvUtility.h"
#include "OvBuffer.h"

using namespace std;

OvRTTI_IMPL(OvObject);

OvObject::OvObject()
{
	m_idObjectID = OvObjectManager::GetInstance()->AllocObjectID(this);
}
OvObject::~OvObject()
{
	OvObjectManager::GetInstance()->RecallObjectID(this);
}

OvObjectID		OvObject::GetObjectID()
{
	return m_idObjectID;
}

void OvObject::Serialize( OvObjectOutputStream & output )
{
// 	output.Write( GetObjectID() );
// 	output.Write( OvTypeName( this ) );
}

void OvObject::Deserialize( OvObjectInputStream & input )
{
// 	OvObjectID	oldID;
// 	OvString	type_name;
// 
// 	input.Read( oldID );
// 	input.Read( type_name );
}
