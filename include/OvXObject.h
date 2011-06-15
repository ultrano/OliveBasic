#pragma once
#include "OvObject.h"
#include "OvValue.h"


OvSmartPointer_Class(OvComponent);
OvSmartPointer_Class(OvXObject);
class OvXObject : public OvObject
{
	OvRTTI_DECL( OvXObject );
	friend class OvComponent;

public:

	/// Construct & Destruct
	OvXObject();
	~OvXObject();

	/// Update
	virtual void	Update( OvTimeTick elapsed ) = 0;

	/// Values
	void			InsertValue( const OvString & key, OvValueSPtr val );
	OvValueSPtr 	FindValue( const OvString & key );

	/// Components
	void			CopyComponentSet( OvObjectSet & comset );
	OvComponentSPtr RemoveComponent( OvComponentSPtr comp );

	/// Stream
	virtual void 	Serialize( OvObjectOutputStream & output ) OVERRIDE;
	virtual void 	Deserialize( OvObjectInputStream & input ) OVERRIDE;

private:

	void		_add_component( OvComponentSPtr comp );

private:

	typedef OvMap<OvString,OvValueSPtr> value_table;
	value_table m_value_table;

	OvObjectSet	m_component_set;

};