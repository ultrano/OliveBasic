#include "OvXObject.h"
#include "OvComponent.h"

OvRTTI_IMPL( OvXObject );

void OvXObject::Serialize( OvObjectOutputStream & output )
{
	__super::Serialize( output );

	output.Write( m_value_table.size() );
	for each ( const value_table::value_type & val in m_value_table )
	{
		output.Write( val.first );
		output.WriteObject( val.second );
	}

	output.Write( m_component_list.size() );
	for each ( OvComponentSPtr comp in m_component_list )
	{
		output.WriteObject( comp );
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

	count = 0;
	input.Read( count );
	while ( count-- )
	{
		OvComponentSPtr comp = (OvComponent*)input.ReadObject();
		m_component_list.push_back( comp );
	}
}

void OvXObject::InsertValue( const OvString & key, OvValueSPtr val )
{
	m_value_table.insert( std::make_pair( key, val ) );
}

OvValueSPtr OvXObject::FindValue( const OvString & key )
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

void OvXObject::_add_component( OvComponentSPtr comp )
{
	m_component_list.push_back( comp );
}

OvComponentSPtr OvXObject::RemoveComponent( OvComponentSPtr comp )
{
	component_list::iterator itor = OU::container::find( m_component_list, comp );
	if ( itor != m_component_list.end() )
	{
		return *itor;
	}
	return NULL;
}
