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


#define MnToBoolean( v ) ((MnIsBoolean(v)? (v).u.bln : false))
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
	map_hash_val global;
	map_hash_str strtable;
	vec_stack	 stack;
	MnIndex		 top;

};

OvSPtr<MnString> mn_new_string( OvWRef<MnState> s, const OvString& str );

void			 mn_set_stack( OvWRef<MnState> s, MnIndex idx, const MnValue& val );
MnValue			 mn_get_stack( OvWRef<MnState> s, MnIndex idx );
MnValue			 mn_global_val( OvWRef<MnState> s, OvHash32 hash );
OvBool			 mn_is_global( OvWRef<MnState> s, OvHash32 hash );

void	mn_push_value( OvWRef<MnState> s, const MnValue& v );

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
		OvBool bln;
	} u;

	MnValue();
	MnValue( const MnValue &v );
	MnValue( OvBool b );
	MnValue( OvReal n );
	MnValue( MnObjType t, OvSPtr<MnObject> o );
	~MnValue();

	const MnValue& operator =( const MnValue& v );

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

MnValue::MnValue( OvBool b )
{
	type = MOT_BOOLEAN;
	u.bln = b;
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

const MnValue& MnValue::operator=( const MnValue& v )
{
	MnRefDec(*this);
	type = v.type;
	u	 = v.u;
	MnRefInc(*this);
	return *this;
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

void mn_set_stack( OvWRef<MnState> s, MnIndex idx, const MnValue& val )
{
	MnIndex top = mn_get_top(s);
	if (idx < 0 && (top + idx) >= 0 )
	{
		s->stack[top + idx] = val;
	}
	else if (idx > 0 && top >= idx )
	{
		s->stack[idx - 1] = val;
	}
}

MnValue mn_get_stack( OvWRef<MnState> s, MnIndex idx )
{
	MnIndex top = mn_get_top(s);
	if (idx < 0 && (top + idx) >= 0 )
	{
		return s->stack.at( top + idx );
	}
	else if (idx > 0 && top >= idx )
	{
		return s->stack.at( idx - 1 );
	}
	else
	{
		return MnValue();
	}
}

MnValue mn_global_val( OvWRef<MnState> s, OvHash32 hash )
{
	MnState::map_hash_val::iterator itor = s->global.find( hash );
	if ( itor != s->global.end() )
	{
		return itor->second;
	}
	return MnValue();
}

OvBool mn_is_global( OvWRef<MnState> s, OvHash32 hash )
{
	return ( s->global.find( hash ) != s->global.end() );
}

void mn_set_global( OvWRef<MnState> s, const OvString& name )
{
	MnValue val  = mn_get_stack( s, -1 );

	mn_pop(s,1);

	if ( MnIsNil(val) )
	{
		s->global.erase( OU::string::rs_hash( name ) );
	}
	else
	{
		s->global.insert( make_pair( OU::string::rs_hash( name ), val ) );
	}
}

void mn_get_global( OvWRef<MnState> s, const OvString& name )
{
	mn_push_value( s, mn_global_val( s, OU::string::rs_hash( name ) ) );
}

OvSPtr<MnString> mn_new_string( OvWRef<MnState> s, const OvString& str )
{
	OvSPtr<MnString> ret;

	OvHash32 hash = OU::string::rs_hash(str);
	if ( !mn_is_global( s, hash ) )
	{
		s->strtable.insert( make_pair( hash, OvNew MnString(s, hash, str) ) );
	}
	ret = s->strtable[hash];
	return ret;
}

///////////////////////* get/set top *///////////////////////

void mn_set_top( OvWRef<MnState> s, MnIndex idx )
{
	MnIndex newtop = mn_get_top(s);
	if ( idx < 0 )
	{
		newtop += idx + 1;
		newtop = (newtop<0)? 0:newtop;
	}
	else if ( idx >= 0 )
	{
		if ( idx > s->stack.size() ) s->stack.resize(idx);
		newtop = idx;
	}
	s->top = newtop;
}

MnIndex mn_get_top( OvWRef<MnState> s )
{
	return s->top;
}

///////////////////////*   kind of push    *//////////////////////

void mn_push_value( OvWRef<MnState> s, const MnValue& v )
{
	MnIndex top = mn_get_top(s);
	mn_set_top( s, ++top );
	mn_set_stack( s, top, v );
}

void mn_push_boolean( OvWRef<MnState> s, OvBool v )
{
	mn_push_value( s, MnValue( (OvBool)v ) );
}

void mn_push_number( OvWRef<MnState> s, OvReal v )
{
	mn_push_value( s, MnValue( (OvReal)v ) );
}

void mn_push_string( OvWRef<MnState> s, const OvString& v )
{
	mn_push_value( s, MnValue( MOT_STRING, mn_new_string( s, v ) ) );
}

/////////////////////*  all kinds of "is"    *///////////////////////////

OvBool mn_is_boolean( OvWRef<MnState> s, MnIndex idx )
{
	return MnIsBoolean( mn_get_stack( s, idx ) );
}

OvBool mn_is_number( OvWRef<MnState> s, MnIndex idx )
{
	return MnIsNumber( mn_get_stack( s, idx ) );
}

OvBool mn_is_string( OvWRef<MnState> s, MnIndex idx )
{
	return MnIsString( mn_get_stack( s, idx ) );
}

/////////////////////*  all kinds of "to"    *///////////////////////////

OvBool mn_to_boolean( OvWRef<MnState> s, MnIndex idx )
{
	MnValue val = mn_get_stack( s, idx );
	if ( MnIsBoolean(val) )
	{
		return MnToBoolean(val);
	}
	return false;
}

OvReal mn_to_number( OvWRef<MnState> s, MnIndex idx )
{
	MnValue val = mn_get_stack( s, idx );
	if ( MnIsNumber(val) )
	{
		return MnToNumber(val);
	}
	return 0;
}

const OvString& mn_to_string( OvWRef<MnState> s, MnIndex idx )
{
	MnValue val = mn_get_stack( s, idx );
	if ( MnIsString(val) )
	{
		return MnToString(val)->get_str();
	}

	static OvString empty;
	return empty;
}

//////////////////////////////////////////////////////////////////////////

// DOTO: 컴파일러 만들자.
