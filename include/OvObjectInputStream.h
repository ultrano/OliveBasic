#pragma once
#include "OvInputStream.h"

class OvObjectID;
OvSmartPointer_Class(OvObject);
OvSmartPointer_Class(OvObjectInputStream);
class OvObjectInputStream : public OvInputStream
{
public:

	OvObjectInputStream( OvInputStream* input );
	~OvObjectInputStream();

	virtual OvSize ReadBytes( OvByte * dest, OvSize dest_size ) OVERRIDE;

	OvObject* ReadObject();

	OvObject*	Deserialize();

private:

	typedef OvMap<OvObjectID,OvObjectSPtr> id_obj_table;
	OvInputStream* m_input;
	id_obj_table	m_deserialized_done;
	id_obj_table	m_deserialized_yet;

};