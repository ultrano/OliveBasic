#include "OvComponent.h"
#include "OvNexObject.h"

OvRTTI_IMPL(OvComponent);

void OvComponent::SetTarget( OvNexObjectSPtr target )
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

OvNexObjectSPtr OvComponent::GetTarget()
{
	return m_target;
}
