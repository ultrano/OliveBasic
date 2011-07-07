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
	Read( type_name );
	Read( oldID );

	OvObjectSPtr obj = NULL;
	if ( oldID != OvObjectID::INVALID )
	{
		id_obj_table::iterator done = m_deserialized_done.find( oldID );	//< 완료 목록
		id_obj_table::iterator yet  = m_deserialized_yet.find( oldID );		//< 대기 목록

		if ( done != m_deserialized_done.end() )
		{
			obj = done->second;
		}
		else if ( yet != m_deserialized_yet.end() )
		{
			obj = yet->second;
		}
		else if ( obj = OvCreateObject( type_name ) )
		{
			m_deserialized_yet[ oldID ] = obj;
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
