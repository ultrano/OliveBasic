#pragma once
#include "OvXObject.h"

OvSmartPointer_Class(OvComponent);

OvSmartPointer_Class(OvActObject);
class OvActObject :  public OvXObject
{
	OvRTTI_DECL(OvActObject);
	OvFACTORY_OBJECT_DECL(OvActObject);

	friend class OvComponent;
public:

	OvActObject(factory);

	virtual void Update( OvFloat elapsed );

	void	GetComponentSet( OvObjectSet& components );

	void			AttachChild( OvActObjectSPtr child );
	OvActObjectSPtr DettachChild( OvActObjectSPtr child );
	OvActObjectSPtr GetParent();

	/// Stream
	virtual void 	Serialize( OvObjectOutputStream & output ) OVERRIDE;
	virtual void 	Deserialize( OvObjectInputStream & input ) OVERRIDE;

private:

	void	_set_parent( OvActObject* parent );
	void	_insert_component( OvComponentSPtr comp );
	void	_remove_component( OvComponentSPtr comp );

private:

	OvObjectSet m_components;
	OvObjectSet m_children;
	OvActObject* m_parent;

};