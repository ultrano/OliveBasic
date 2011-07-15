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

void OvComponent::SetTarget( OvActObjectSPtr target )
{
	if ( m_target != target )
	{
		if ( m_target )
		{
			m_target->_remove_component( this );
		}

		if ( m_target = target.GetRear() )
		{
			m_target->_insert_component( this );
		}
	}
}

OvActObjectSPtr OvComponent::GetTarget() const
{
	return m_target;
}

void OvComponent::OnComponentMsg( OvMessage* msg )
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

	m_target = (OvActObject*)input.ReadObject();

}
