#include "MnScript.h"
#include "OvMemObject.h"
#include "OvSolidString.h"

class MnCallInfo;
class MnObject;
class MnString;
class MnTable;
class MnValue;
class MnArray;
class MnClosure;

enum MnCLType { CCL= 1, MCL = 2 };
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
#define DEAD (2)

#define MnMarking( v ) \
	if ( MnIsObj( (v) ) && !(MnToObject((v))->mark == MARKED)  )\
	MnToObject((v))->marking();

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
#define MnToObject( v ) (MnIsObj(v)? (v).u.cnt->u.obj : MnBadConvert())
#define MnToString( v ) (MnIsString(v)? (v).u.cnt->u.str : MnBadConvert())
#define MnToTable( v ) (MnIsTable(v)? (v).u.cnt->u.tbl : MnBadConvert())
#define MnToArray( v ) (MnIsArray(v)? (v).u.cnt->u.arr : MnBadConvert())
#define MnToClosure( v ) (MnIsClosure(v)? (v).u.cnt->u.cls: MnBadConvert())
//////////////////////////////////////////////////////////////////////////

class MnState : public OvMemObject
{
public:
	typedef OvMap<OvHash32,MnValue> map_hash_val;
	typedef OvMap<OvHash32,MnString*> map_hash_str;
	typedef OvVector<MnValue>	vec_stack;

	MnObject*	 heap;
	map_hash_val global;
	map_hash_str strtable;
	vec_stack	 stack;

	MnCallInfo*	 ci;
	MnIndex		 base;
	MnIndex		 top;

};

//////////////////////////////////////////////////////////////////////////

class MnCallInfo : public OvMemObject
{
public:
	MnClosure*	cls;
	MnIndex		base;
	MnCallInfo* prev;
};

//////////////////////////////////////////////////////////////////////////
void*			 nx_alloc( OvSize sz ) { return OvMemAlloc(sz); };
void			 nx_free( void* p ) { OvMemFree(p); };

MnString*		 nx_new_string( MnState* s, const OvString& str );
MnTable*		 nx_new_table( MnState* s );
MnArray*		 nx_new_array( MnState* s );
MnClosure*		 nx_new_closure( MnState* s, MnCLType t );

void			 nx_delete_object( MnObject* o );
void			 nx_delete_garbage( MnObject* o );

MnIndex			 nx_absidx( MnState* s, MnIndex idx );

OvBool			 nx_is_global( MnState* s, OvHash32 hash );

void			 nx_set_global( MnState* s, MnValue& n, const MnValue& val );
MnValue			 nx_get_global( MnState* s, MnValue& n );


void			 nx_remove( MnState* s, MnIndex idx );
void			 nx_set_stack( MnState* s, MnIndex idx, const MnValue& val );
MnValue			 nx_get_stack( MnState* s, MnIndex idx );

void			 nx_set_table( MnState* s, MnValue& t, MnValue& n, MnValue& v );
MnValue			 nx_get_table( MnState* s, MnValue& t, MnValue& n );

void			 nx_set_array( MnState* s, MnValue& t, MnValue& n, MnValue& v );
MnValue			 nx_get_array( MnState* s, MnValue& t, MnValue& n );

void			 nx_push_value( MnState* s, const MnValue& v );

//////////////////////////////////////////////////////////////////////////

class MnRefCounter : public OvMemObject
{
public:
	MnRefCounter() : scnt(0), wcnt(0) { u.obj = NULL; };
	OvInt		scnt;
	OvInt		wcnt;
	union
	{
		MnObject* obj;
		MnString* str;
		MnTable*  tbl;
		MnArray*  arr;
		MnClosure*  cls;
	} u;
};

class MnObject : public OvMemObject
{
public:
	
	MnObject( MnState* s );
	virtual ~MnObject();

	/* field */
	MnState*const 	state;
	MnObjType		type;
	MnRefCounter*	refcnt;

	MnObject*	next;
	MnObject*	prev;

	/* we use "mark and sweep" GC algorithm*/
	OvByte		mark;

	virtual void marking() = 0;
};

MnObject::MnObject( MnState* s ) 
: state(s)
, next(NULL)
, prev(NULL)
, mark(UNMARKED)
, refcnt(new(nx_alloc(sizeof(MnRefCounter))) MnRefCounter)
{

	if ( state->heap ) state->heap->prev = this;

	next = state->heap;
	state->heap = this;

	++refcnt->wcnt;
	refcnt->u.obj = this;
}

