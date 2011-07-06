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

	for each( OvComponentSPtr com in components )
	{
		com->Update( elapsed );
	}
}

void OvActObject::_insert_component( OvComponentSPtr comp )
{
	if ( comp )
	{
		m_components.insert( comp );
		comp->Setup();
	}
}

void OvActObject::_remove_component( OvComponentSPtr comp )
{
	if ( comp )
	{
		comp->Teardown();
		m_components.erase( comp );
	}
}

void OvActObject::GetComponentSet( OvObjectSet& components )
{
	components = m_components;
}
