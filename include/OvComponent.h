#pragma once
#include "OvXObject.h"

OvSmartPointer_Class(OvClientMessage);
OvSmartPointer_Class(OvActObject);
OvSmartPointer_Class(OvComponent);
class OvComponent : public OvXObject
{
	OvRTTI_DECL(OvComponent);
public:

	OvComponent();
	~OvComponent();

	virtual void	Setup() {};
	virtual void	Teardown() {};
	virtual void	OnComponentMsg( OvClientMessageSPtr msg );

	virtual void	Update( OvTimeTick elapsed ) = 0;

	void		 SetTarget( OvActObjectSPtr target );
	OvActObjectSPtr GetTarget() const;

private:

	OvActObject* m_target;

};

