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

#define MnBadConvert()	(NULL)
#define MnToBoolean( v ) ((MnIsBoolean(v)? (v).u.bln : MnBadConvert()))
#define MnToNumber( v ) ((MnIsNumber(v)? (v).u.num : MnBadConvert()))
#define MnToString( v ) ((MnString*)(MnIsString(v)? (v).u.refcnt->getref() : MnBadConvert()))
#define MnToTable( v ) ((MnTable*)(MnIsTable(v)? (v).u.refcnt->getref() : MnBadConvert()))
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

	MnIndex		 base;
	MnIndex		 top;

};

OvSPtr<MnString> nx_new_string( OvWRef<MnState> s, const OvString& str );

MnIndex			 nx_absidx( OvWRef<MnState> s, MnIndex idx );

OvBool			 nx_is_global( OvWRef<MnState> s, OvHash32 hash );

void			 nx_set_global( OvWRef<MnState> s, OvHash32 hash, const MnValue& val );
MnValue			 nx_get_global( OvWRef<MnState> s, OvHash32 hash );

void			 nx_set_stack( OvWRef<MnState> s, MnIndex idx, const MnValue& val );
MnValue			 nx_get_stack( OvWRef<MnState> s, MnIndex idx );

void			 nx_set_table( OvWRef<MnState> s, MnValue& t, MnValue& n, MnValue& v );
MnValue			 nx_get_table( OvWRef<MnState> s, MnValue& t, MnValue& n );

void			 nx_push_value( OvWRef<MnState> s, const MnValue& v );

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

	OvHash32		get_hash() { return m_hash; };
	const OvString& get_str() { return m_str; };

	virtual void marking();
	virtual void cleanup();
private:
	OvHash32 m_hash;
	OvString m_str;
};

MnString::MnString( OvWRef<MnState> s, OvHash32 hash,const OvString& sstr )
: MnObject(s)
, m_hash( hash )
, m_str( sstr )
{
}

void MnString::marking()
{
	mark = MARKED;
}

void MnString::cleanup()
{
	state->strtable.erase( m_hash );
}

//////////////////////////////////////////////////////////////////////////

class MnTable : public MnObject
{
public:

	typedef OvPair<MnValue,MnValue> pair_val_val;
	typedef OvMap<OvHash32,pair_val_val> map_hash_pair;

	MnTable( OvWRef<MnState> s );

	map_hash_pair table;

};

MnTable::MnTable( OvWRef<MnState> s )
: MnObject(s)
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
	s->base = 0;
	s->top  = 0;
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

void nx_set_global( OvWRef<MnState> s, OvHash32 hash, const MnValue& val )
{
	if ( MnIsNil(val) )
	{
		s->global.erase( hash );
	}
	else
	{
		s->global.insert( make_pair( hash, val ) );
	}
}

MnValue nx_get_global( OvWRef<MnState> s, OvHash32 hash )
{
	MnState::map_hash_val::iterator itor = s->global.find( hash );
	if ( itor != s->global.end() )
	{
		return itor->second;
	}
	return MnValue();
}

void nx_set_stack( OvWRef<MnState> s, MnIndex idx, const MnValue& val )
{
	idx = nx_absidx( s, idx );
	if ( idx < s->stack.size() )
	{
		s->stack[idx] = val;
	}
}

MnValue nx_get_stack( OvWRef<MnState> s, MnIndex idx )
{
	idx = nx_absidx( s, idx );
	MnIndex top = mn_get_top(s);
	if ( idx < s->stack.size() )
	{
		return s->stack.at( idx );
	}
	else
	{
		return MnValue();
	}
}

void nx_set_table( OvWRef<MnState> s, MnValue& t, MnValue& n, MnValue& v )
{
	if ( MnIsTable(t) && MnIsString(n) )
	{
		OvHash32 hash = MnToString(n)->get_hash();
		if ( MnIsNil(v) )
		{
			MnToTable(t)->table.erase( hash );
		}
		else
		{
			MnToTable(t)->table.insert( make_pair( hash, make_pair(n,v) ) );
		}
	}
}

MnValue nx_get_table( OvWRef<MnState> s, MnValue& t, MnValue& n )
{
	if ( MnIsTable(t) && MnIsString(n) )
	{
		OvHash32 hash = MnIsString(n)->get_hash();
		MnTable::map_hash_pair::iterator itor = MnToTable(t)->table.find( hash );

		if ( itor !=  MnToTable(t)->table.end() )
		{
			return itor->second.second;
		}
	}
	return MnValue();
}

