#pragma once
#include "OvXObject.h"

OvSmartPointer_Class(OvComponent);

OvSmartPointer_Class(OvActObject);
class OvActObject :  public OvXObject
{
	OvRTTI_DECL(OvActObject);
	friend class OvComponent;
public:

	virtual void Update( OvFloat elapsed );

	void	GetComponentSet( OvObjectSet& components );

private:

	void	_insert_component( OvComponentSPtr comp );
	void	_remove_component( OvComponentSPtr comp );

private:

	OvObjectSet m_components;

};