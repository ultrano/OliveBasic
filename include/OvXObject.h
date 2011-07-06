#pragma once
#include "OvObject.h"
#include "OvValue.h"


OvSmartPointer_Class(OvXObject);
class OvXObject : public OvObject
{
	OvRTTI_DECL( OvXObject );

public:

	/// Construct & Destruct
	OvXObject();
	~OvXObject();

	/// Values
	void			InsertValue( const OvString & key, OvValueSPtr val );
	OvValueSPtr 	FindValue( const OvString & key );
	OvValueSPtr 	RemoveValue( const OvString & key );

	/// Stream
	virtual void 	Serialize( OvObjectOutputStream & output ) OVERRIDE;
	virtual void 	Deserialize( OvObjectInputStream & input ) OVERRIDE;

private:

	typedef OvMap<OvString,OvValueSPtr> value_table;
	value_table m_value_table;

};