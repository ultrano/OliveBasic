#pragma once
#include "OvXObject.h"

OvDescSPtr(class,OvComponent);
OvDescSPtr(struct,OvMessage);

OvDescSPtr(class,OvActObject);
class OvActObject :  public OvXObject
{
	typedef OvList<OvMessageSPtr> list_msg;
	OvRTTI_DECL(OvActObject);
	OvFACTORY_OBJECT_DECL(OvActObject);

	friend class OvComponent;
	OvActObject(factory);
public:

	OvActObject();;

	virtual void Update( OvFloat elapsed );

	/// Component
	void	InsertComponent( OvComponentSPtr comp );
	void	RemoveComponent( OvComponentSPtr comp );
	void	GetComponentSet( OvObjectSet& components );
	void	PostComponentMsg( OvMessageSPtr msg );


	/// Tree Node
	void			GetChildrenSet( OvObjectSet& children );
	void			AttachChild( OvActObjectWRef child );
	OvActObjectSPtr DettachChild( OvActObjectWRef child );
	void			ClearChildren();

	/// Get/Set Parent
	void			SetParent( OvActObjectWRef parent );
	OvActObjectSPtr GetParent();


	/// Stream
	virtual void 	Serialize( OvObjectOutputStream & output ) OVERRIDE;
	virtual void 	Deserialize( OvObjectInputStream & input ) OVERRIDE;

private:

	list_msg m_msg_list;
	OvObjectSet m_components;
	OvObjectSet m_children;
	OvActObjectWRef m_parent;

};
