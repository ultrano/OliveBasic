#pragma once
#include "OvXObject.h"
#include "OvMessage.h"

OvDescSPtr(struct,OvMessage);
OvDescSPtr(class,OvActObject);
OvDescSPtr(class,OvComponent);
class OvComponent : public OvXObject
{
	OvRTTI_DECL(OvComponent);
	friend class OvActObject;
public:

	OvComponent();
	~OvComponent();

	/// Stream
	virtual void 	Serialize( OvObjectOutputStream & output ) OVERRIDE;
	virtual void 	Deserialize( OvObjectInputStream & input ) OVERRIDE;

	virtual void	Setup() {};
	virtual void	Teardown() {};
	virtual void	OnComponentMsg( OvMessageWRef msg );

	virtual void	Update( OvTimeTick elapsed ) = 0;

	OvActObjectWRef GetTarget() const;
	void			RemoveFromTarget();

private:

	void		 _set_target( OvActObjectWRef target );

private:

	OvActObjectWRef m_target;

};

