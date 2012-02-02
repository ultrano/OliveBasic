#include "MnScript.h"
#include "OvMemObject.h"
#include "OvSolidString.h"
#include "OvFile.h"

class MnCallInfo;
class MnObject;
class MnString;
class MnTable;
class MnValue;
class MnArray;
class MnMFunction;
class MnClosure;
class MnUpval;
struct MnInstruction;

typedef OvUShort MnOperand;

enum MnCLType { CCL= 1, MCL = 2 };
enum MnObjType
{
	MOT_UNKNOWN,
	MOT_NIL,
	MOT_BOOLEAN,
	MOT_NUMBER,
	MOT_STRING,

	MOT_TABLE,
	MOT_ARRAY,
	MOT_FUNCPROTO,
	MOT_CLOSURE,
	MOT_UPVAL,
	MOT_USER,
};
struct MnTypeStr { MnObjType type; const char* str; };
const MnTypeStr g_type_str[] = 
{
	{ MOT_UNKNOWN, "unknown" },
	{ MOT_NIL, "nil" },
	{ MOT_BOOLEAN, "boolean" },
	{ MOT_NUMBER, "number" },
	{ MOT_STRING, "string" },

	{ MOT_TABLE, "table" },
	{ MOT_ARRAY, "array" },
	{ MOT_FUNCPROTO, "funcproto" },
	{ MOT_CLOSURE, "closure" },
	{ MOT_UPVAL, "upval" },
	{ MOT_USER, "user" },
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
#define MnIsFunction( v ) ((v).type == MOT_FUNCPROTO)
#define MnIsClosure( v ) ((v).type == MOT_CLOSURE)
#define MnIsUpval( v ) ((v).type == MOT_UPVAL)
#define MnIsObj( v ) \
	( \
	MnIsString((v)) ||  \
	MnIsArray((v)) ||  \
	MnIsFunction((v)) ||  \
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
#define MnToFunction( v ) (MnIsFunction(v)? (v).u.cnt->u.func: MnBadConvert())
#define MnToClosure( v ) (MnIsClosure(v)? (v).u.cnt->u.cls: MnBadConvert())
#define MnToUpval( v ) (MnIsUpval(v)? (v).u.cnt->u.upv: MnBadConvert())
//////////////////////////////////////////////////////////////////////////

OvBool ut_str2num( const OvString& str, OvReal &num ) 
{
	OvInt i = 0;
	OvChar c = str[i];
	if ( isdigit(c) )
	{
		OvInt mult = 10;
		if ( c == '0' )
		{
			mult = 8;
			c = str[++i];
			if ( c == 'x' || c == 'X' )
			{
				mult = 16;
				c = str[++i];
			}
		}
		bool under = false;
		while ( true )
		{
			if ( isdigit(c) ) num = (num * mult) + (c-'0'); else break;
			if ( !(i >= str.size()) ) break;
		}
		return true;
	}
	return false;
}

OvBool ut_num2str( const OvReal& num, OvString& str )
{
	str = OU::string::format( "%f", num );
	return true;
}

//////////////////////////////////////////////////////////////////////////

class MnState : public OvMemObject
{
public:
	typedef OvMap<OvHash32,MnValue>		map_hash_val;
	typedef OvMap<OvHash32,MnString*>	map_hash_str;
	typedef OvSet<MnUpval*>				set_upval;
	typedef OvVector<MnValue>			vec_value;

	MnObject*	 heap;
	map_hash_val global;
	map_hash_str strtable;
	vec_value	 stack;

	set_upval	 upvals;
	set_upval	 openeduv;

	MnCallInfo*	 ci;
	MnInstruction* pc;
	MnIndex		 base;
	MnIndex		 last;

};

//////////////////////////////////////////////////////////////////////////

class MnCallInfo : public OvMemObject
{
public:
	MnCallInfo*		prev;
	MnIndex			base;
	MnClosure*		cls;
	MnInstruction*	savepc;
};

//////////////////////////////////////////////////////////////////////////
void*			 nx_alloc( OvSize sz ) { return OvMemAlloc(sz); };
void			 nx_free( void* p ) { OvMemFree(p); };

MnString*		 nx_new_string( MnState* s, const OvString& str );
MnTable*		 nx_new_table( MnState* s );
MnArray*		 nx_new_array( MnState* s );
MnClosure*		 nx_new_Cclosure( MnState* s, MnCLType t );
MnClosure*		 nx_new_Mclosure( MnState* s );
MnMFunction*	 nx_new_function( MnState* s );

void			 nx_delete_object( MnObject* o );
void			 nx_delete_garbage( MnObject* o );

MnIndex			 nx_absidx( MnState* s, MnIndex idx );

OvBool			 nx_is_global( MnState* s, OvHash32 hash );

void			 nx_set_global( MnState* s, MnValue& n, const MnValue& val );
MnValue			 nx_get_global( MnState* s, MnValue& n );


void			 nx_set_stack( MnState* s, MnIndex idx, const MnValue& val );
MnValue			 nx_get_stack( MnState* s, MnIndex idx );

void			 nx_set_table( MnState* s, MnValue& t, MnValue& n, MnValue& v );
MnValue			 nx_get_table( MnState* s, MnValue& t, MnValue& n );

void			 nx_set_array( MnState* s, MnValue& t, MnValue& n, MnValue& v );
MnValue			 nx_get_array( MnState* s, MnValue& t, MnValue& n );

void			 nx_set_upval( MnState* s, MnIndex clsidx, MnIndex upvalidx,MnValue& v );
MnValue			 nx_get_upval( MnState* s, MnIndex clsidx, MnIndex upvalidx );

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
		MnMFunction*  func;
		MnClosure*  cls;
		MnUpval*  upv;
	} u;
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

	MnValue		  metatable;
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

	MnValue	metatable;
	vec_val array;

	MnArray( MnState* s );
	~MnArray();

	virtual void marking();

};

