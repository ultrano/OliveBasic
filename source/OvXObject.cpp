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

	output.Write( m_prop_table.size() );
	for each ( const OvPropertyTable::value_type & val in m_prop_table )
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
		m_prop_table.insert( std::make_pair( key, val ) );
	}

}

void OvXObject::InsertProp( const OvString & key, OvValueSPtr val )
{
	m_prop_table.insert( std::make_pair( key, val ) );
}

OvValueSPtr OvXObject::FindProp( const OvString & key )
{
	OvValueSPtr val = NULL;
	OvPropertyTable::iterator itor = m_prop_table.find( key );
	if ( itor != m_prop_table.end() )
	{
		val = itor->second;
	}
	return val;
}

OvValueSPtr OvXObject::RemoveProp( const OvString & key )
{
	OvValueSPtr val = FindProp( key );
	m_prop_table.erase( key );
	return val;
}

void OvXObject::GetPropTable( OvPropertyTable& prop_table )
{
	if ( prop_table.size() )
	{
		for each ( const OvPropertyTable::value_type& val in prop_table )
		{
			if ( m_prop_table.end() != m_prop_table.find( val.first ) )
			{
				prop_table[val.first] = m_prop_table[val.first];
			}
		}
	}
	else
	{
		prop_table = m_prop_table;
	}
}
