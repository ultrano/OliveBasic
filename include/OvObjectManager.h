#pragma once
#include "OvSingleton.h"
#include "OvUtility.h"
#include "OvObjectID.h"
#include "OvRefable.h"
#include <map>

OvDescSPtr(class,OvObject);

class OvObjectManager : public OvSingleton< OvObjectManager >, public OvThreadSyncer< OvObjectManager >
{
	OvSINGLETON(OvObjectManager);
	friend class OvObject;
	typedef std::map<OvObjectID,OvObjectWRef> object_table;

public:

	OvObjectManager();
	~OvObjectManager();

	OvObjectWRef	FindObject(const OvObjectID& rObjHandle);

protected:

	OvObjectID		AllocObjectID(OvObjectWRef _pObject);
	void			RecallObjectID(OvObjectWRef _pObject);

private:
	OvObjectID		m_maxID;
	object_table	m_objectTable;
};