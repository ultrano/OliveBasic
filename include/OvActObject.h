#pragma once
#include "OvXObject.h"

OvSmartPointer_Class(OvComponent);
OvDescSPtr(struct,OvMessage);

OvSmartPointer_Class(OvActObject);
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
	template<typename T> OvSmartPointer<T> FindComponent();


	/// Tree Node
	void			GetChildrenSet( OvObjectSet& children );
	void			AttachChild( OvActObjectSPtr child );
	OvActObjectSPtr DettachChild( OvActObjectSPtr child );
	void			ClearChildren();

	/// Get/Set Parent
	void			SetParent( OvActObject* parent );
	OvActObjectSPtr GetParent();


	/// Stream
	virtual void 	Serialize( OvObjectOutputStream & output ) OVERRIDE;
	virtual void 	Deserialize( OvObjectInputStream & input ) OVERRIDE;

private:

	list_msg m_msg_list;
	OvObjectSet m_components;
	OvObjectSet m_children;
	OvActObject* m_parent;

};

template<typename T>
OvSmartPointer<T> OvActObject::FindComponent()
{
	for each ( OvObjectSPtr comp in m_components )
	{
		if ( OvIsTypeOf<T>(comp) )
		{
			return comp;
		}
	}
	return NULL;
}
