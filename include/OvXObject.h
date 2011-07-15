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
	void			InsertProp( const OvString & key, OvValueSPtr val );
	OvValueSPtr 	FindProp( const OvString & key );
	OvValueSPtr 	RemoveProp( const OvString & key );

	/// Stream
	virtual void 	Serialize( OvObjectOutputStream & output ) OVERRIDE;
	virtual void 	Deserialize( OvObjectInputStream & input ) OVERRIDE;

private:

	OvPropertyMap m_value_table;

};