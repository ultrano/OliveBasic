#pragma once
#include "OvInputStream.h"

class OvObjectID;
OvDescSPtr(class,OvObject);
OvDescSPtr(class,OvObjectInputStream);
class OvObjectInputStream : public OvInputStream
{
public:

	OvObjectInputStream( OvInputStream* input );
	~OvObjectInputStream();

	virtual OvSize ReadBytes( OvByte * dest, OvSize dest_size ) OVERRIDE;

	OvObjectWRef ReadObject();

	OvObjectWRef Deserialize();

private:

	OvInputStream* m_input;

	typedef OvMap<OvObjectID,OvObjectSPtr> id_obj_table;
	id_obj_table	m_done;

	typedef OvList<OvPair<OvObjectID,OvObjectSPtr>> id_obj_list;
	id_obj_list m_yet;

};