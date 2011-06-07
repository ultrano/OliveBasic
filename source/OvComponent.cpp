#include "OvComponent.h"
#include "OvXObject.h"

OvRTTI_IMPL(OvComponent);

void OvComponent::SetTarget( OvXObjectSPtr target )
{
	if ( m_target )
	{
		m_target->RemoveComponent( this );
	}
	if ( target )
	{
		target->_add_component( this );
	}
	m_target = target;
}

OvXObjectSPtr OvComponent::GetTarget()
{
	return m_target;
}
