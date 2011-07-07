#include "OvObjectInputStream.h"
#include "OvObject.h"

OvObjectInputStream::OvObjectInputStream( OvInputStream* input )
: m_input( input )
{

}

OvObjectInputStream::~OvObjectInputStream()
{

}

OvSize OvObjectInputStream::ReadBytes( OvByte * dest, OvSize dest_size )
{
	if ( m_input )
	{
		return m_input->ReadBytes( dest, dest_size );
	}
	return 0;
}

OvObject* OvObjectInputStream::ReadObject()
{
	OvObjectID	oldID;
	OvString	type_name;
	Read( oldID );
	Read( type_name );

	OvObjectSPtr obj = NULL;
	if ( oldID != OvObjectID::INVALID )
	{
		id_obj_table::iterator itor = m_deserialized_done.find( oldID );
		if ( itor != m_deserialized_done.end() )
		{
			obj = itor->second;
		}
		else
		{
			if ( obj = OvCreateObject( type_name ) )
			{
				m_deserialized_yet[ oldID ] = obj;
			}
		}
	}
	return obj.GetRear();
}

OvObject* OvObjectInputStream::Deserialize()
{
	OvObject* root = ReadObject();

	id_obj_table copy_table;
	while ( m_deserialized_yet.size() )
	{
		copy_table = m_deserialized_yet;
		m_deserialized_yet.clear();
		for each ( const id_obj_table::value_type val in copy_table )
		{
			if ( OvObject* obj = val.second.GetRear() )
			{
				obj->Deserialize( *this );
				m_deserialized_done[val.first] = val.second ;
			}
		}
	}
	return root;
}
