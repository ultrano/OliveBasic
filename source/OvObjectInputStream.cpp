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

	OvObjectSPtr obj = NULL;
	if ( oldID != OvObjectID::INVALID )
	{

		for each ( id_obj_list::value_type val in m_deserialized_yet )
		{
			if ( val.first == oldID )
			{
				obj = val.second;
				break;
			}
		}

		if ( ! obj )
		{

			id_obj_table::iterator done = m_deserialized_done.find( oldID );
			if ( done != m_deserialized_done.end() )
			{
				obj = done->second;
			}
			else
			{
				Read( type_name );
				if ( obj = OvCreateObject( type_name ) )
				{
					m_deserialized_yet.push_back( make_pair( oldID, obj ) );
				}
			}
		}
	}
	return obj.GetRear();
}

OvObject* OvObjectInputStream::Deserialize()
{
	OvObject* root = ReadObject();

	id_obj_list copy_table;
	while ( m_deserialized_yet.size() )
	{
		copy_table = m_deserialized_yet;
		m_deserialized_yet.clear();
		for each ( id_obj_list::value_type val in copy_table )
		{
			OvObjectID oldID = val.first;
			OvObjectSPtr obj = val.second;
			if ( oldID != OvObjectID::INVALID && obj )
			{
				obj->Deserialize( *this );
				m_deserialized_done[ oldID ] = obj;
			}
		}
	}
	return root;
}
