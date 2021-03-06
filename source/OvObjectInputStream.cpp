#include "OvObjectInputStream.h"
#include "OvObject.h"

OvObjectInputStream::OvObjectInputStream( OvInputStreamWRef input )
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

OvObjectWRef OvObjectInputStream::ReadObject()
{
	OvObjectID	oldID;
	OvString	type_name;

	Read( oldID );

	OvObjectSPtr obj = NULL;
	if ( oldID != OvObjectID::INVALID )
	{

		for each ( id_obj_list::value_type val in m_yet )
		{
			if ( val.first == oldID )
			{
				obj = val.second;
				break;
			}
		}

		if ( ! obj )
		{
			id_obj_table::iterator done = m_done.find( oldID );
			if ( done != m_done.end() )
			{
				obj = done->second;
			}
			else
			{
				Read( type_name );
				if ( obj = OvCreateObject( type_name ) )
				{
					m_yet.push_back( make_pair( oldID, obj ) );
				}
			}
		}
	}
	return obj;
}

OvObjectWRef OvObjectInputStream::Deserialize()
{
	OvObjectWRef root = ReadObject();

	id_obj_list copy_table;
	while ( m_yet.size() )
	{
		copy_table = m_yet;
		m_yet.clear();
		for each ( id_obj_list::value_type val in copy_table )
		{
			OvObjectID oldID = val.first;
			OvObjectSPtr obj = val.second;
			if ( oldID != OvObjectID::INVALID && obj )
			{
				obj->Deserialize( *this );
				m_done[ oldID ] = obj;
			}
		}
	}
	return root;
}

void OvObjectInputStream::Flush()
{
	m_input = NULL;
	m_done.clear();
	m_yet.clear();
}
