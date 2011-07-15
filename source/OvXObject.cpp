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

	output.Write( m_value_table.size() );
	for each ( const OvPropertyMap::value_type & val in m_value_table )
	{
		output.Write( val.first );
		output.WriteObject( val.second );
	}

}

void OvXObject::Deserialize( OvObjectInputStream & input )
{
	__super::Deserialize( input );

	OvUInt count = 0;

	count = 0;
	input.Read( count );
	while ( count-- )
	{
		OvString key;
		input.Read( key );
		OvValueSPtr val = (OvValue*)input.ReadObject();
		m_value_table.insert( std::make_pair( key, val ) );
	}

}

void OvXObject::InsertProp( const OvString & key, OvValueSPtr val )
{
	m_value_table.insert( std::make_pair( key, val ) );
}

OvValueSPtr OvXObject::FindProp( const OvString & key )
{
	OvValueSPtr val = NULL;
	OvPropertyMap::iterator itor = m_value_table.find( key );
	if ( itor != m_value_table.end() )
	{
		val = itor->second;
	}
	return val;
}

OvValueSPtr OvXObject::RemoveProp( const OvString & key )
{
	OvValueSPtr val = FindProp( key );
	m_value_table.erase( key );
	return val;
}
