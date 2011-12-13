#pragma once
#include "OvInputStream.h"

class OvObjectID;
OvDescSPtr(class,OvObject);
OvDescSPtr(class,OvObjectInputStream);
class OvObjectInputStream : public OvInputStream
{
	typedef OvMap<OvObjectID,OvObjectSPtr> id_obj_table;
	typedef OvList<OvPair<OvObjectID,OvObjectSPtr>> id_obj_list;
public:

	OvObjectInputStream( OvInputStreamWRef input );
	~OvObjectInputStream();

	virtual OvSize ReadBytes( OvByte * dest, OvSize dest_size ) OVERRIDE;

	OvObjectWRef ReadObject();

	OvObjectWRef Deserialize();

	void		 Flush();
private:

	OvInputStreamWRef m_input;
	id_obj_table	 m_done;
	id_obj_list		 m_yet;

};