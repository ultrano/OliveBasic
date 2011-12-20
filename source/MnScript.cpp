#include "MnScript.h"
#include "OvSolidString.h"

class MnObject;
class MnString;
class MnValue;

enum MnObjType
{
	MOT_NIL,
	MOT_BOOLEAN,
	MOT_NUMBER,
	MOT_STRING,
	MOT_TABLE,
	MOT_ARRAY,
	MOT_FUNCPROTO,
	MOT_CLOSURE,
	MOT_USER,
};

//////////////////////////////////////////////////////////////////////////

#define MARKED (1)
#define UNMARKED (0)
#define DYING (-1)

#define MnIsNil( v ) ((v).type == MOT_NIL)
#define MnIsBoolean( v ) ((v).type == MOT_BOOLEAN)
#define MnIsNumber( v ) ((v).type == MOT_NUMBER)
#define MnIsString( v ) ((v).type == MOT_STRING)
#define MnIsTable( v ) ((v).type == MOT_TABLE)
#define MnIsArray( v ) ((v).type == MOT_ARRAY)
#define MnIsFuncProto( v ) ((v).type == MOT_FUNCPROTO)
#define MnIsClosure( v ) ((v).type == MOT_CLOSURE)
#define MnIsObj( v ) \
	( \
	MnIsString((v)) ||  \
	MnIsArray((v)) ||  \
	MnIsFuncProto((v)) ||  \
	MnIsClosure((v)) ||  \
	MnIsTable((v)) \
	)


#define MnToNumber( v ) ((MnIsNumber(v)? (v).u.num : 0))
#define MnToString( v ) ((MnString*)(MnIsString(v)? (v).u.refcnt->getref() : NULL))
//////////////////////////////////////////////////////////////////////////

class MnState : public OvRefable
{
public:
	typedef OvMap<OvHash32,MnValue> map_hash_val;
	typedef OvMap<OvHash32,OvWRef<MnString>> map_hash_str;
	typedef OvVector<MnValue>	vec_stack;

	MnObject*	 heap;
	map_hash_val field;
	map_hash_str strtable;
	vec_stack	 stack;
	MnIndex		 top;

};

OvSPtr<MnString> mn_new_string( OvWRef<MnState> s, const OvString& str );

MnValue			 mn_stack_val( OvWRef<MnState> s, MnIndex idx );
MnValue			 mn_field_val( OvWRef<MnState> s, OvHash32 hash );
OvBool			 mn_isfield( OvWRef<MnState> s, OvHash32 hash );

void	mn_push( OvWRef<MnState> s, const MnValue& v );

//////////////////////////////////////////////////////////////////////////

class MnObject : public OvRefable
{
public:
	
	MnObject( OvWRef<MnState> s );
	~MnObject();

	/* field */
	OvWRef<MnState> state;
	MnObjType	type;

	MnObject*	next;
	MnObject*	prev;

	/* we use "mark and sweep" GC algorithm*/
	OvByte		mark;

	virtual void marking() = 0;
	virtual void cleanup() = 0;
};

MnObject::MnObject( OvWRef<MnState> s ) 
: state(s)
, next(NULL)
, prev(NULL)
{
	if ( state->heap ) state->heap->prev = this;

	next = state->heap;
	state->heap = this;
}

MnObject::~MnObject()
{
	if (next) next->prev = prev;
	if (prev) prev->next = next;
	else state->heap = next;
}

//////////////////////////////////////////////////////////////////////////

class MnString : public MnObject
{
public:

	MnString( OvWRef<MnState> s, OvHash32 hash, const OvString& sstr );

	OvHash32		get_hash() { return hash; };
	const OvString& get_str() { return str; };

	virtual void marking();
	virtual void cleanup();
private:
	OvHash32 hash;
	OvString str;
};

MnString::MnString( OvWRef<MnState> s, OvHash32 hash,const OvString& sstr )
: MnObject(s)
, hash( hash )
, str( sstr )
{
}

void MnString::marking()
{
	mark = MARKED;
}

