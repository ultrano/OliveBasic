#include "OvObject.h"
#include "OvObjectManager.h"
#include "OvUtility.h"
#include "OvBuffer.h"
#include "MnScript.h"

using namespace std;

OvRTTI_IMPL(OvObject);

OvObject::OvObject()
: m_state( mn_openstate() )
{
	m_idObjectID = OvObjectManager::GetInstance()->AllocObjectID(this);
}
OvObject::~OvObject()
{
	mn_closestate( m_state );
	OvObjectManager::GetInstance()->RecallObjectID( GetObjectID() );
}

OvObjectID		OvObject::GetObjectID()
{
	return m_idObjectID;
}

MnState*		OvObject::GetState()
{
	return m_state;
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
