#include "OvObjectOutputStream.h"
#include "OvObject.h"

OvObjectOutputStream::OvObjectOutputStream( OvOutputStream* output )
: m_output( output )
{

}

OvObjectOutputStream::~OvObjectOutputStream()
{

}

OvSize OvObjectOutputStream::WriteBytes( OvByte * write_buf, OvSize write_size )
{
	if ( m_output )
	{
		return m_output->WriteBytes( write_buf, write_size );
	}
	return 0;
}

OvBool OvObjectOutputStream::WriteObject( OvObjectSPtr obj )
{
	OvObjectID objID = OvObjectID::INVALID;
	OvString type_name = "";
	if ( obj )
	{
		if ( m_serialized_done.find( obj ) == m_serialized_done.end() )
		{
			m_serialized_yet.insert( obj );
		}
		objID = obj->GetObjectID();
		type_name = OvTypeName( obj );
	}
	Write( objID );
	Write( type_name );
	return !!obj;
}

OvBool OvObjectOutputStream::Serialize( OvObjectSPtr obj )
{
	WriteObject( obj );

	OvSet<OvObjectSPtr> copy_targets;
	while ( m_serialized_yet.size() )
	{
		copy_targets = m_serialized_yet;
		m_serialized_yet.clear();
		for each( OvObjectSPtr target in copy_targets )
		{
			target->Serialize( *this );
			m_serialized_done.insert( target );
		}
	}
	return true;
}
