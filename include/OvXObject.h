#pragma once
#include "OvObject.h"
#include "OvValue.h"


OvREF_POINTER(OvComponent);
OvREF_POINTER(OvXObject);
class OvXObject : public OvObject
{
	OvRTTI_DECL( OvXObject );
	friend class OvComponent;

public:

	OvXObject();
	~OvXObject();

	//! Update
	virtual void Update( OvTimeTick elapsed ) = 0;

	void			InsertValue( const OvString & key, OvValueSPtr val );
	OvValueSPtr 	FindValue( const OvString & key );

	void			CopyComponentSet( OvObjectSet & comset );
	OvComponentSPtr RemoveComponent( OvComponentSPtr comp );

	//! Stream
	virtual void Serialize( OvObjectOutputStream & output );
	virtual void Deserialize( OvObjectInputStream & input );

private:

	void		_add_component( OvComponentSPtr comp );

private:

	typedef OvMap<OvString,OvValueSPtr> value_table;
	value_table m_value_table;

	OvObjectSet	m_component_set;

};