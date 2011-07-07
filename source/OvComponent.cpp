#include "OvComponent.h"
#include "OvActObject.h"
#include "OvClientMessage.h"

OvRTTI_IMPL(OvComponent);

OvComponent::OvComponent()
: m_target( NULL )
{

}

OvComponent::~OvComponent()
{

}

void OvComponent::SetTarget( OvActObjectSPtr target )
{
	if ( m_target != target )
	{
		if ( m_target )
		{
			Teardown();
			m_target->_remove_component( this );
		}

		if ( m_target = target.GetRear() )
		{
			m_target->_insert_component( this );
			Setup();
		}
	}
}

OvActObjectSPtr OvComponent::GetTarget() const
{
	return m_target;
}

void OvComponent::OnComponentMsg( OvClientMessageSPtr msg )
{

}