OvBool nx_is_global( OvWRef<MnState> s, OvHash32 hash )
{
	return ( s->global.find( hash ) != s->global.end() );
}

void mn_set_global( OvWRef<MnState> s, const OvString& name )
{
	nx_set_global( s, OU::string::rs_hash( name ), nx_get_stack( s, -1 ) );
	mn_pop(s,1);
}

void mn_get_global( OvWRef<MnState> s, const OvString& name )
{
	nx_push_value( s, nx_get_global( s, OU::string::rs_hash( name ) ) );
}

void mn_set_table( OvWRef<MnState> s, MnIndex idx )
{
	if ( mn_get_top(s) >= 2 )
	{
		nx_set_table( s
			, nx_get_stack(s,idx)
			, nx_get_stack(s,-2)
			, nx_get_stack(s,-1));

		mn_pop(s,2);
	}
}

void mn_get_table( OvWRef<MnState> s, MnIndex idx )
{
	if ( mn_get_top(s) >= 1 )
	{
		MnValue val = nx_get_table( s
			, nx_get_stack(s,idx)
			, nx_get_stack(s,-1));

		mn_pop(s,1);

		nx_push_value( s, val );
	}
}

OvSPtr<MnString> nx_new_string( OvWRef<MnState> s, const OvString& str )
{
	OvSPtr<MnString> ret;

	OvHash32 hash = OU::string::rs_hash(str);
	if ( !nx_is_global( s, hash ) )
	{
		s->strtable.insert( make_pair( hash, OvNew MnString(s, hash, str) ) );
	}
	ret = s->strtable[hash];
	return ret;
}

MnIndex nx_absidx( OvWRef<MnState> s, MnIndex idx )
{
	MnIndex abidx = 0;
	if ( idx < 0 )
	{
		abidx = mn_get_top(s) + idx;
		abidx = s->base + ((abidx<0)? 0 : abidx);
	}
	else if ( idx >= 0 )
	{
		abidx = s->base + abidx - 1;
	}
	return abidx;
}

///////////////////////* get/set top *///////////////////////

void mn_set_top( OvWRef<MnState> s, MnIndex idx )
{
	idx = nx_absidx( s, idx );
	if ( idx >= s->stack.size() )
	{
		s->stack.resize( idx );
	}
	s->top = idx;
}

MnIndex mn_get_top( OvWRef<MnState> s )
{
	return s->top - s->base;
}

///////////////////////*   kind of push    *//////////////////////

void nx_push_value( OvWRef<MnState> s, const MnValue& v )
{
	MnIndex top = mn_get_top(s);
	mn_set_top( s, ++top );
	nx_set_stack( s, top, v );
}

void mn_push_boolean( OvWRef<MnState> s, OvBool v )
{
	nx_push_value( s, MnValue( (OvBool)v ) );
}

void mn_push_number( OvWRef<MnState> s, OvReal v )
{
	nx_push_value( s, MnValue( (OvReal)v ) );
}

void mn_push_string( OvWRef<MnState> s, const OvString& v )
{
	nx_push_value( s, MnValue( MOT_STRING, nx_new_string( s, v ) ) );
}

/////////////////////*  all kinds of "is"    *///////////////////////////

OvBool mn_is_boolean( OvWRef<MnState> s, MnIndex idx )
{
	return MnIsBoolean( nx_get_stack( s, idx ) );
}

OvBool mn_is_number( OvWRef<MnState> s, MnIndex idx )
{
	return MnIsNumber( nx_get_stack( s, idx ) );
}

OvBool mn_is_string( OvWRef<MnState> s, MnIndex idx )
{
	return MnIsString( nx_get_stack( s, idx ) );
}

/////////////////////*  all kinds of "to"    *///////////////////////////

OvBool mn_to_boolean( OvWRef<MnState> s, MnIndex idx )
{
	MnValue val = nx_get_stack( s, idx );
	if ( MnIsBoolean(val) )
	{
		return MnToBoolean(val);
	}
	return false;
}

OvReal mn_to_number( OvWRef<MnState> s, MnIndex idx )
{
	MnValue val = nx_get_stack( s, idx );
	if ( MnIsNumber(val) )
	{
		return MnToNumber(val);
	}
	return 0;
}

const OvString& mn_to_string( OvWRef<MnState> s, MnIndex idx )
{
	MnValue val = nx_get_stack( s, idx );
	if ( MnIsString(val) )
	{
		return MnToString(val)->get_str();
	}

	static OvString empty;
	return empty;
}

//////////////////////////////////////////////////////////////////////////

// DOTO: 컴파일러 만들자.
