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
	OvString type_name;
	if ( obj )
	{
		objID = obj->GetObjectID();
		OvSet<OvObjectSPtr>::iterator done = m_done.find( obj );	//< 완료 목록
		OvList<OvObjectSPtr>::iterator yet  = OU::container::find( m_yet, obj );		//< 대기 목록
		if (( done == m_done.end() ) && ( yet == m_yet.end() ))
		{
			type_name = OvTypeName( obj );
			m_yet.push_back( obj );
		}
	}
	Write( objID );
	if ( type_name.size() )
	{
		Write( type_name );
	}
	return !!obj;
}

OvBool OvObjectOutputStream::Serialize( OvObjectSPtr obj )
{
	WriteObject( obj );

	OvList<OvObjectSPtr> copy_targets;
	while ( m_yet.size() )
	{
		copy_targets = m_yet;
		m_yet.clear();
		for each( OvObjectSPtr target in copy_targets )
		{
			target->Serialize( *this );
			m_done.insert( target );
		}
	}
	return true;
}
