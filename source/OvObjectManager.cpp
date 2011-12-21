#include "OvObjectManager.h"
#include "OvUtility.h"
#include "OvObject.h"
#include <map>

OvObjectManager::OvObjectManager()
{
}

OvObjectManager::~OvObjectManager()
{
}

OvObjectID		OvObjectManager::AllocObjectID(OvObjectWRef _pObject)
{
	OvSectionLock lock;
	m_maxID.core++;
	m_objectTable[ m_maxID ] = _pObject;

	return m_maxID;
}

void		OvObjectManager::RecallObjectID(const OvObjectID& objid)
{
	OvSectionLock lock;
	//! ������Ʈ�� ������Ʈ ���̺��� �����Ѵ�.
	m_objectTable.erase(objid);
}

OvObjectWRef			OvObjectManager::FindObject(const OvObjectID& rObjHandle)
{
	OvSectionLock lock;
	if (m_objectTable.find(rObjHandle) != m_objectTable.end())
	{
		return m_objectTable[rObjHandle];
	}
	return NULL;
}