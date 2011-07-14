#pragma once
#include "OvXObject.h"

OvSmartPointer_Class(OvComponent);
OvSmartPointer_Struct(OvMessage);

OvSmartPointer_Class(OvActObject);
class OvActObject :  public OvXObject
{
	OvRTTI_DECL(OvActObject);
	OvFACTORY_OBJECT_DECL(OvActObject);

	friend class OvComponent;
public:

	OvActObject(factory);

	virtual void Update( OvFloat elapsed );

	/// Component
	void	GetComponentSet( OvObjectSet& components );
	void	PostComponentMsg( OvMessageSPtr msg );
	template<typename T> OvSmartPointer<T> FindComponent();


	/// Tree Node
	void			GetChildrenSet( OvObjectSet& children );
	void			AttachChild( OvActObjectSPtr child );
	OvActObjectSPtr DettachChild( OvActObjectSPtr child );
	void			ClearChildren();
	void			SetParent( OvActObject* parent );
	OvActObjectSPtr GetParent();


	/// Stream
	virtual void 	Serialize( OvObjectOutputStream & output ) OVERRIDE;
	virtual void 	Deserialize( OvObjectInputStream & input ) OVERRIDE;

private:

	void	_insert_component( OvComponentSPtr comp );
	void	_remove_component( OvComponentSPtr comp );

private:

	OvObjectSet m_component_msg;
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
