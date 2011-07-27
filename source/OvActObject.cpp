#include "OvActObject.h"
#include "OvComponent.h"
#include "OvMessage.h"
OvRTTI_IMPL(OvActObject);
OvFACTORY_OBJECT_IMPL(OvActObject);

OvActObject::OvActObject( factory )
{

}

OvActObject::OvActObject()
{

}

void OvActObject::Update( OvFloat elapsed )
{
	OvObjectSet components = m_components;
	if ( m_component_msg.size() )
	{
		OvObjectSet component_msg = m_component_msg;
		m_component_msg.clear();
		for each( OvMessageSPtr msg in component_msg )
		{
			for each( OvComponentSPtr comp in components )
			{
				comp->OnComponentMsg( msg.GetRear() );
			}
		}
	}

	for each( OvComponentSPtr comp in components )
	{
		comp->Update( elapsed );
	}

	OvObjectSet children = m_children;

	for each( OvActObjectSPtr child in children )
	{
		child->Update( elapsed );
	}
}

void OvActObject::InsertComponent( OvComponentSPtr comp )
{
	if ( comp )
	{
		comp->_set_target( this );
		m_components.insert( comp );
		comp->Setup();
	}
}

void OvActObject::RemoveComponent( OvComponentSPtr comp )
{
	if ( comp )
	{
		comp->Teardown();
		m_components.erase( comp );
		comp->_set_target( NULL );
	}
}

void OvActObject::GetComponentSet( OvObjectSet& components )
{
	components = m_components;
}

void OvActObject::GetChildrenSet( OvObjectSet& children )
{
	children = m_children;
}

void OvActObject::AttachChild( OvActObjectSPtr child )
{
	if ( OvIsKindOf<OvActObject>(child) )
	{
		if ( this != child->GetParent())
		{
			m_children.insert( child );
			child->SetParent( this );
		}
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

void OvActObject::ClearChildren()
{
	m_children.clear();
}

OvActObjectSPtr OvActObject::GetParent()
{
	return m_parent;
}

void OvActObject::SetParent( OvActObject* parent )
{
	if ( m_parent != parent )
	{
		if ( m_parent )
		{
			m_parent->DettachChild( this );
		}

		m_parent = parent;

		if ( m_parent )
		{
			m_parent->AttachChild( this );
		}
	}
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

void OvActObject::PostComponentMsg( OvMessageSPtr msg )
{
	m_component_msg.insert( msg );
}