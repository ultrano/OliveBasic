#pragma once
#include "OvXObject.h"
#include "OvMessage.h"

OvSmartPointer_Struct(OvMessage);
OvSmartPointer_Class(OvActObject);
OvSmartPointer_Class(OvComponent);
class OvComponent : public OvXObject
{
	OvRTTI_DECL(OvComponent);
public:

	OvComponent();
	~OvComponent();

	/// Stream
	virtual void 	Serialize( OvObjectOutputStream & output ) OVERRIDE;
	virtual void 	Deserialize( OvObjectInputStream & input ) OVERRIDE;

	virtual void	Setup() {};
	virtual void	Teardown() {};
	virtual void	OnComponentMsg( OvMessage* msg );

	virtual void	Update( OvTimeTick elapsed ) = 0;

	void		 SetTarget( OvActObjectSPtr target );
	OvActObjectSPtr GetTarget() const;

private:

	OvActObject* m_target;

};