//////////////////////////////////////////////////////////////////////////

class MnMFunction : public MnObject
{
public:
	typedef OvVector<MnValue>		vec_value;
	typedef OvVector<MnInstruction> vec_instruction;

	MnMFunction( MnState* s );
	~MnMFunction();

	vec_value		consts;
	vec_instruction	codes;

	OvInt			nargs;

	virtual void marking();
};

//////////////////////////////////////////////////////////////////////////

class MnUpval : public MnObject
{
public:
	MnUpval( MnState* s ) : MnObject(s), link(NULL)
	{
		state->upvals.insert( this );
	};
	~MnUpval()
	{ 
		state->upvals.erase( this );
		if ( link == &hold ) hold = MnValue(); 
	}
	MnValue  hold;
	MnValue* link;
	virtual void marking()
	{
		mark = MARKED;
		if ( link == &hold ) MnMarking(hold);
	}
};

class MnClosure : public MnObject
{
public:
	struct MClosure : OvMemObject
	{
		MnValue  func;
	};

	struct CClosure : OvMemObject
	{
		MnCFunction func;
	};

	const MnCLType type;
	OvVector<MnValue>	upvals;
	union
	{
		CClosure* c;
		MClosure* m;
	} u;

	MnClosure( MnState* s, MnCLType t );
	~MnClosure();

	virtual void marking();

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

	MnMarking( metatable );
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

	MnMarking( metatable );
	for each ( const MnValue& v in array )
	{
		MnMarking( v );
	}
}

//////////////////////////////////////////////////////////////////////////

MnMFunction::MnMFunction( MnState* s )
: MnObject(s)
{

}

MnMFunction::~MnMFunction()
{
	consts.clear();
	codes.clear();
}

void MnMFunction::marking()
{
	mark = MARKED;
	for each ( const MnValue& v in consts )
	{
		MnMarking( v );
	}
}

//////////////////////////////////////////////////////////////////////////

MnClosure::MnClosure( MnState* s, MnCLType t )
: MnObject(s)
, type(t)
{
	u.c = (CClosure*)nx_alloc( (type==MCL)? sizeof(MClosure) : sizeof(CClosure) );
}

MnClosure::~MnClosure()
{
	if ( type == MCL ) u.m->func = MnValue();
	nx_free(u.c);
}

void MnClosure::marking()
{
	mark = MARKED;
	if ( type == MCL ) MnMarking( u.m->func );
	for each ( const MnValue& uv in upvals )
	{
		MnMarking( uv );
	}
}

/////////////////////// type-string convert ////////////////////////////////

const char* ut_type2str( MnObjType type )
{
	for each ( const MnTypeStr& elem in g_type_str )
	{
		if ( elem.type == type )
		{
			return elem.str;
		}
	}
	return g_type_str[MOT_UNKNOWN].str;
};

MnObjType ut_str2type( const char* str )
{
	for each ( const MnTypeStr& elem in g_type_str )
	{
		if ( elem.str == str )
		{
			return elem.type;
		}
	}
	return g_type_str[MOT_UNKNOWN].type;
};

MnObjType ut_str2type( const MnValue& val )
{
	return ut_str2type( MnIsString(val)? MnToString(val)->get_str().c_str() : "unknown" );
}


////////////////////*    open and close    *///////////////////

MnState* mn_open_state()
{
	MnState* s = new(nx_alloc(sizeof(MnState))) MnState;
	s->base = 0;
	s->last  = 0;
	s->ci	= NULL;
	s->pc	= NULL;
	return s;
}

void mn_close_state( MnState* s )
{
	if ( s )
	{
		while ( s->ci ) { MnCallInfo* ci = s->ci; s->ci = ci->prev; nx_free(ci); }
		s->base = s->last = 0;
		s->stack.clear();
		s->global.clear();
		s->strtable.clear();
		s->upvals.clear();
		s->openeduv.clear();
		mn_collect_garbage(s);
		nx_free(s);
	}
}

