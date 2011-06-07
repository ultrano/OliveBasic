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

	//! Stream
	virtual void Serialize( OvObjectOutputStream & output );
	virtual void Deserialize( OvObjectInputStream & input );

	void		InsertValue( const OvString & key, OvValueSPtr val );
	OvValueSPtr FindValue( const OvString & key );

	OvComponentSPtr RemoveComponent( OvComponentSPtr comp );

private:

	void		_add_component( OvComponentSPtr comp );

private:

	typedef OvMap<OvString,OvValueSPtr> value_table;
	value_table m_value_table;

	typedef OvList<OvComponentSPtr>		component_list;
	component_list	m_component_list;

};