MnObject::~MnObject()
{
	refcnt->u.obj = NULL;
	refcnt->scnt = 0;
	if ( --refcnt->wcnt == 0) nx_free( refcnt );
}

//////////////////////////////////////////////////////////////////////////

class MnString : public MnObject
{
public:

	MnString( MnState* s, OvHash32 hash, const OvString& sstr );
	~MnString();

	OvHash32		get_hash() { return m_hash; };
	const OvString& get_str() { return m_str; };

	virtual void marking();
private:
	OvHash32 m_hash;
	OvString m_str;
};

//////////////////////////////////////////////////////////////////////////

class MnTable : public MnObject
{
public:

	typedef OvPair<MnValue,MnValue> pair_val_val;
	typedef OvMap<OvHash32,pair_val_val> map_hash_pair;

	map_hash_pair table;

	MnTable( MnState* s );
	~MnTable();

	virtual void marking();

};

//////////////////////////////////////////////////////////////////////////

class MnArray : public MnObject
{
public:

	typedef OvVector<MnValue> vec_val;

	vec_val array;

	MnArray( MnState* s );
	~MnArray();

	virtual void marking();

};

//////////////////////////////////////////////////////////////////////////

class MnClosure : public MnObject
{
public:

	struct CClosure : OvMemObject
	{
		MnNative proto;
	};

	struct MClosure : OvMemObject
	{
	};
	
	MnCLType type;
	union
	{
		CClosure* ccl;
		MClosure* mcl;
	} u;

	MnClosure( MnState* s, MnCLType t );
	~MnClosure();

	virtual void marking();

};

//////////////////////////////////////////////////////////////////////////

class MnValue : public OvMemObject
{
public:
	MnObjType type;
	union
	{
		MnRefCounter* cnt;
		OvReal num;
		OvBool bln;
	} u;

	MnValue();
	MnValue( const MnValue &v );
	MnValue( OvBool b );
	MnValue( OvReal n );
	MnValue( MnObjType t, MnObject* o );
	~MnValue();

	const MnValue& operator =( const MnValue& v );

};

//////////////////////////////////////////////////////////////////////////

void	nx_inc_ref( MnValue& v )
{
	if ( MnIsObj(v) ) 
	{
		if ( MnToObject(v) ) ++v.u.cnt->scnt;
		++v.u.cnt->wcnt;
	}
}

void	nx_dec_ref( MnValue& v )
{
	if ( MnIsObj(v) )
	{
		if ( MnToObject(v) )
		{
			--v.u.cnt->scnt;
			if ( v.u.cnt->scnt == 0 && MnToObject(v)->mark != DEAD )
			{
				nx_delete_object( v.u.cnt->u.obj );
				v.u.cnt->u.obj = NULL;
			}
		}

		if ( --v.u.cnt->wcnt == 0 ) nx_free( v.u.cnt );
	}
}

MnValue::MnValue()
: type(MOT_NIL)
{

}

