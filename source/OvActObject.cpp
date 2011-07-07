#include "OvActObject.h"
#include "OvComponent.h"
OvRTTI_IMPL(OvActObject);
OvFACTORY_OBJECT_IMPL(OvActObject);

OvActObject::OvActObject( factory )
{

}

void OvActObject::Update( OvFloat elapsed )
{
	OvObjectSet components = m_components;

	for each( OvComponentSPtr comp in components )
	{
		comp->Update( elapsed );
	}

	OvObjectSet children = m_children;

	for each( OvComponentSPtr child in children )
	{
		child->Update( elapsed );
	}
}

void OvActObject::_insert_component( OvComponentSPtr comp )
{
	if ( comp )
	{
		m_components.insert( comp );
	}
}

void OvActObject::_remove_component( OvComponentSPtr comp )
{
	if ( comp )
	{
		m_components.erase( comp );
	}
}

void OvActObject::GetComponentSet( OvObjectSet& components )
{
	components = m_components;
}

void OvActObject::AttachChild( OvActObjectSPtr child )
{
	if ( child )
	{
		OvActObjectSPtr old_parent = child->GetParent();
		if ( old_parent )
		{
			old_parent->DettachChild( child );
		}
		m_children.insert( child );
		child->_set_parent( this );
	}
}

OvActObjectSPtr OvActObject::DettachChild( OvActObjectSPtr child )
{
	OvActObjectSPtr ret = NULL;
	if ( child )
	{
		OvObjectSet::iterator itor = m_children.find( child );
		if ( itor != m_children.end() )
		{
			ret = *itor;
		}
		m_children.erase( child );
	}
	return ret;
}

OvActObjectSPtr OvActObject::GetParent()
{
	return m_parent;
}

void OvActObject::_set_parent( OvActObject* parent )
{
	m_parent = parent;
}

void OvActObject::Serialize( OvObjectOutputStream & output )
{
	__super::Serialize(output);

	output.Write( m_components.size() );
	for each ( OvComponentSPtr comp in m_components )
	{
		output.WriteObject( comp );
	}

	output.Write( m_children.size() );
	for each ( OvComponentSPtr child in m_children )
	{
		output.WriteObject( child );
	}

	output.WriteObject( m_parent );
}

void OvActObject::Deserialize( OvObjectInputStream & input )
{
	__super::Deserialize(input);

	OvSize count = 0;

	input.Read( count );
	while ( count-- )
	{
		m_components.insert( input.ReadObject() );
	}

	input.Read( count );
	while ( count-- )
	{
		m_children.insert( input.ReadObject() );
	}

	m_parent = (OvActObject*)input.ReadObject();
}
