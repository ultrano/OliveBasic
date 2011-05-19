#include "OvNexObject.h"

OvRTTI_IMPL( OvNexObject );

void OvNexObject::Serialize( OvObjectOutputStream & output )
{
	__super::Serialize( output );

	output.Write( m_value_table.size() );
	for each ( const value_table::value_type & val in m_value_table )
	{
		output.Write( val.first );
		output.WriteObject( val.second );
	}
}

void OvNexObject::Deserialize( OvObjectInputStream & input )
{
	__super::Deserialize( input );

	OvUInt table_size = 0;
	input.Read( table_size );
	while ( table_size-- )
	{
		OvString key;
		input.Read( key );
		OvValueSPtr val = (OvValue*)input.ReadObject();
		m_value_table.insert( OU::container::makepair( key, val ) );
	}
}

void OvNexObject::InsertValue( const OvString & key, OvValueSPtr val )
{
	m_value_table.insert( OU::container::makepair( key, val ) );
}

OvValueSPtr OvNexObject::FindValue( const OvString & key )
{
	OvValueSPtr val = NULL;
	value_table::iterator itor = m_value_table.find( key );
	m_value_table;
	if ( itor != m_value_table.end() )
	{
		val = itor->second;
	}
	return val;
}
