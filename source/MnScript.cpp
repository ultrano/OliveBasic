#include "MnScript.h"

class MnObject;
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

#define MnIsString( v ) ((v).h->type == MOT_STRING)
#define MnIsTable( v ) ((v).h->type == MOT_TABLE)
#define MnIsArray( v ) ((v).h->type == MOT_ARRAY)
#define MnIsStringTable( v ) ((v).h->type == MOT_STRINGTABLE)
#define MnIsFuncProto( v ) ((v).h->type == MOT_FUNCPROTO)
#define MnIsOClosure( v ) ((v).h->type == MOT_OCLOSURE)
#define MnIsCClosure( v ) ((v).h->type == MOT_CCLOSURE)
#define MnIsObj( v ) \
	( \
	MnIsString((v)) ||  \
	MnIsArray((v)) ||  \
	MnIsStringTable((v)) ||  \
	MnIsFuncProto((v)) ||  \
	MnIsOClosure((v)) ||  \
	MnIsCClosure((v)) ||  \
	MnIsTable((v)) \
	)

//////////////////////////////////////////////////////////////////////////
class MnStringTable : OvMemObject
{
	typedef OvMap<OvHash32,OvSolidString> map_hash_str;
	map_hash_str m_table;
};

//////////////////////////////////////////////////////////////////////////
class MnState : public OvObject
{
public:
	MnObject* heap;
};
//////////////////////////////////////////////////////////////////////////
class MnObject : public OvRefable
{
public:
	
	MnObject( MnState & s );
	~MnObject();

	/* field */
	MnState&	state;
	MnObjType	type;

	MnObject*	next;
	MnObject*	prev;

	/* we use "mark and sweep" GC algorithm*/
	OvByte		mark;

	virtual void marking() = 0;
	virtual void cleanup() = 0;
};

MnObject::MnObject( MnState & s ) 
: state(s)
, next(NULL)
, prev(NULL)
{
	if ( state.heap ) state.heap->prev = this;

	next = state.heap;
	state.heap = this;
}

MnObject::~MnObject()
{
	if (next) next->prev = prev;
	if (prev) prev->next = next;
	else state.heap = next;
}
//////////////////////////////////////////////////////////////////////////
OvDescSPtr( class, MnHolder );
class MnHolder : public OvRefable
{
public:
	MnObjType type;
	union
	{
		OvRefCounter* refcnt;
		OvReal num;
	} u;
};
//////////////////////////////////////////////////////////////////////////
struct MnValue : OvMemObject
{
	MnHolderSPtr h;

	MnValue();
	MnValue( MnHolderSPtr v ); // value share
	MnValue( const MnValue &v );
	~MnValue();
};

MnValue::MnValue()
: h( OvNew MnHolder )
{

}

MnValue::MnValue( MnHolderSPtr v )
: h( v )
{
	if ( MnIsObj(*this) )
	{
		h->u.refcnt->inc();
	}
}

MnValue::MnValue( const MnValue &v )
: h( OvNew MnHolder )
{
	h->type = v.h->type;
	h->u	= v.h->u;

	if ( MnIsObj(*this) )
	{
		h->u.refcnt->inc();
	}
}

MnValue::~MnValue()
{
	if ( MnIsObj(*this) )
	{
		h->u.refcnt->dec();
	}
	h = NULL;
}

//////////////////////////////////////////////////////////////////////////