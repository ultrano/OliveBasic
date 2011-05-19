#pragma once
#include "OvObject.h"
#include "OvValue.h"

class OvNexObject : public OvObject
{
	OvRTTI_DECL( OvNexObject );

public:

	OvNexObject();
	~OvNexObject();

	//! Stream
	virtual void Serialize( OvObjectOutputStream & output );
	virtual void Deserialize( OvObjectInputStream & input );

	void		InsertValue( const OvString & key, OvValueSPtr val );
	OvValueSPtr FindValue( const OvString & key );

private:

	typedef OvMap<OvString,OvValueSPtr> value_table;

	value_table m_value_table;

};