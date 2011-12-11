#include "OvComponent.h"
#include "OvActObject.h"
#include "OvMessage.h"

OvRTTI_IMPL(OvComponent);

OvComponent::OvComponent()
: m_target( NULL )
{

}

OvComponent::~OvComponent()
{

}

void OvComponent::_set_target( OvActObjectWRef target )
{
	m_target = target;
}

OvActObjectWRef OvComponent::GetTarget() const
{
	return m_target;
}

void OvComponent::RemoveFromTarget()
{
	if ( m_target )
	{
		m_target->RemoveComponent( this );
		m_target = NULL;
	}
}

void OvComponent::OnComponentMsg( OvMessageWRef msg )
{

}

void OvComponent::Serialize( OvObjectOutputStream & output )
{
	__super::Serialize(output);

	output.WriteObject( m_target );

}

void OvComponent::Deserialize( OvObjectInputStream & input )
{
	__super::Deserialize(input);

	m_target = input.ReadObject();

}
