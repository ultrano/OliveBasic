#pragma once
#include "OvUtility.h"
#include "OvRefable.h"
#include "OvAutoPtr.h"
#include "OvObjectFactory.h"
#include "OvObjectID.h"
#include "OvObjectOutputStream.h"
#include "OvObjectInputStream.h"

class OvStorage;

OvDescSPtr(class,OvObject);
typedef OvSet<OvObjectSPtr> OvObjectSet;
typedef OvList<OvObjectSPtr> OvObjectList;

class OvObject : public OvRefable
{
	OvRTTI_DECL(OvObject);
private:

	OvObject( OvObject& );
	OvObject( const OvObject& );

public:

	OvObject();
	virtual ~OvObject(); 

	//! Stream
	virtual void Serialize( OvObjectOutputStream & output );
	virtual void Deserialize( OvObjectInputStream & input );

	//! ID of Object
	OvObjectID		GetObjectID();

private:

	OvObjectID		m_idObjectID;

};
