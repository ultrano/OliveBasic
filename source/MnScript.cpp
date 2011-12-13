#include "MnScript.h"

enum MnObjType
{
	MOT_NIL,
	MOT_BOOLEAN,
	MOT_NUMBER,
	MOT_STRING,
	MOT_TABLE,
	MOT_ARRAY,
	MOT_STRINGTABLE,
	MOT_FUNCPROTO,
	MOT_OCLOSURE,
	MOT_CCLOSURE,
	MOT_USER,
};

class MnState : public OvObject
{

};

class MnObject : public OvRefable
{
	MnObjType type;
};

OvDescSPtr( class, MnHold );
class MnHold : public OvRefable
{
public:
	MnHold() {};
	MnObjType type;
	union
	{
	} u;
};

struct MnValue : OvMemObject
{
	MnHoldSPtr hold;

	MnValue();
	MnValue( MnHoldSPtr v ); // value share
	MnValue( const MnValue &v );
	//OsValue( MnObjType vt, OsCollectable * obj );
	~MnValue();
};

MnValue::MnValue()
: hold( OvNew MnHold )
{

}

MnValue::MnValue( MnHoldSPtr v )
: hold( v )
{
}

MnValue::MnValue( const MnValue &v )
: hold( OvNew MnHold )
{
	hold->type = v.hold->type;
	hold->u	   = v.hold->u;
}

MnValue::~MnValue()
{
	hold = NULL;
}
