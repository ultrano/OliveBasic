#pragma once
#include "OvObject.h"

OvDescSPtr(class,OvXObject);
class OvXObject : public OvObject
{
	OvRTTI_DECL( OvXObject );

public:

	/// Construct & Destruct
	OvXObject();
	~OvXObject();

	/// Stream
	virtual void 	Serialize( OvObjectOutputStream & output ) OVERRIDE;
	virtual void 	Deserialize( OvObjectInputStream & input ) OVERRIDE;

};