/////////////////////////* correct / close upval *//////////////////////////////

void nx_correct_upval( MnState* s, MnValue* oldstack )
{
	MnValue* newstack = s->stack.size()? &(s->stack[0]) : NULL;
	if ( oldstack && newstack )
	{
		for each ( MnUpval* upval in s->upvals )
		{
			if ( upval->link != &upval->hold )
			{
				upval->link = (upval->link - oldstack) + newstack;
			}
		}
	}
}

void nx_close_upval( MnState* s, MnValue* level )
{
	MnState::set_upval opened = s->openeduv;
	for each ( MnUpval* upval in opened )
	{
		if ( (upval->link != &upval->hold) && (upval->link >= level) )
		{
			upval->hold = *upval->link;
			upval->link = &upval->hold;
			s->openeduv.erase( upval );
		}
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

void nx_set_stack( MnState* s, MnIndex idx, const MnValue& val )
{
	idx = nx_absidx( s, idx );
	if ( idx >= 0 && idx < s->stack.size() )
	{
		s->stack[idx] = val;
	}
}

MnValue* nx_get_stack_ptr( MnState* s, MnIndex idx )
{
	idx = nx_absidx( s, idx );

	if ( idx >= 0 && idx < s->stack.size() )
	{
		return &(s->stack.at( idx ));
	}
	else
	{
		return NULL;
	}
}

MnValue nx_get_stack( MnState* s, MnIndex idx )
{
	MnValue* v = nx_get_stack_ptr(s,idx);
	return v? *v : MnValue();
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
		MnTable* tbl = MnToTable(t);
		OvHash32 hash = MnToString(n)->get_hash();
		MnTable::map_hash_pair::iterator itor = tbl->table.find( hash );

		if ( itor !=  tbl->table.end() )
		{
			return itor->second.second;
		}
		else if ( MnIsTable(tbl->metatable) )
		{
			return nx_get_table(s,tbl->metatable,n);
		}
	}
	return MnValue();
}

void nx_set_array( MnState* s, MnValue& a, MnValue& n, MnValue& v )
{
	if ( MnIsArray(a) )
	{
		if ( MnIsNumber(n) )
		{
			MnIndex idx = (MnIndex)MnToNumber(n);
			if ( idx >= 0 && idx < MnToArray(a)->array.size() )
			{
				MnToArray(a)->array[idx] = v;
			}
		}
	}
}

MnValue nx_get_array( MnState* s, MnValue& a, MnValue& n )
{
	if ( MnIsArray(a) )
	{
		if ( MnIsNumber(n) )
		{
			MnIndex idx = (MnIndex)MnToNumber(n);
			if ( idx >= 0 && idx < MnToArray(a)->array.size() )
			{
				return MnToArray(a)->array.at(idx);

			}
		}
		else
		{
			return nx_get_table( s, MnToArray(a)->metatable, n );
		}
	}
	return MnValue();
}

void nx_set_upval( MnState* s, MnIndex clsidx, MnIndex upvalidx, MnValue& v )
{
	MnValue c = nx_get_stack(s,clsidx);
	if ( MnIsClosure(c) )
	{
		MnClosure* cls = MnToClosure(c);
		if ( upvalidx > 0 && upvalidx <= cls->upvals.size() )
		{
			cls->upvals[ upvalidx ] = v;
			return ;
		}
	}
}

MnValue nx_get_upval( MnState* s, MnIndex clsidx, MnIndex upvalidx )
{
	MnValue c = nx_get_stack(s,clsidx);
	if ( MnIsClosure(c) )
	{
		MnClosure* cls = MnToClosure(c);
		if ( upvalidx > 0 && upvalidx <= cls->upvals.size() )
		{
			return cls->upvals.at( upvalidx - 1 );
		}
	}
	return MnValue();
}

OvBool nx_is_global( MnState* s, OvHash32 hash )
{
	return ( s->global.find( hash ) != s->global.end() );
}

void mn_set_stack( MnState* s, MnIndex idx )
{
	nx_set_stack( s, idx, nx_get_stack(s,-1));
	mn_pop(s,1);
}

void mn_get_stack( MnState* s, MnIndex idx )
{
	nx_push_value( s, nx_get_stack(s,idx) );
}

void mn_insert_stack( MnState* s, MnIndex idx )
{
	idx = nx_absidx( s, idx );
	if ( idx >= 0 && idx < s->last )
	{
		MnIndex i = idx;
		MnValue val = s->stack[idx];
		while ( i < s->last )
		{
			if ( i+1 == s->last )
			{
				s->stack[idx] = val;
			}
			else
			{
				MnValue temp = s->stack[i+1];
				s->stack[i+1] = val;
				val = temp;
			}
			++i;
		}
	}
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

void mn_set_metatable( MnState* s, MnIndex idx )
{
	MnValue t = nx_get_stack(s,idx);
	MnValue v = nx_get_stack(s,-1);
	if ( MnIsTable(v) )
	{
		if ( MnIsTable(t) )
		{
			MnToTable(t)->metatable = v;
		}
		else if ( MnIsArray(t) )
		{
			MnToArray(t)->metatable = v;
		}
	}
	mn_pop(s,1);
}

void mn_get_metatable( MnState* s, MnIndex idx )
{
	MnValue t = nx_get_stack(s,idx);
	if ( MnIsTable(t) )
	{
		nx_push_value(s,MnToTable(t)->metatable);
	}
	else if ( MnIsArray(t) )
	{
		nx_push_value(s,MnToArray(t)->metatable);
	}
}

void mn_set_upval( MnState* s, MnIndex upvalidx )
{
	nx_set_upval(s, 0, upvalidx, nx_get_stack(s,-1));
}

void mn_get_upval( MnState* s, MnIndex upvalidx )
{
	nx_push_value(s, nx_get_upval(s, 0, upvalidx ) );
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

MnClosure* nx_new_Cclosure( MnState* s )
{
	return new(nx_alloc(sizeof(MnClosure))) MnClosure(s,CCL);
}

MnClosure* nx_new_Mclosure( MnState* s )
{
	return new(nx_alloc(sizeof(MnClosure))) MnClosure(s,MCL);
}

MnUpval*  nx_new_upval( MnState* s )
{
	return new(nx_alloc(sizeof(MnUpval))) MnUpval(s);
}

MnMFunction* nx_new_function( MnState* s )
{
	return new(nx_alloc(sizeof(MnMFunction))) MnMFunction(s);
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

void nx_ensure_stack( MnState* s, OvInt sz )
{
	if ( sz > s->stack.size() )
	{
		MnValue* oldstack = s->stack.size()? &(s->stack[0]) : NULL;
		s->stack.resize( sz );
		nx_correct_upval( s, oldstack );
	}
}

///////////////////////* get/set top *///////////////////////

void mn_set_top( MnState* s, MnIndex idx )
{
	idx = nx_absidx( s, idx ) + 1;
	idx = (idx < s->base)? s->base : idx;
	nx_ensure_stack( s, idx );
	while ( s->last > idx ) s->stack[--s->last] = MnValue();
	s->last = idx;
}

MnIndex mn_get_top( MnState* s )
{
	return s->last - s->base;
}

///////////////////////*   kind of push    *//////////////////////

void nx_push_value( MnState* s, const MnValue& v )
{
	MnIndex top = mn_get_top(s);
	mn_set_top( s, ++top );
	nx_set_stack( s, top, v );
}

void mn_new_table( MnState* s )
{
	nx_push_value( s, MnValue( MOT_TABLE, nx_new_table(s) ) );
}

void mn_new_array( MnState* s )
{
	nx_push_value( s, MnValue( MOT_ARRAY, nx_new_array(s) ) );
}

void mnd_new_garbege( MnState* s )
{
	MnString* str = nx_new_string(s,"f1");
	MnTable* t1 = nx_new_table(s);
	MnTable* t2 = nx_new_table(s);

	t1->table.insert(make_pair(str->get_hash(),make_pair(MnValue(MOT_STRING,str),MnValue(MOT_TABLE,t2))));
	t2->table.insert(make_pair(str->get_hash(),make_pair(MnValue(MOT_STRING,str),MnValue(MOT_TABLE,t1))));
}

void mn_new_closure( MnState* s, MnCFunction proto, OvInt nupvals )
{
	MnClosure* cl = nx_new_Cclosure(s);
	cl->u.c->func = proto;
	cl->upvals.reserve(nupvals);
	for ( OvInt i = 0 ; i < nupvals ; ++i )
	{
		cl->upvals.push_back( nx_get_stack(s, i - nupvals ) );
	}
	mn_pop(s,nupvals);
	nx_push_value( s, MnValue(MOT_CLOSURE,cl) );
}

void mn_push_function( MnState* s, MnCFunction proto )
{
	mn_new_closure(s,proto,0);
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

void mn_push_stack( MnState* s, MnIndex idx )
{
	nx_push_value( s, nx_get_stack(s, idx) );
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
	else if ( MnIsNumber(val) )
	{
		return (MnToNumber(val) != 0.0);
	}
	else if ( MnIsObj(val) )
	{
		return !!MnToObject(val);
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
	else if ( MnIsString(val) )
	{
		OvReal num; 
		if (ut_str2num( MnToString(val)->get_str(), num ) ) return num;;
	}
	return 0;
}

OvString mn_to_string( MnState* s, MnIndex idx )
{
	MnValue val = nx_get_stack( s, idx );
	if ( MnIsString(val) )
	{
		return MnToString(val)->get_str();
	}
	else if ( MnIsNumber(val) )
	{
		OvString str;
		ut_num2str( MnToNumber(val), str );
		return str;
	}

	static OvString empty;
	return empty;
}

OvInt mn_collect_garbage( MnState* s )
{
	for ( MnState::map_hash_val::iterator itor = s->global.begin()
		; itor != s->global.end()
		; ++itor )
	{
		MnMarking(itor->second);
	}
	MnIndex idx = s->last;
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

	OvInt num = 0;
	while ( dead )
	{
		++num;

		MnObject* next = dead->next;

		nx_delete_garbage(dead);

		dead = next;
	}
	return num;
}

OvInt mt_global_length(MnState* s)
{
	OvReal nsize = 0.0;
	MnValue arg1 = nx_get_stack(s,1);
	if ( MnIsArray(arg1) )
	{
		mn_push_number( s, (OvReal)MnToArray(arg1)->array.size() );
	}
	else if ( MnIsTable(arg1) )
	{
		mn_push_number( s, (OvReal)MnToTable(arg1)->table.size() );
	}
	else
	{
		mn_push_nil( s );
	}
	return 1;
}

OvInt mt_global_resize( MnState* s )
{
	MnValue arg1 = nx_get_stack(s,1);
	MnValue arg2 = nx_get_stack(s,2);
	if ( MnIsArray(arg1) && MnIsNumber(arg2) )
	{
		MnToArray(arg1)->array.resize( (OvSize)MnToNumber(arg2) );
	}
	return 0;
}

OvInt mt_collect_garbage( MnState* s )
{
	mn_push_number( s, mn_collect_garbage(s) );
	return 1;
}

OvInt mt_print( MnState* s )
{
	printf( mn_to_string(s,1).c_str() );
	printf( "\n" );
	return 0;
}

void mn_default_lib( MnState* s )
{
	mn_push_string( s, "length" );
	mn_push_function( s, mt_global_length );
	mn_set_global( s );

	mn_push_string( s, "resize" );
	mn_push_function( s, mt_global_resize );
	mn_set_global( s );

	mn_push_string( s, "collect_garbage" );
	mn_push_function( s, mt_collect_garbage );
	mn_set_global( s );

	mn_push_string( s, "print" );
	mn_push_function( s, mt_print);
	mn_set_global( s );
}

//////////////////////////////////////////////////////////////////////////

struct MnInstruction : public OvMemObject
{
	OvByte op;
	union
	{
		OvInt eax;
		struct { OvShort ax; OvShort bx; };
		struct { OvChar a; OvChar b; OvChar c; OvChar d; };
	};
};

enum MnOperate
{
	MOP_NONEOP = 0, 
	MOP_NEWTABLE,
	MOP_NEWARRAY,
	MOP_NEWCLOSURE,

	MOP_LINK_UPVAL,
	MOP_LINK_STACK,

	MOP_SET_STACK,
	MOP_GET_STACK,
	MOP_INSERT_STACK,

	MOP_SET_FIELD,
	MOP_GET_FIELD,

	MOP_SET_GLOBAL,
	MOP_GET_GLOBAL,

	MOP_SET_META,
	MOP_GET_META,

	MOP_SET_UPVAL,
	MOP_GET_UPVAL,

	MOP_PUSH,
	MOP_POP,

	MOP_AND,
	MOP_OR,
	MOP_XOR,

	MOP_EQ,
	MOP_LT,
	MOP_GT,

	MOP_NOT,
	MOP_CMP,
	MOP_JMP,


	MOP_ADD,
	MOP_SUB,
	MOP_MUL,
	MOP_DIV,

	MOP_CALL,

	MOP_LOG,
};

OvInt nx_exec_func( MnState* s, MnMFunction* func )
{

#define _Ax	(MnAx(i))
#define _A	(MnA(i))
#define _B	(MnB(i))
#define _C	(MnC(i))

#define fB	(MnReposit(_B))
#define fC	(MnReposit(_C))

#define iAx (_Ax)
#define iA	(MnIdx(_A))
#define iB	(MnIdx(_B))
#define iC	(MnIdx(_C))

#define cB  (func->consts[iB - 1])
#define cC  (func->consts[iC - 1])

#define rA (sA)
#define rB ((fB==FlagConst)? cB:sB)
#define rC ((fC==FlagConst)? cC:sC)

	s->pc = func->codes.size()? &(func->codes[0]) : NULL;
	while ( s->pc )
	{
		MnInstruction i = *s->pc; ++s->pc;
		switch ( i.op )
		{
		case MOP_NEWTABLE:	mn_new_table( s ); break;
		case MOP_NEWARRAY:	mn_new_array( s ); break;
		case MOP_NEWCLOSURE:
			{
				MnClosure* cls = nx_new_Mclosure(s);
				MnClosure::MClosure* mcls = cls->u.m;
				mcls->func = func->consts[i.ax-1];
				cls->upvals.resize( i.bx );
				nx_push_value(s,MnValue(MOT_CLOSURE,cls));
			}
			break;

		case MOP_SET_STACK:	mn_set_stack( s, i.eax ); break;
		case MOP_GET_STACK:	mn_get_stack( s, i.eax ); break;
		case MOP_INSERT_STACK:	mn_insert_stack( s, i.eax ); break;

		case MOP_SET_FIELD:	mn_set_field( s, i.eax); break;
		case MOP_GET_FIELD:	mn_get_field( s, i.eax); break;

		case MOP_SET_GLOBAL:	mn_set_global( s ); break;
		case MOP_GET_GLOBAL:	mn_get_global( s ); break;

		case MOP_SET_META:	mn_set_metatable( s, i.eax ); break;
		case MOP_GET_META:	mn_get_metatable( s, i.eax ); break;

		case MOP_SET_UPVAL:	mn_set_upval( s, i.eax ); break;
		case MOP_GET_UPVAL:	mn_get_upval( s, i.eax ); break;

		case MOP_PUSH:		nx_push_value( s, func->consts[i.eax-1] ); break;
		case MOP_POP:		mn_pop( s, i.eax ); break;

		case MOP_CALL:		mn_call( s, i.ax, i.bx ); break;

		case MOP_LOG:
			{
				MnValue log = func->consts[i.eax-1];
				printf( MnToString(log)->get_str().c_str() );
				printf("\n");
			}
			break;

		case MOP_JMP: s->pc += (i.eax - 1); break;

		case MOP_NOT:
			{
				OvBool b = mn_to_boolean(s,-1);
				mn_pop(s,1);
				mn_push_boolean(s,!b);
			}
			break;

		case MOP_CMP:
			{
				OvBool b = mn_to_boolean(s,-1);
				mn_pop(s,1);
				if ( !b ) ++s->pc;
			}
			break;

		case MOP_EQ:
		case MOP_LT:
		case MOP_GT:
			{
				if ( mn_is_number(s,-2) && mn_is_number(s,-1) )
				{
					OvReal na = mn_to_number(s,-2);
					OvReal nb = mn_to_number(s,-1);
					mn_pop(s,2);
					if ( i.op == MOP_EQ ) mn_push_boolean( s, (na == nb) );
					else if ( i.op == MOP_LT ) mn_push_boolean( s, (na < nb) );
					else if ( i.op == MOP_GT ) mn_push_boolean( s, (na > nb) );
					else mn_push_nil(s);
				}
				else if ( mn_is_string(s,-2) && mn_is_string(s,-1) )
				{
					OvString sa = mn_to_string(s,-2);
					OvString sb = mn_to_string(s,-1);
					mn_pop(s,2);
					if ( i.op == MOP_EQ ) mn_push_boolean( s, ( sa == sb ) );
					else mn_push_nil(s);
				}
				else
				{
					mn_get_metatable(s,-2);
					if ( i.op == MOP_EQ ) mn_push_string(s,"__eq");
					else if ( i.op == MOP_LT ) mn_push_string(s,"__lt");
					else if ( i.op == MOP_GT ) mn_push_string(s,"__gt");
					mn_get_field(s,-2);
					mn_insert_stack(s,-4);
					mn_pop(s,1);
					mn_call(s,2,1);
				}
			}
			break;

		case MOP_ADD:
		case MOP_SUB:
		case MOP_MUL:
		case MOP_DIV:
			{
				if ( mn_is_number(s,-2) )
				{
					OvReal na = mn_to_number(s,-2);
					OvReal nb = mn_to_number(s,-1);
					mn_pop(s,2);

					switch (i.op)
					{
					case MOP_ADD: na += nb; break;
					case MOP_SUB: na -= nb; break;
					case MOP_MUL: na *= nb; break;
					case MOP_DIV: na /= nb; break;
					}

					mn_push_number( s, na );
				}
				else if ( mn_is_string(s,-2) )
				{
					OvString sa = mn_to_string(s,-2);
					OvString sb = mn_to_string(s,-1);
					
					if ( i.op == MOP_ADD )
					{
						mn_pop(s,2);
						mn_push_string( s, sa+sb );
					}
					else mn_pop(s,1);
				}
				else
				{
					mn_get_metatable(s,-2);
					if ( i.op == MOP_ADD ) mn_push_string(s,"__add");
					else if ( i.op == MOP_SUB ) mn_push_string(s,"__sub");
					else if ( i.op == MOP_MUL ) mn_push_string(s,"__mul");
					else if ( i.op == MOP_DIV ) mn_push_string(s,"__div");
					mn_get_field(s,-2);
					mn_insert_stack(s,-4);
					mn_pop(s,1);
					mn_call(s,2,1);
				}
			}
			break;

		case MOP_NONEOP:
		default:
			return 0;
		}
	}
	return 0;
}

void mn_call( MnState* s, OvInt nargs, OvInt nrets )
{
	nargs = max(nargs,0);
	MnValue v = nx_get_stack(s, -(1 + nargs) );
	MnCallInfo* ci = ( MnCallInfo* )nx_alloc( sizeof( MnCallInfo ) );
	ci->prev = s->ci;
	ci->savepc = s->pc;
	ci->base = s->base;

	s->ci    = ci;
	s->base  = s->last - nargs;

	OvInt r = 0;
	if ( MnIsClosure(v) )
	{
		MnClosure* cls = MnToClosure(v);
		ci->cls  = cls;
		if ( cls->type == CCL )
		{
			MnClosure::CClosure* ccl = cls->u.c;
			r = ccl->func(s);
		}
		else
		{
			MnClosure::MClosure* mcl = cls->u.m;
			r = nx_exec_func( s, MnToFunction( mcl->func ) );
		}
	}

	nx_close_upval( s, &(s->stack[ s->base - 1 ]) );

	MnIndex oldlast = s->base - 1;
	MnIndex newlast = oldlast + nrets;
	MnIndex first_ret  = s->last - r;
	first_ret = max( oldlast, first_ret );
	nx_ensure_stack( s, oldlast + max( nrets, r ) );

	if ( r > 0 ) for ( OvInt i = 0 ; i < r ; ++i )  s->stack[oldlast++] = s->stack[first_ret++];

	while ( oldlast < s->last ) s->stack[ --s->last ] = MnValue();
	while ( oldlast > s->last ) s->stack[ s->last++ ] = MnValue();

	ci = s->ci;
	s->last  = newlast;
	s->base = ci->base;
	s->ci	= ci->prev;
	nx_free(ci);
}

struct MnCompileState : public OvMemObject
{
	MnState* state;
	OvInputStream* is;
	OvChar c;
	MnValue	errfunc;
};

void cp_build_func( MnCompileState* cs, MnMFunction* func );
void mn_load_asm( MnState* s, const OvString& file, MnIndex idx )
{
	OvFileInputStream fis( file );
	MnCompileState cs;
	cs.state = s;
	cs.is = &fis;
	cs.is->Read(cs.c);
	cs.errfunc = nx_get_stack(s,idx);

	MnMFunction* func = nx_new_function(s);
	cp_build_func(&cs,func);
	MnClosure* cls = nx_new_Mclosure( s );
	cls->u.m->func = MnValue(MOT_FUNCPROTO,func);
	nx_push_value(s,MnValue(MOT_CLOSURE,cls));
	mn_call(s,0,0);
}

enum eErrCode
{
	eErrCode_Unknown,
};

void cp_call_errfunc( MnCompileState* cs, OvInt errcode, const OvString& msg )
{
	nx_push_value( cs->state, cs->errfunc );
	mn_push_number( cs->state, errcode );
	mn_push_string( cs->state, msg );
	mn_call( cs->state, 2, 0 );
}

enum
{
	eTNumber = 256,
	eTString,
	eTIdentifier,
	eTEndOfStream,
	eTUnknown,
};

OvInt cp_token( MnCompileState* cs, OvInt& num, OvString& str )
{
	OvChar& c = cs->c;
	num = 0;
	str.clear();
#define cp_read() (cs->is->Read(c))
	while ( true )
	{
		if ( c == EOF  )
		{
			return eTEndOfStream;
		}
		else if ( isdigit(c) )
		{
			OvInt mult = 10;
			if ( c == '0' )
			{
				mult = 8;
				cp_read();
				if ( c == 'x' || c == 'X' )
				{
					mult = 16;
					cp_read();
				}
			}
			bool under = false;
			while ( true )
			{
				if ( isdigit(c) ) num = (num * mult) + (c-'0'); else break;
				if ( !cp_read() ) break;
			}
			return eTNumber;
		}
		else if ( c == '"' )
		{
			cp_read();
			do
			{
				str.push_back(c);
			} while ( cp_read() && c != '"' );
			cp_read();
			return eTString;
		}
		else if ( isalpha(c) || c == '_' )
		{
			do
			{
				str.push_back(c);
			} while ( cp_read() && (isalnum(c) || c == '_') );
			return eTIdentifier;
		}
		else
		{
			OvChar ret = c;
			cp_read();
			return ret;
		}
	}
	return eTUnknown;
#undef cp_read
}

MnOperate cp_str2op( OvString str ) 
{
	if ( str == "push" ) return MOP_PUSH;
	else if ( str == "pop" ) return MOP_POP;
	if ( str == "log" ) return MOP_LOG;

	else if ( str == "newarray" ) return MOP_NEWARRAY;
	else if ( str == "newtable" ) return MOP_NEWTABLE;
	else if ( str == "newclosure" ) return MOP_NEWCLOSURE;

	else if ( str == "setstack" ) return MOP_SET_STACK;
	else if ( str == "getstack" ) return MOP_GET_STACK;
	else if ( str == "insertstack" ) return MOP_INSERT_STACK;

	else if ( str == "setfield" ) return MOP_SET_FIELD;
	else if ( str == "getfield" ) return MOP_GET_FIELD;

	else if ( str == "setglobal" ) return MOP_SET_GLOBAL;
	else if ( str == "getglobal" ) return MOP_GET_GLOBAL;

	else if ( str == "setmeta" ) return MOP_SET_META;
	else if ( str == "getmeta" ) return MOP_GET_META;

	else if ( str == "setupval" ) return MOP_SET_UPVAL;
	else if ( str == "getupval" ) return MOP_GET_UPVAL;

	else if ( str == "jmp" )   return MOP_JMP;
	else if ( str == "cmp" )   return MOP_CMP;

	else if ( str == "add" )   return MOP_ADD;
	else if ( str == "sub" )   return MOP_SUB;
	else if ( str == "mul" )   return MOP_MUL;
	else if ( str == "div" )   return MOP_DIV;

	else if ( str == "not" )   return MOP_NOT;

	else if ( str == "eq" )   return MOP_EQ;
	else if ( str == "lt" )   return MOP_LT;
	else if ( str == "gt" )   return MOP_GT;

	else if ( str == "call" )   return MOP_CALL;

	else if ( str == "linkstack" )   return MOP_LINK_STACK;
	else if ( str == "linkupval" )   return MOP_LINK_UPVAL;

	return MOP_NONEOP;
}

OvBool cp_operate( MnCompileState* cs, MnOperate& op )
{
	OvString str;
	OvInt num;
	OvInt tok;
	do tok = cp_token(cs,num,str); while ( isspace((OvChar)tok) );

	op = MOP_NONEOP;
	if ( tok == eTIdentifier )
	{
		op = cp_str2op(str);
		if ( op == MOP_NONEOP ) cp_call_errfunc( cs, 0, (str + " - unknown operate\n"));
		return true;
	}
	else if ( (OvChar)tok == ':' )
	{
		do tok = cp_token(cs,num,str); while ( isspace((OvChar)tok) );
		if ( str == "end" )
		{
			return false;
		}
	}
	return true;
}

OvInt	  cp_operand( MnCompileState* cs )
{
	OvString str;
	OvInt num;
	OvInt tok;
	do { tok = cp_token(cs,num,str); } while ( isspace((OvChar)tok) );

	bool minus = ( tok == '-' );
	if ( minus )
	{
		do { tok = cp_token(cs,num,str); } while ( isspace((OvChar)tok) );
	}

	if ( tok == eTNumber )
	{
		return num * (minus? -1:1);
	}
	return 0;
}

MnOperand cp_func_const( MnCompileState* cs, MnMFunction* func )
{
	OvString str;
	OvInt num;
	OvInt tok;
	do 
	{
		tok = cp_token(cs,num,str);
	} while ( isspace((OvChar)tok) );

	if ( tok == eTIdentifier )
	{
		if ( str == "false" || str == "true" )
		{
			func->consts.push_back( MnValue( (OvBool)(str=="true") ) );
		}
		else if ( str == "nil" )
		{
			func->consts.push_back( MnValue() );
		}
		return func->consts.size();
	}
	else if ( tok == eTString )
	{
		func->consts.push_back( MnValue( MOT_STRING, nx_new_string(cs->state,str) ) );
		return func->consts.size();
	}
	else if ( tok == eTNumber )
	{
		func->consts.push_back( MnValue( (OvReal)num ) );
		return func->consts.size();
	}
	else if ( (OvChar)tok == ':' )
	{
		do tok = cp_token(cs,num,str); while ( isspace((OvChar)tok) );
		if ( str == "func" )
		{
			MnMFunction* proto = nx_new_function(cs->state);
			cp_build_func(cs,proto);
			func->consts.push_back( MnValue( MOT_FUNCPROTO, proto ) );
			return func->consts.size();
		}
	}
	return 0;
}

void cp_build_func( MnCompileState* cs, MnMFunction* func )
{
	MnInstruction i;
	while ( cp_operate( cs, (MnOperate&)i.op ) )
	{
		switch ( i.op )
		{
		case MOP_SET_STACK:
		case MOP_GET_STACK:
		case MOP_INSERT_STACK:
		case MOP_SET_FIELD:
		case MOP_GET_FIELD:
		case MOP_SET_META:
		case MOP_GET_META:
		case MOP_SET_UPVAL:
		case MOP_GET_UPVAL:
		case MOP_POP:
		case MOP_JMP:
			i.eax = cp_operand(cs);
			break;
		case MOP_PUSH:
		case MOP_LOG:
		case MOP_NEWCLOSURE:
			i.eax  = cp_func_const( cs, func );
			break;
		case MOP_CALL:
			i.ax = cp_operand(cs);
			i.bx = cp_operand(cs);
			break;
		case MOP_NONEOP:
			func->codes.push_back( i );
			return;
		}
		func->codes.push_back( i );
	}
	func->codes.push_back( i );
}
