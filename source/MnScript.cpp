#include "MnScript.h"
#include "OvSolidString.h"

class MnObject;
class MnValue;

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

//////////////////////////////////////////////////////////////////////////

#define MnIsString( v ) ((v).type == MOT_STRING)
#define MnIsTable( v ) ((v).type == MOT_TABLE)
#define MnIsArray( v ) ((v).type == MOT_ARRAY)
#define MnIsStringTable( v ) ((v).type == MOT_STRINGTABLE)
#define MnIsFuncProto( v ) ((v).type == MOT_FUNCPROTO)
#define MnIsOClosure( v ) ((v).type == MOT_OCLOSURE)
#define MnIsCClosure( v ) ((v).type == MOT_CCLOSURE)
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

class MnState : public OvObject
{
	typedef OvMap<OvHash32,MnValue> map_hash_val;
	typedef OvMap<OvHash32,OvSolidString> map_hash_str;
	typedef OvVector<MnValue>	vec_stack;
public:

	MnObject*	 heap;
	map_hash_val global;
	map_hash_str strtable;
	vec_stack	 stack;

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

class MnString : public MnObject
{
public:

	MnString( MnState& s, const OvString& sstr );

	const OvString& get_str() { return str.str(); };

	virtual void marking();
	virtual void cleanup();
private:
	OvSolidString str;
};

MnString::MnString( MnState& s, const OvString& sstr )
: MnObject(s)
{
	OvHash32 hash = OU::string::rs_hash( sstr );
	if ( s.strtable.find(hash) == s.strtable.end() )
	{
		s.strtable.insert( make_pair(hash,sstr) );
	}
	str = s.strtable[hash];
}

void MnString::marking()
{

}

void MnString::cleanup()
{

}

//////////////////////////////////////////////////////////////////////////

class MnValue : public OvMemObject
{
public:
	MnObjType type;
	union
	{
		OvRefCounter* refcnt;
		OvReal num;
	} u;

	MnValue();
	MnValue( const MnValue &v );
	MnValue( MnObjType t, const MnObject* o );
	~MnValue();
};

MnValue::MnValue()
: type(MOT_NIL)
{

}

#define MnRefInc(v) if ( MnIsObj((v)) ) (v).u.refcnt->inc();
#define MnRefDec(v) if ( MnIsObj((v)) ) (v).u.refcnt->dec();

MnValue::MnValue( const MnValue &v )
: type(MOT_NIL)
{
	type = v.type;
	u	 = v.u;

	MnRefInc(*this);
}

MnValue::MnValue( MnObjType t, const MnObject* o )
{
	type	 = t;
	u.refcnt = o->refcnt;
	MnRefInc(*this);
}

MnValue::~MnValue()
{
	MnRefDec(*this);
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////