MnValue::MnValue( const MnValue &v )
: type(MOT_NIL)
{
	type = v.type;
	u	 = v.u;
	nx_inc_ref(*this);
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

MnValue::MnValue( MnObjType t, MnObject* o )
{
	type	 = t;
	u.cnt = o->refcnt;
	nx_inc_ref(*this);
}

MnValue::~MnValue()
{
	nx_dec_ref(*this);
}

const MnValue& MnValue::operator=( const MnValue& v )
{
	nx_dec_ref(*this);
	type = v.type;
	u	 = v.u;
	nx_inc_ref(*this);
	return *this;
}

//////////////////////////////////////////////////////////////////////////

MnString::MnString( MnState* s, OvHash32 hash,const OvString& sstr )
: MnObject(s)
, m_hash( hash )
, m_str( sstr )
{
}

MnString::~MnString()
{
	state->strtable.erase( m_hash );
}

void MnString::marking()
{
	mark = MARKED;
}

//////////////////////////////////////////////////////////////////////////

MnTable::MnTable( MnState* s )
: MnObject(s)
{

}

MnTable::~MnTable()
{
	table.clear();
}

void MnTable::marking()
{
	mark = MARKED;

	for ( map_hash_pair::iterator itor = table.begin()
		; itor != table.end()
		; ++itor )
	{
		pair_val_val& vpair = itor->second;
		MnMarking( vpair.first );
		MnMarking( vpair.second );
	}
}

//////////////////////////////////////////////////////////////////////////

MnArray::MnArray( MnState* s )
: MnObject(s)
{

}

MnArray::~MnArray()
{
	array.clear();
}

void MnArray::marking()
{
	mark = MARKED;

	for each ( const MnValue& v in array )
	{
		MnMarking( v );
	}
}

//////////////////////////////////////////////////////////////////////////

MnClosure::MnClosure( MnState* s, MnCLType t )
: MnObject(s)
, type(t)
{
	u.ccl = (CClosure*)nx_alloc( (type==MCL)? sizeof(MClosure) : sizeof(CClosure) );
}

MnClosure::~MnClosure()
{
	nx_free(u.ccl);
}

void MnClosure::marking()
{

}

////////////////////*    open and close    *///////////////////

MnState* mn_open_state()
{
	MnState* s = new(nx_alloc(sizeof(MnState))) MnState;
	s->base = 0;
	s->top  = 0;
	return s;
}

void mn_close_state( MnState* s )
{
	if ( s )
	{
		s->stack.clear();
		s->global.clear();
		s->~MnState();
		nx_free(s);
	}
}

////////////////////////*    get/set stack, field    */////////////////////////////////

void nx_set_global( MnState* s, MnValue& n, const MnValue& val )
{
	if ( MnIsString(n) )
	{
		OvHash32 hash = MnToString(n)->get_hash();
		if ( MnIsNil(val) )
		{
			s->global.erase( hash );
		}
		else
		{
			s->global.insert( make_pair( hash, val ) );

		}
	}
}

MnValue nx_get_global( MnState* s, MnValue& n )
{
	if ( MnIsString(n) )
	{
		OvHash32 hash = MnToString(n)->get_hash();
		MnState::map_hash_val::iterator itor = s->global.find( hash );
		if ( itor != s->global.end() )
		{
			return itor->second;
		}
	}
	return MnValue();
}


void nx_remove( MnState* s, MnIndex idx )
{
	idx = nx_absidx( s, idx );
	if ( idx >= 0 )
	{
		while ( ++idx < s->top ) s->stack[idx-1] = s->stack[idx];
		--s->top;
	}

}

void nx_set_stack( MnState* s, MnIndex idx, const MnValue& val )
{
	idx = nx_absidx( s, idx );
	if ( idx >= 0 && idx < s->stack.size() )
	{
		s->stack[idx] = val;
	}
}

MnValue nx_get_stack( MnState* s, MnIndex idx )
{
	idx = nx_absidx( s, idx );
	if ( idx >= 0 && idx < s->stack.size() )
	{
		return s->stack.at( idx );
	}
	else
	{
		return MnValue();
	}
}

void nx_set_table( MnState* s, MnValue& t, MnValue& n, MnValue& v )
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

MnValue nx_get_table( MnState* s, MnValue& t, MnValue& n )
{
	if ( MnIsTable(t) && MnIsString(n) )
	{
		OvHash32 hash = MnToString(n)->get_hash();
		MnTable::map_hash_pair::iterator itor = MnToTable(t)->table.find( hash );

		if ( itor !=  MnToTable(t)->table.end() )
		{
			return itor->second.second;
		}
	}
	return MnValue();
}

void nx_set_array( MnState* s, MnValue& a, MnValue& n, MnValue& v )
{
	if ( MnIsArray(a) && MnIsNumber(n) )
	{
		MnIndex idx = (MnIndex)MnToNumber(n);
		if ( idx >= 0 && idx < MnToArray(a)->array.size() )
		{
			MnToArray(a)->array[idx] = v;
		}
	}
}

MnValue nx_get_array( MnState* s, MnValue& a, MnValue& n )
{
	if ( MnIsArray(a) && MnIsNumber(n) )
	{
		MnIndex idx = (MnIndex)MnToNumber(n);
		if ( idx >= 0 && idx < MnToArray(a)->array.size() )
		{
			return MnToArray(a)->array.at(idx);
		}
	}
	return MnValue();
}

OvBool nx_is_global( MnState* s, OvHash32 hash )
{
	return ( s->global.find( hash ) != s->global.end() );
}

void mn_new_table( MnState* s )
{
	nx_push_value( s, MnValue( MOT_TABLE, nx_new_table(s) ) );
}

void mn_new_closure( MnState* s, MnNative proto )
{
	MnClosure* cl = nx_new_closure(s,CCL);
	cl->u.ccl->proto = proto;

	nx_push_value( s, MnValue(MOT_CLOSURE,cl) );
}

void mnd_new_garbege( MnState* s )
{
	MnString* str = nx_new_string(s,"f1");
	MnTable* t1 = nx_new_table(s);
	MnTable* t2 = nx_new_table(s);

	t1->table.insert(make_pair(str->get_hash(),make_pair(MnValue(MOT_STRING,str),MnValue(MOT_TABLE,t2))));
	t2->table.insert(make_pair(str->get_hash(),make_pair(MnValue(MOT_STRING,str),MnValue(MOT_TABLE,t1))));
}

void mn_set_field( MnState* s, MnIndex idx )
{
	if ( mn_get_top(s) >= 2 )
	{
		if ( idx )
		{
			MnValue c = nx_get_stack(s,idx);

			if ( MnIsTable(c) )
			{
				nx_set_table( s
					, c
					, nx_get_stack(s,-2)
					, nx_get_stack(s,-1));
			}
			else if ( MnIsArray(c) )
			{
				nx_set_array( s
					, c
					, nx_get_stack(s,-2)
					, nx_get_stack(s,-1));
			}
		}
		else
		{
			nx_set_global( s
				, nx_get_stack(s,-2)
				, nx_get_stack(s,-1));
		}

		mn_pop(s,2);
	}
}

void mn_get_field( MnState* s, MnIndex idx )
{
	if ( mn_get_top(s) >= 1 )
	{
		MnValue val;
		if ( idx )
		{
			MnValue c = nx_get_stack(s,idx);

			if ( MnIsTable(c) )
			{
				val = nx_get_table( s
					, c
					, nx_get_stack(s,-1));
			}
			else if ( MnIsArray(c) )
			{
				val = nx_get_array( s
					, c
					, nx_get_stack(s,-1));
			}
		}
		else
		{
			val = nx_get_global( s
				, nx_get_stack(s,-1));
		}

		mn_pop(s,1);

		nx_push_value( s, val );
	}
}

MnString* nx_new_string( MnState* s, const OvString& str )
{
	MnString* ret;
	OvHash32 hash = OU::string::rs_hash(str);

	MnState::map_hash_str::iterator itor = s->strtable.find( hash );
	if ( itor == s->strtable.end() )
	{
		ret = new(nx_alloc(sizeof(MnString))) MnString(s, hash, str);
		itor = s->strtable.insert( make_pair( hash, ret ) ).first;
	}
	ret = itor->second;
	return ret;
}

MnTable* nx_new_table( MnState* s )
{
	return new(nx_alloc(sizeof(MnTable))) MnTable(s);
}

MnArray* nx_new_array( MnState* s )
{
	return new(nx_alloc(sizeof(MnArray))) MnArray(s);
}

MnClosure* nx_new_closure( MnState* s, MnCLType t )
{
	return new(nx_alloc(sizeof(MnClosure))) MnClosure(s,t);
}

void nx_delete_object( MnObject* o )
{
	if ( o )
	{
		o->~MnObject();

		if (o->next) o->next->prev = o->prev;
		if (o->prev) o->prev->next = o->next;
		else o->state->heap = o->next;

		nx_free((void*)o);
	}
}

void nx_delete_garbage( MnObject* o )
{
	if ( o )
	{
		o->~MnObject();

		nx_free((void*)o);
	}
}

MnIndex nx_absidx( MnState* s, MnIndex idx )
{
	MnIndex abidx = 0;
	if ( idx < 0 )
	{
		abidx = mn_get_top(s) + idx;
		abidx = s->base + abidx;
	}
	else if ( idx >= 0 )
	{
		abidx = s->base + idx - 1;
	}
	return abidx;
}

///////////////////////* get/set top *///////////////////////

void mn_set_top( MnState* s, MnIndex idx )
{
	idx = nx_absidx( s, idx ) + 1;
	idx = (idx < s->base)? s->base : idx;
	if ( idx > s->stack.size() )
	{
		s->stack.resize( idx );
	}
	s->top = idx;
}

MnIndex mn_get_top( MnState* s )
{
	return s->top - s->base;
}

MnIndex mn_get_gtop( MnState* s )
{
	return s->top;
}

///////////////////////*   kind of push    *//////////////////////

void nx_push_value( MnState* s, const MnValue& v )
{
	MnIndex top = mn_get_top(s);
	mn_set_top( s, ++top );
	nx_set_stack( s, top, v );
}

void mn_push_nil( MnState* s )
{
	nx_push_value( s, MnValue() );
}

void mn_push_boolean( MnState* s, OvBool v )
{
	nx_push_value( s, MnValue( (OvBool)v ) );
}

void mn_push_number( MnState* s, OvReal v )
{
	nx_push_value( s, MnValue( (OvReal)v ) );
}

void mn_push_string( MnState* s, const OvString& v )
{
	nx_push_value( s, MnValue( MOT_STRING, nx_new_string( s, v ) ) );
}

/////////////////////*  all kinds of "is"    *///////////////////////////

OvBool mn_is_nil( MnState* s, MnIndex idx )
{
	return MnIsNil( nx_get_stack( s, idx ) );
}

OvBool mn_is_boolean( MnState* s, MnIndex idx )
{
	return MnIsBoolean( nx_get_stack( s, idx ) );
}

OvBool mn_is_number( MnState* s, MnIndex idx )
{
	return MnIsNumber( nx_get_stack( s, idx ) );
}

OvBool mn_is_string( MnState* s, MnIndex idx )
{
	return MnIsString( nx_get_stack( s, idx ) );
}

/////////////////////*  all kinds of "to"    *///////////////////////////

OvBool mn_to_boolean( MnState* s, MnIndex idx )
{
	MnValue val = nx_get_stack( s, idx );
	if ( MnIsBoolean(val) )
	{
		return MnToBoolean(val);
	}
	return false;
}

OvReal mn_to_number( MnState* s, MnIndex idx )
{
	MnValue val = nx_get_stack( s, idx );
	if ( MnIsNumber(val) )
	{
		return MnToNumber(val);
	}
	return 0;
}

const OvString& mn_to_string( MnState* s, MnIndex idx )
{
	MnValue val = nx_get_stack( s, idx );
	if ( MnIsString(val) )
	{
		return MnToString(val)->get_str();
	}

	static OvString empty;
	return empty;
}

void mn_call( MnState* s, OvInt nargs )
{
	MnValue v = nx_get_stack(s, -(nargs + 1) );

	if ( MnIsClosure(v) && MnToClosure(v)->type == CCL )
	{
		MnClosure* cls = MnToClosure(v);
		MnClosure::CClosure* ccl = cls->u.ccl;
		if ( ccl->proto )
		{
			MnIndex funcidx = s->top - nargs;
			MnCallInfo ici;
			MnCallInfo* ci = &ici;
			ci->cls  = cls;
			ci->base = s->base;
			ci->prev = s->ci;

			s->ci	 = ci;
			s->base  = funcidx;

			OvInt nrets = ccl->proto(s);

			s->ci	= ci->prev;
			s->top  = s->base + nrets;
			s->base = ci->base;

			nx_remove(s, funcidx - s->base );
		}

	}
}

void mn_collect_garbage( MnState* s )
{
	for ( MnState::map_hash_val::iterator itor = s->global.begin()
		; itor != s->global.end()
		; ++itor )
	{
		MnMarking(itor->second);
	}
	MnIndex idx = s->stack.size();
	while ( idx-- ) MnMarking(s->stack[idx]);

	MnObject* dead = NULL;
	MnObject* heap = s->heap;
	while ( heap )
	{
		MnObject* next = heap->next;
		heap->mark = (heap->mark == MARKED)? UNMARKED : DEAD;
		if ( heap->mark == DEAD )
		{
			if (heap->prev) heap->prev->next = heap->next;
			if (heap->next) heap->next->prev = heap->prev;
			
			heap->prev = NULL;
			heap->next = dead;
			dead = heap;
		}
		heap = next;
	}

	while ( dead )
	{
		MnObject* next = dead->next;

		nx_delete_garbage(dead);

		dead = next;
	}
}

//////////////////////////////////////////////////////////////////////////

// DOTO: 컴파일러 만들자.
