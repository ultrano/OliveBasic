#pragma once
#include "OvOutputStream.h"

OvSmartPointer_Class(OvObject);
class OvObjectOutputStream : public OvOutputStream
{
public:

	OvObjectOutputStream( OvOutputStream* output );
	~OvObjectOutputStream();

	virtual OvSize WriteBytes( OvByte * write_buf, OvSize write_size ) OVERRIDE;

	OvBool	WriteObject( OvObjectSPtr obj );

	OvBool	Serialize( OvObjectSPtr obj );

private:

	OvOutputStream* m_output;

	OvSet<OvObjectSPtr>	m_serialize_targets;
	OvSet<OvObjectSPtr>	m_serialized;

};