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
	OvBool			InsertProp( const OvString & key, OvValueSPtr val );
	OvValueSPtr 	RemoveProp( const OvString & key );

	OvValueSPtr 	FindProp( const OvString & key );
	void			GetPropTable( OvPropertyTable& prop_table );

	/// Stream
	virtual void 	Serialize( OvObjectOutputStream & output ) OVERRIDE;
	virtual void 	Deserialize( OvObjectInputStream & input ) OVERRIDE;

private:

	OvPropertyTable m_prop_table;

};