void MnString::cleanup()
{
	state->strtable.erase( hash );
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
	MnValue( OvReal n );
	MnValue( MnObjType t, OvSPtr<MnObject> o );
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

MnValue::MnValue( OvReal n )
{
	type = MOT_NUMBER;
	u.num = n;
}

MnValue::MnValue( MnObjType t, OvSPtr<MnObject> o )
{
	type	 = t;
	u.refcnt = o.refcnt;
	MnRefInc(*this);
}

MnValue::~MnValue()
{
	MnRefDec(*this);
}

////////////////////*    open and close    *///////////////////

OvSPtr<MnState> mn_open_state()
{
	OvSPtr<MnState> s = OvNew MnState;
	s->top = 0;
	return s;
}

void mn_close_state( OvWRef<MnState> s )
{
	if ( s )
	{
		OvDelete s.get_real();
	}
}

////////////////////////*    get/set stack, field    */////////////////////////////////

MnValue mn_stack_val( OvWRef<MnState> s, MnIndex idx )
{
	if (idx < 0 && (s->top + idx) >= 0 )
	{
		return s->stack.at( s->top + idx );
	}
	else if (idx > 0 && (s->top - idx) >= 0 )
	{
		return s->stack.at( idx );
	}
	else
	{
		return MnValue();
	}
}

MnValue mn_field_val( OvWRef<MnState> s, OvHash32 hash )
{
	MnState::map_hash_val::iterator itor = s->field.find( hash );
	if ( itor != s->field.end() )
	{
		return itor->second;
	}
	return MnValue();
}

OvBool mn_isfield( OvWRef<MnState> s, OvHash32 hash )
{
	return ( s->field.find( hash ) != s->field.end() );
}

void mn_set_field( OvWRef<MnState> s, MnIndex idx )
{
	MnValue name = mn_stack_val( s, idx );
	MnValue val  = mn_stack_val( s, idx + 1 );

	if ( MnIsString(name) )
	{
		s->field.insert( make_pair( MnToString(name)->get_hash(), val ) );
	}
}

void mn_get_field( OvWRef<MnState> s, MnIndex idx )
{
	MnValue name = mn_stack_val( s, idx );

	if ( MnIsString(name) )
	{
		mn_push( s, mn_field_val( s, MnToString(name)->get_hash() ) );
	}
	else
	{
		mn_push( s, MnValue() );
	}
}

OvSPtr<MnString> mn_new_string( OvWRef<MnState> s, const OvString& str )
{
	OvSPtr<MnString> ret;

	OvHash32 hash = OU::string::rs_hash(str);
	if ( !mn_isfield( s, hash ) )
	{
		s->strtable.insert( make_pair( hash, OvNew MnString(s, hash, str) ) );
	}
	ret = s->strtable[hash];
	return ret;
}

///////////////////////*   kind of push    *//////////////////////

void mn_push( OvWRef<MnState> s, const MnValue& v )
{
	s->stack.push_back( v );
	++s->top;
}

void mn_push_number( OvWRef<MnState> s, OvReal v )
{
	mn_push( s, MnValue( v ) );
}

void mn_push_string( OvWRef<MnState> s, const OvString& v )
{
	mn_push( s, MnValue( MOT_STRING, mn_new_string( s, v ) ) );
}

/////////////////////*  all kinds of is    *///////////////////////////

OvBool mn_is_number( OvWRef<MnState> s, MnIndex idx )
{
	return MnIsNumber( mn_stack_val( s, idx ) );
}

OvBool mn_is_string( OvWRef<MnState> s, MnIndex idx )
{
	return MnIsString( mn_stack_val( s, idx ) );
}

/////////////////////*  all kinds of to    *///////////////////////////

OvReal mn_to_number( OvWRef<MnState> s, MnIndex idx )
{
	MnValue val = mn_stack_val( s, idx );
	if ( MnIsNumber(val) )
	{
		return MnToNumber(val);
	}
	return 0;
}

const OvString& mn_to_string( OvWRef<MnState> s, MnIndex idx )
{
	MnValue val = mn_stack_val( s, idx );
	if ( MnIsString(val) )
	{
		return MnToString(val)->get_str();
	}

	static OvString empty;
	return empty;
}

//////////////////////////////////////////////////////////////////////////

// DOTO: 컴파일러 만들자.
