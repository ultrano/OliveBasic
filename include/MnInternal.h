#pragma once
#pragma warning(disable:4146)
#pragma warning(disable:4018)
#include "MnScript.h"
#include "OvMemObject.h"
#include "OvBuffer.h"
#include "OvSolidString.h"
#include "OvFile.h"

#define	MOT_UPVAL		(10)
#define	MOT_FUNCPROTO	(11)
#define MOT_TYPEMAX		(12)

#define VERSION_MAJOR	(0)
#define VERSION_MINOR	(0)
#define VERSION_PATCH	(3)

class MnCallInfo;
class MnObject;
class MnString;
class MnTable;
class MnValue;
class MnArray;
class MnMFunction;
class MnClosure;
class MnUpval;
class MnUserData;
class MnMiniData;
typedef OvUInt MnInstruction;

typedef OvUShort MnOperand;

enum MnCLType { CCL= 1, MCL = 2 };
struct MnTypeStr { OvInt type; const char* str; };
const MnTypeStr g_type_str[] = 
{
	{ MOT_UNKNOWN, "[unknown]" },
	{ MOT_NIL, "[nil]" },
	{ MOT_BOOLEAN, "[boolean]" },
	{ MOT_NUMBER, "[number]" },
	{ MOT_STRING, "[string]" },

	{ MOT_TABLE, "[table]" },
	{ MOT_ARRAY, "[array]" },
	{ MOT_CLOSURE, "[closure]" },
	{ MOT_USERDATA, "[userdata]" },
	{ MOT_MINIDATA, "[minidata]" },
	{ MOT_UPVAL, "[upval]" },
	{ MOT_FUNCPROTO, "[funcproto]" },
};

#define METHOD_EQ ("__eq")
#define METHOD_NEQ ("__neq")
#define METHOD_LT ("__lt")
#define METHOD_GT ("__gt")
#define METHOD_NEWINDEX ("__newindex")
#define METHOD_INDEX ("__index")
#define METHOD_CALL ("__call")
#define METHOD_ADD ("__add")
#define METHOD_SUB ("__sub")
#define METHOD_MUL ("__mul")
#define METHOD_DIV ("__div")
#define METHOD_MOD ("__mod")

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
#define MnIsUserData( v ) ((v).type == MOT_USERDATA)
#define MnIsMiniData( v ) ((v).type == MOT_MINIDATA)
#define MnIsObj( v ) \
	( \
	MnIsString((v)) ||  \
	MnIsArray((v)) ||  \
	MnIsClosure((v)) ||  \
	MnIsTable((v)) ||  \
	MnIsUserData((v)) ||  \
	MnIsMiniData((v)) ||  \
	MnIsUpval((v)) ||  \
	MnIsFunction((v))   \
	)

#define MnBadConvert()	(NULL)
#define MnToBoolean( v ) ((MnIsBoolean(v)? (v).u.bln : MnBadConvert()))
#define MnToNumber( v ) ((MnIsNumber(v)? (v).u.num : MnBadConvert()))
#define MnToObject( v ) (MnIsObj(v)? (v).u.cnt->u.obj : MnBadConvert())
#define MnToString( v ) (MnIsString(v)? (v).u.cnt->u.str : MnBadConvert())
#define MnToTable( v ) (MnIsTable(v)? (v).u.cnt->u.tbl : MnBadConvert())
#define MnToArray( v ) (MnIsArray(v)? (v).u.cnt->u.arr : MnBadConvert())
#define MnToClosure( v ) (MnIsClosure(v)? (v).u.cnt->u.cls: MnBadConvert())
#define MnToUserData( v ) (MnIsUserData(v)? (v).u.cnt->u.user: MnBadConvert())
#define MnToMiniData( v ) (MnIsMiniData(v)? (v).u.cnt->u.mini: MnBadConvert())
#define MnToUpval( v ) (MnIsUpval(v)? (v).u.cnt->u.upv: MnBadConvert())
#define MnToFunction( v ) (MnIsFunction(v)? (v).u.cnt->u.func: MnBadConvert())
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

OvBool ut_str2num( const OvString& str, MnNumber &num ) 
{
	OvInt i = 0;
	OvChar c = str[i];
	num = 0;
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
			if ( i >= str.size() ) break;
			c = str[++i];
		}
		return true;
	}
	return false;
}

OvBool ut_num2str( const MnNumber& num, OvString& str )
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
	MnValue*	 begin;
	MnValue*	 end;
	MnValue*	 base;
	MnValue*	 top;

	set_upval	 upvals;
	set_upval	 openeduv;

	MnCallInfo*		ci;
	OvByte*			pc;
	MnClosure*		cls;
	MnMFunction*	func;
	OvUInt			accumid;
};

//////////////////////////////////////////////////////////////////////////

class MnCallInfo : public OvMemObject
{
public:
	MnCallInfo*		prev;
	MnIndex			base;
	MnIndex			top;
	MnClosure*		cls;
	MnMFunction*	func;
	OvByte*			pc;
};

//////////////////////////////////////////////////////////////////////////
void*			ut_alloc( OvSize sz ) { return sz>0? OvMemAlloc(sz) : NULL ; };
void			ut_free( void* p ) { OvMemFree(p); };

void			ut_ensure_stack( MnState* s, OvInt sz );

MnString*		ut_newstring( MnState* s, const OvString& str );
MnTable*		ut_newtable( MnState* s );
MnArray*		ut_newarray( MnState* s );
MnClosure*		ut_newCclosure( MnState* s, MnCLType t );
MnClosure*		ut_newMclosure( MnState* s );
MnMFunction*	ut_newfunction( MnState* s );
MnUserData*		ut_newuserdata( MnState* s, void* p );
MnMiniData*		ut_newminidata( MnState* s, OvInt sz );

void			ut_delete_object( MnObject* o );
void			ut_delete_garbage( MnObject* o );

OvBool			ut_isglobal( MnState* s, OvHash32 hash );

void			ut_setglobal( MnState* s, MnValue& n, const MnValue& val );
MnValue			ut_getglobal( MnState* s, MnValue& n );


void			ut_setstack( MnState* s, MnIndex idx, const MnValue& val );
MnValue			ut_getstack( MnState* s, MnIndex idx );

void			ut_settable( MnState* s, MnValue& t, MnValue& n, MnValue& v );
MnValue			ut_gettable( MnState* s, MnValue& t, MnValue& n );

void			ut_setarray( MnState* s, MnValue& t, MnValue& n, MnValue& v );
MnValue			ut_getarray( MnState* s, MnValue& t, MnValue& n );

void			ut_setfield( MnState* s, MnValue& c, MnValue& n, MnValue& v );
MnValue			ut_getfield( MnState* s, MnValue& c, MnValue& n );

MnValue			ut_getmeta( const MnValue& c );
void			ut_setmeta( MnValue& c, const MnValue& m );

void			ut_setupval( MnState* s, MnIndex upvalidx,MnValue& v );
MnValue			ut_getupval( MnState* s, MnIndex upvalidx );
MnUpval*		ut_getupval_ptr( MnState* s, MnIndex upvalidx );

void			ut_pushvalue( MnState* s, const MnValue& v );

const OvChar*	ut_typename( const MnValue& v );
OvInt			ut_type( const MnValue& v );

MnValue			ut_getconst( MnMFunction* f, MnIndex idx );

void			ut_excute_func( MnState* s, MnMFunction* func );
//////////////////////////////////////////////////////////////////////////

class MnRefCounter : public OvMemObject
{
public:
	MnRefCounter() : scnt(0), wcnt(0) { u.obj = NULL; };
	OvInt		scnt;
	OvInt		wcnt;
	union
	{
		MnObject* 		obj;
		MnString* 		str;
		MnTable*  		tbl;
		MnArray*  		arr;
		MnMFunction*	func;
		MnClosure*		cls;
		MnUpval*		upv;
		MnUserData*		user;
		MnMiniData*		mini;
	} u;
};

//////////////////////////////////////////////////////////////////////////

class MnValue : public OvMemObject
{
public:
	OvInt type;
	union
	{
		MnRefCounter* cnt;
		MnNumber num;
		OvBool bln;
	} u;

	MnValue();
	MnValue( const MnValue &v );
	MnValue( OvBool b );
	MnValue( MnNumber n );
	MnValue( OvInt t, MnObject* o );
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
	const OvUInt	id;
	MnRefCounter*	refcnt;

	MnObject*	next;
	MnObject*	prev;

	/* we use "mark and sweep" GC algorithm*/
	OvByte		mark;

	virtual void marking() = 0;
};

MnObject::MnObject( MnState* s ) 
: state(s)
, id(++s->accumid)
, next(NULL)
, prev(NULL)
, mark(UNMARKED)
, refcnt(new(ut_alloc(sizeof(MnRefCounter))) MnRefCounter)
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
	if ( --refcnt->wcnt == 0) ut_free( refcnt );
}

//////////////////////////////////////////////////////////////////////////

class MnString : public MnObject
{
public:

	MnString( MnState* s, OvHash32 hash, const OvString& sstr );
	~MnString();

	OvHash32		hash() { return m_hash; };
	const OvString& str() { return m_str; };

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
	typedef OvVector<MnValue> vec_value;
	typedef OvVector<OvByte>  vec_byte;

	MnMFunction( MnState* s );
	~MnMFunction();

	vec_value		consts;
	vec_byte		code;

	OvUInt			nargs;

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

//////////////////////////////////////////////////////////////////////////

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

class MnUserData : public MnObject
{
public:

	MnUserData( MnState* s, void* p );
	~MnUserData();

	void* ptr;
	MnValue	metatable;

	virtual void marking();
};

//////////////////////////////////////////////////////////////////////////

class MnMiniData : public MnObject
{
public:

	MnMiniData( MnState* s, OvInt sz );
	~MnMiniData();

	void* ptr;
	MnValue	metatable;

	virtual void marking();
};

//////////////////////////////////////////////////////////////////////////

void	ut_inc_ref( MnValue& v )
{
	if ( MnIsObj(v) ) 
	{
		if ( MnToObject(v) ) ++v.u.cnt->scnt;
		++v.u.cnt->wcnt;
	}
}

void	ut_dec_ref( MnValue& v )
{
	if ( MnIsObj(v) )
	{
		if ( MnToObject(v) )
		{
			--v.u.cnt->scnt;
			if ( v.u.cnt->scnt == 0 && MnToObject(v)->mark != DEAD )
			{
				ut_delete_object( v.u.cnt->u.obj );
				v.u.cnt->u.obj = NULL;
			}
		}

		if ( --v.u.cnt->wcnt == 0 ) ut_free( v.u.cnt );
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
	ut_inc_ref(*this);
}

MnValue::MnValue( OvBool b )
{
	type = MOT_BOOLEAN;
	u.bln = b;
}

MnValue::MnValue( MnNumber n )
{
	type = MOT_NUMBER;
	u.num = n;
}

MnValue::MnValue( OvInt t, MnObject* o )
{
	type	 = t;
	u.cnt = o->refcnt;
	ut_inc_ref(*this);
}

MnValue::~MnValue()
{
	ut_dec_ref(*this);
}

const MnValue& MnValue::operator=( const MnValue& v )
{
	if ( this != &v ) 
	{
		ut_dec_ref(*this);
		type = v.type;
		u	 = v.u;
		ut_inc_ref(*this);
	}
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
, nargs(0)
{
}

MnMFunction::~MnMFunction()
{
	consts.clear();
	code.clear();
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
	u.c = (CClosure*)ut_alloc( (type==MCL)? sizeof(MClosure) : sizeof(CClosure) );
}

MnClosure::~MnClosure()
{
	if ( type == MCL ) u.m->func = MnValue();
	ut_free(u.c);
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

//////////////////////////////////////////////////////////////////////////

MnUserData::MnUserData( MnState* s, void* p ) : MnObject(s), ptr(p)
{

}

MnUserData::~MnUserData()
{
	ptr = NULL; 
	metatable = MnValue();
}

void MnUserData::marking()
{
	mark = MARKED;
	MnMarking( metatable );
}

//////////////////////////////////////////////////////////////////////////

MnMiniData::MnMiniData( MnState* s, OvInt sz )
: MnObject(s)
, ptr( ut_alloc( sz ) )
{

}

MnMiniData::~MnMiniData()
{
	if ( ptr ) ut_free( ptr );
}

void MnMiniData::marking()
{
	mark = MARKED;
}

/////////////////////// type-string convert ////////////////////////////////

const char* ut_type2str( OvInt type )
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

OvInt ut_str2type( const char* str )
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

OvInt ut_str2type( const MnValue& val )
{
	return ut_str2type( MnIsString(val)? MnToString(val)->str().c_str() : "unknown" );
}


/////////////////////////* correct / close upval *//////////////////////////////

void ut_correct_upval( MnState* s, MnValue* oldstack )
{
	MnValue* newstack = s->begin;
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

void ut_close_upval( MnState* s, MnValue* level )
{
	MnState::set_upval::iterator itor = s->openeduv.begin();
	while ( itor != s->openeduv.end() )
	{
		MnUpval* upval = *itor;
		if ( (upval->link != &upval->hold) && (upval->link >= level) )
		{
			upval->hold = *(upval->link);
			upval->link = &(upval->hold);
			itor = s->openeduv.erase( itor );
			continue;
		}
		++itor;
	}
}

////////////////////////*    get/set stack, field    */////////////////////////////////

void ut_setglobal( MnState* s, MnValue& n, const MnValue& val )
{
	if ( MnIsString(n) )
	{
		OvHash32 hash = MnToString(n)->hash();
		if ( MnIsNil(val) )
		{
			s->global.erase( hash );
		}
		else
		{
			s->global[hash] = val;
		}
	}
}

MnValue ut_getglobal( MnState* s, MnValue& n )
{
	if ( MnIsString(n) )
	{
		OvHash32 hash = MnToString(n)->hash();
		MnState::map_hash_val::iterator itor = s->global.find( hash );
		if ( itor != s->global.end() )
		{
			return itor->second;
		}
	}
	return MnValue();
}

void ut_setstack( MnState* s, MnIndex idx, const MnValue& val )
{
	if ( idx < 0 )
	{
		MnValue* ret = s->top + idx;
		if ( ret > s->base ) *ret = val;
	}
	else if ( idx > 0 )
	{
		MnValue* ret = s->base + idx;
		if ( ret < s->top ) *ret = val;
	}
}

MnValue* ut_getstack_ptr( MnState* s, MnIndex idx )
{
	if ( idx < 0 )
	{
		MnValue* ret = s->top + idx;
		if ( ret > s->base ) return ret;
		else return NULL;
	}
	else if ( idx > 0 )
	{
		MnValue* ret = s->base + idx;
		if ( ret < s->top ) return ret;
		else return NULL;
	}
	return NULL;
}

MnValue ut_getstack( MnState* s, MnIndex idx )
{
	MnValue* v = ut_getstack_ptr(s,idx);
	return v? *v : MnValue();
}

MnValue ut_getconst( MnMFunction* f, MnIndex idx )
{
	if ( idx > 0 && idx <= f->consts.size() )
	{
		return f->consts[ idx - 1 ];
	}
	else if ( idx < 0 && -idx <= f->consts.size() )
	{
		return f->consts[ f->consts.size() + idx ];
	}
	return MnValue();
}

OvBool ut_meta_newindex( MnState* s, MnValue& c, MnValue& n, MnValue& v ) 
{
	if ( !MnIsNil(ut_getmeta( c )) )
	{
		ut_pushvalue( s, ut_getmeta( c ) );
		mn_pushstring( s, METHOD_NEWINDEX );
		mn_getfield( s, -2 );

		if ( mn_isfunction( s, -1 ) )
		{
			ut_pushvalue( s, c );
			ut_pushvalue( s, n );
			ut_pushvalue( s, v );
			mn_call( s, 3, 0 );
			mn_pop( s, 1 );
			return true;
		}
		else
		{
			mn_pop( s, 2 );
			return false;
		}
	}
	return false;
}

void ut_settable( MnState* s, MnValue& t, MnValue& n, MnValue& v )
{
	if ( MnIsTable(t) )
	{
		MnTable* tbl = MnToTable(t);
		if ( MnIsString(n) )
		{
			OvHash32 hash = MnToString(n)->hash();
			MnTable::map_hash_pair::iterator itor = tbl->table.find( hash );
			if ( itor !=  tbl->table.end() )
			{
				if ( MnIsNil(v) )	tbl->table.erase( itor );
				else				itor->second = make_pair(n,v);
				return;
			}
			else if ( !ut_meta_newindex(s, t, n, v) )
			{
				tbl->table.insert( make_pair(hash,make_pair(n,v)) );
			}
		}
	}
}

MnValue ut_meta_index( MnState* s, MnValue& c, MnValue& n ) 
{
	if ( !MnIsNil(ut_getmeta( c )) )
	{
		ut_pushvalue( s, ut_getmeta( c ) );
		mn_pushstring( s, METHOD_INDEX );
		mn_getfield( s, -2 );
		if ( mn_isfunction( s, -1 ) )
		{
			ut_pushvalue( s, c );
			ut_pushvalue( s, n );
			mn_call( s, 2, 1 );

			MnValue ret = ut_getstack( s, -1 );
			mn_pop( s, 2 );
			return ret;
		}
		mn_pop(s,2);
	}
	return MnValue();
}

MnValue ut_gettable( MnState* s, MnValue& t, MnValue& n )
{
	if ( MnIsTable(t) )
	{
		MnTable* tbl = MnToTable(t);
		if ( MnIsString(n) )
		{
			OvHash32 hash = MnToString(n)->hash();
			MnTable::map_hash_pair::iterator itor = tbl->table.find( hash );

			if ( itor !=  tbl->table.end() )
			{
				return itor->second.second;
			}
		}
		return ut_meta_index(s, t, n);
	}
	return MnValue();
}

void ut_setarray( MnState* s, MnValue& a, MnValue& n, MnValue& v )
{
	if ( MnIsArray(a) )
	{
		MnArray* arr = MnToArray(a);
		if ( MnIsNumber(n) )
		{
			MnIndex idx = (MnIndex)MnToNumber(n) - 1;
			if ( idx >= 0 && idx < arr->array.size() )
			{
				arr->array[idx] = v;
			}
			else ut_meta_newindex(s, a, n, v);
		}
	}
}

MnValue ut_getarray( MnState* s, MnValue& a, MnValue& n )
{
	if ( MnIsArray(a) )
	{
		MnArray* arr = MnToArray(a);
		if ( MnIsNumber(n) )
		{
			MnIndex idx = (MnIndex)MnToNumber(n) - 1;
			if ( idx >= 0 && idx < arr->array.size() )
			{
				return arr->array.at(idx);
			}
		}
		return ut_meta_index( s, a, n );
	}
	return MnValue();
}

void ut_setupval( MnState* s, MnIndex upvalidx, MnValue& v )
{
	if ( s->cls )
	{
		MnClosure* cls = s->cls;
		if ( upvalidx > 0 && upvalidx <= cls->upvals.size() )
		{
			MnUpval* upval = MnToUpval(cls->upvals[upvalidx - 1]);
			*upval->link = v;
			return ;
		}
	}
}

MnValue ut_getupval( MnState* s, MnIndex upvalidx )
{
	MnUpval* upval = ut_getupval_ptr( s, upvalidx );
	return (upval? *(upval->link) : MnValue());
}

MnUpval* ut_getupval_ptr( MnState* s, MnIndex upvalidx )
{
	if ( s->cls )
	{
		MnClosure* cls = s->cls;
		if ( upvalidx > 0 && upvalidx <= cls->upvals.size() )
		{
			MnValue val = cls->upvals[upvalidx - 1];
			return MnToUpval(val);
		}
	}
	return NULL;
}

OvBool ut_isglobal( MnState* s, OvHash32 hash )
{
	return ( s->global.find( hash ) != s->global.end() );
}

void ut_insertstack(MnState* s, MnIndex idx, MnValue& val )
{
	MnValue* itor = ut_getstack_ptr( s, idx );
	if ( itor && itor < s->top )
	{
		mn_settop(s,mn_gettop(s)+1);
		itor = ut_getstack_ptr( s, (idx>0)? idx:idx-1 );
		MnValue temp = *itor;
		*itor = val;
		while ( ++itor < s->top )
		{
			MnValue next = *itor;
			*itor = temp;
			temp = next;
		}
	}
}

void ut_setfield( MnState* s, MnValue& c, MnValue& n, MnValue& v )
{
	if ( MnIsTable(c) )		 ut_settable( s, c, n, v);
	else if ( MnIsArray(c) ) ut_setarray( s, c, n, v);
	else if ( MnIsUserData(c) || MnIsMiniData(c) )  ut_meta_newindex( s, c, n, v );
}

MnValue ut_getfield( MnState* s, MnValue& c, MnValue& n )
{
	if ( MnIsTable(c) )		return ut_gettable( s, c, n);
	else if ( MnIsArray(c) )return ut_getarray( s, c, n);
	else if ( MnIsUserData(c) || MnIsMiniData(c) ) return ut_meta_index( s, c, n );
	return MnValue();
}

void ut_setmeta( MnValue& c, MnValue& m )
{
	if ( MnIsTable(c) )			MnToTable(c)->metatable = m;
	else if ( MnIsArray(c) )	MnToArray(c)->metatable = m;
	else if ( MnIsUserData(c) ) MnToUserData(c)->metatable = m;
	else if ( MnIsMiniData(c) ) MnToMiniData(c)->metatable = m;
}

MnValue ut_getmeta( const MnValue& c )
{
	if ( MnIsTable(c) )			return MnToTable(c)->metatable;
	else if ( MnIsArray(c) )	return MnToArray(c)->metatable;
	else if ( MnIsUserData(c) ) return MnToUserData(c)->metatable;
	else if ( MnIsMiniData(c) ) return MnToMiniData(c)->metatable;
	return MnValue();
}

MnString* ut_newstring( MnState* s, const OvString& str )
{
	MnString* ret;
	OvHash32 hash = OU::string::rs_hash(str);

	MnState::map_hash_str::iterator itor = s->strtable.find( hash );
	if ( itor == s->strtable.end() )
	{
		ret = new(ut_alloc(sizeof(MnString))) MnString(s, hash, str);
		itor = s->strtable.insert( make_pair( hash, ret ) ).first;
	}
	ret = itor->second;
	return ret;
}

MnUserData* ut_newuserdata( MnState* s, void* p )
{
	return new(ut_alloc(sizeof(MnUserData))) MnUserData(s,p);
}

MnMiniData* ut_newminidata( MnState* s, OvInt sz )
{
	return new(ut_alloc(sizeof(MnMiniData))) MnMiniData(s,sz);
}

MnTable* ut_newtable( MnState* s )
{
	return new(ut_alloc(sizeof(MnTable))) MnTable(s);
}

MnArray* ut_newarray( MnState* s )
{
	return new(ut_alloc(sizeof(MnArray))) MnArray(s);
}

MnClosure* ut_newCclosure( MnState* s )
{
	return new(ut_alloc(sizeof(MnClosure))) MnClosure(s,CCL);
}

MnClosure* ut_newMclosure( MnState* s )
{
	return new(ut_alloc(sizeof(MnClosure))) MnClosure(s,MCL);
}

MnUpval*  ut_newupval( MnState* s )
{
	return new(ut_alloc(sizeof(MnUpval))) MnUpval(s);
}

MnMFunction* ut_newfunction( MnState* s )
{
	return new(ut_alloc(sizeof(MnMFunction))) MnMFunction(s);
}

void ut_delete_object( MnObject* o )
{
	if ( o )
	{
		o->~MnObject();

		if (o->next) o->next->prev = o->prev;
		if (o->prev) o->prev->next = o->next;
		else o->state->heap = o->next;

		ut_free((void*)o);
	}
}

void ut_delete_garbage( MnObject* o )
{
	if ( o )
	{
		o->~MnObject();

		ut_free((void*)o);
	}
}


void ut_ensure_stack( MnState* s, OvInt idx )
{
	OvSize sz = idx + (s->stack.size()? ( s->base - s->begin ) : 0);
	if ( sz > s->stack.size() )
	{
		MnValue* oldstack = s->begin;
		s->stack.resize( sz );
		s->begin	= &(s->stack[0]);
		s->end		= &(s->stack[s->stack.size() - 1]);
		s->base		= (s->base - oldstack) + s->begin;
		s->top		= (s->top - oldstack) + s->begin;
		ut_correct_upval( s, oldstack );
	}
}

//////////////////////////////////////////////////////////////////////////

OvInt ut_stack_size( MnState* s )
{
	return s->stack.size();
}

void ut_pushvalue( MnState* s, const MnValue& v )
{
	MnIndex top = mn_gettop(s);
	mn_settop( s, ++top );
	ut_setstack( s, top, v );
}

OvBool ut_toboolean( MnValue& val ) 
{
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

MnNumber ut_tonumber( const MnValue& val ) 
{
	if ( MnIsNumber(val) )
	{
		return MnToNumber(val);
	}
	else if ( MnIsString(val) )
	{
		MnNumber num; 
		if (ut_str2num( MnToString(val)->str(), num ) ) return num;;
	}
	else if ( MnIsBoolean(val) )
	{
		return MnToBoolean(val)? 1.0 : 0.0;
	}
	return 0;
}

OvString ut_tostring( const MnValue& val ) 
{
	if ( MnIsString(val) )
	{
		return MnToString(val)->str();
	}
	else if ( MnIsNumber(val) )
	{
		OvString str;
		ut_num2str( MnToNumber(val), str );
		return str;
	}
	else if ( MnIsBoolean(val) )
	{
		return (MnToBoolean(val)? "true":"false");
	}
	static OvString empty;
	return empty;
}

void* ut_touserdata( MnValue& val )
{
	if ( MnIsUserData(val) )
	{
		return MnToUserData(val)->ptr;
	}
	return NULL;
}

void* ut_tominidata( MnValue& val )
{
	if ( MnIsMiniData(val) )
	{
		return MnToMiniData(val)->ptr;
	}
	return NULL;
}

OvInt ut_type( const MnValue& v )
{
	if ( v.type >= MOT_TYPEMAX || v.type < MOT_UNKNOWN )
	{
		return MOT_UNKNOWN;
	}
	return v.type;
}

const OvChar* ut_typename( const MnValue& v )
{
	return g_type_str[ ut_type(v) ].str;
}

OvInt ex_stack_size( MnState* s )
{
	mn_pushnumber( s, ut_stack_size(s) );
	return 1;
}

OvInt ex_global_length(MnState* s)
{
	MnNumber nsize = 0.0;
	MnValue arg1 = ut_getstack(s,1);
	if ( MnIsArray(arg1) )
	{
		mn_pushnumber( s, (MnNumber)MnToArray(arg1)->array.size() );
	}
	else if ( MnIsTable(arg1) )
	{
		mn_pushnumber( s, (MnNumber)MnToTable(arg1)->table.size() );
	}
	else
	{
		mn_pushnil( s );
	}
	return 1;
}

OvInt ex_global_resize( MnState* s )
{
	MnValue arg1 = ut_getstack(s,1);
	MnValue arg2 = ut_getstack(s,2);
	if ( MnIsArray(arg1) && MnIsNumber(arg2) )
	{
		MnToArray(arg1)->array.resize( (OvSize)MnToNumber(arg2) );
	}
	return 0;
}

OvInt ex_collect_garbage( MnState* s )
{
	mn_pushnumber( s, mn_collect_garbage(s) );
	return 1;
}

OvInt ex_print( MnState* s )
{
	printf( mn_tostring(s,1).c_str() );
	printf( "\n" );
	return 0;
}

OvInt ex_tostring( MnState* s )
{
	mn_pushstring( s, ut_tostring( ut_getstack( s, 1 ) ) );
	return 1;
}

OvInt ex_tonumber( MnState* s )
{
	mn_pushnumber( s, ut_tonumber( ut_getstack( s, 1 ) ) );
	return 1;
}

OvInt ex_dump_stack( MnState* s )
{
	MnValue* itor = s->end;
	printf("\n--stack end--\n");
	while ( s->begin <= itor )
	{
		printf( "%3d : ", itor - s->begin + 1 );
		MnValue& v =*itor--;

		const OvChar* name = ut_typename( v );

		if ( MnIsNumber(v) )		printf( "%s : %d", name, ut_tonumber(v) );
		else if ( MnIsString(v) )	printf( "%s : %s", name, ut_tostring(v).c_str() );
		else if ( MnIsBoolean(v) )	printf( "%s : %s", name, ut_toboolean(v)? "true":"false" );
		else						printf( name );

		printf("\n");
	}
	printf("--stack begin--\n");
	return 0;
}

OvInt ex_table( MnState* s )
{
	ut_pushvalue( s, MnValue( MOT_TABLE,ut_newtable(s) ) );
	return 1;
}

//////////////////////////////////////////////////////////////////////////

OvInt ex_array_new( MnState* s )
{
	ut_pushvalue( s, MnValue( MOT_ARRAY,ut_newarray(s) ) );
	mn_pushstring(s,"array");
	mn_getglobal(s);
	mn_setmeta(s,-2);
	return 1;
}

OvInt ex_array_resize( MnState* s )
{
	if ( MnArray* arr = MnToArray(ut_getstack(s,1)) )
	{
		MnNumber sz = mn_tonumber(s,2);
		arr->array.resize(sz);
	}
	return 0;
}

OvInt ex_array_size( MnState* s )
{
	if ( MnArray* arr = MnToArray(ut_getstack(s,1)) )
	{
		mn_pushnumber(s,arr->array.size());
	}
	return 1;
}

//////////////////////////////////////////////////////////////////////////

OvInt ex_setmeta( MnState* s )
{
	ut_setmeta( ut_getstack(s,1), ut_getstack(s,2) );
	return 0;
}

//////////////////////////////////////////////////////////////////////////

MnValue ut_meta_call( MnState* s, MnValue& c ) 
{
	MnValue meta = ut_getmeta( c );
	if ( !MnIsNil(meta) )
	{
		ut_pushvalue( s, meta );	//< push meta
		mn_pushstring( s, METHOD_CALL );
		mn_getfield( s, -2 );		//< get _call method
		if ( mn_isfunction( s, -1 ) )
		{
			MnValue call = ut_getstack( s, -1 );
			mn_pop(s,2);
			return call;
		}
		return ut_meta_call(s,meta);
	}
	return MnValue();
}

//////////////////////////////////////////////////////////////////////////
void ut_restore_ci( MnState* s, OvInt nret ) 
{
	ut_close_upval( s, s->base );
	MnValue* vfunc		= s->base;
	MnValue* first_ret  = s->top - nret;
	first_ret = max( vfunc, first_ret );
	if ( nret > 0 ) for ( OvInt i = 0 ; i < nret ; ++i )  (*vfunc++) = (*first_ret++);

	MnValue* newtop = vfunc + nret;
	while ( vfunc < newtop ) (*vfunc++) = MnValue();
	while ( newtop < s->top ) *(--s->top) = MnValue();

	MnCallInfo* ci = s->ci;
	s->top	= ci->top + s->begin;
	s->base = ci->base + s->begin;
	s->pc	= ci->pc;
	s->ci	= ci->prev;
	s->cls  = ci->cls;
	s->func = ci->func;
	ut_free(ci);
}

// MnValue ut_pair_param_method( MnState* s, const OvString& method, const MnValue& a, const MnValue b )
// {
// 	ut_pushvalue( s, ut_getmeta(a) );
// 	mn_pushstring( s, method );
// 	mn_getfield( s, -2 );
// 	if ( mn_isnil( s, -1 ) ) { mn_pop(s,1); return MnValue(); }
// 
// 	ut_pushvalue( s, a );
// 	ut_pushvalue( s, b );
// 	mn_call( s, 2, 1 );
// 	MnValue ret = ut_getstack( s, -1 );
// 	mn_pop(s,1);
// 	return ret;
// }


/*
,
,



op_move,

op_getupval,
op_setupval,

op_getfield,
op_setfield,

op_getglobal,
op_setglobal,

op_newclosure,

op_close_upval,



op_call,
op_return,
*/
enum opcode : OvByte
{
	op_none = 0,
	op_add,
	op_sub,
	op_mul,
	op_div,
	op_mod,

	op_push,
	op_pull,

	op_bit_or,
	op_bit_xor,
	op_bit_and,

	op_or,
	op_and,

	op_eq,
	op_not,
	op_lt,
	op_gt,

	op_setglobal,
	op_getglobal,

	op_popstack,
	op_setstack,
	op_getstack,

	op_setfield,
	op_getfield,

	op_setupval,
	op_getupval,

	op_const_true,
	op_const_false,
	op_const_nil,
	op_const_zero,
	op_const_one,
	op_const_num,
	op_const_char,
	op_const,

	op_newclosure,
	op_settop,

	op_call,
	op_jump,
	op_fjump,

	op_return,
};


struct MnCodeWriter
{
	MnMFunction* func;
	MnCodeWriter( MnMFunction* f ) : func(f) {};

	template<typename T>
	MnCodeWriter& operator << ( const T& data )
	{
		OvUInt pos = func->code.size();
		func->code.resize( func->code.size() + sizeof(T) );
		memcpy( &(func->code[pos]), &data, sizeof(T) );
		return *this;
	}
};

struct MnCodeReader
{
	MnState* state;
	MnCodeReader( MnState* s ) : state(s) {}

	template<typename T>
	MnCodeReader& operator >> ( T& data ) { data = *((T*)state->pc); state->pc += sizeof(T); return *this; }
};

void ut_excute_func( MnState* s, MnMFunction* func )
{
	MnCallInfo* entrycall = s->ci;
	s->func	= func;
	s->pc	= &(func->code[0]);
	MnCodeReader emcee(s);
	while ( true )
	{
		OvByte op;
		emcee >> op;
		switch ( op )
		{
		case op_add : case op_sub : case op_mul : case op_div : case op_mod : 
		case op_push : case op_pull :
		case op_bit_or : case op_bit_xor : case op_bit_and :
		case op_and : case op_or : 
			{
				MnValue left  = ut_getstack(s,-2);
				MnValue right = ut_getstack(s,-1);
				mn_pop(s,2);
				
				if (op==op_add) mn_pushnumber( s, MnToNumber(left) + MnToNumber(right) );
				else if (op==op_sub) mn_pushnumber( s, MnToNumber(left) - MnToNumber(right) );
				else if (op==op_mul) mn_pushnumber( s, MnToNumber(left) * MnToNumber(right) );
				else if (op==op_div) mn_pushnumber( s, MnToNumber(left) / MnToNumber(right) );
				else if (op==op_mod) mn_pushnumber( s, (OvInt)MnToNumber(left) % (OvInt)MnToNumber(right) );
				else if (op==op_push) mn_pushnumber( s, (OvInt)MnToNumber(left) << (OvInt)MnToNumber(right) );
				else if (op==op_pull) mn_pushnumber( s, (OvInt)MnToNumber(left) >> (OvInt)MnToNumber(right) );
				else if (op==op_bit_or) mn_pushnumber( s, (OvInt)MnToNumber(left) | (OvInt)MnToNumber(right) );
				else if (op==op_bit_xor) mn_pushnumber( s, (OvInt)MnToNumber(left) ^ (OvInt)MnToNumber(right) );
				else if (op==op_bit_and) mn_pushnumber( s, (OvInt)MnToNumber(left) & (OvInt)MnToNumber(right) );
				else if (op==op_and) mn_pushboolean( s, ut_toboolean(left) && ut_toboolean(right) );
				else if (op==op_or) mn_pushboolean( s, ut_toboolean(left) || ut_toboolean(right) );
			}
			break;

		case op_eq :
			{
				MnValue left  = ut_getstack(s,-2);
				MnValue right = ut_getstack(s,-1);
				mn_pop(s,2);

				opcode addop;
				emcee >> addop;
				if (addop==op_eq) mn_pushboolean( s, MnToNumber(left) == MnToNumber(right) );
				else if (addop==op_not) mn_pushboolean( s, MnToNumber(left) != MnToNumber(right) );
				else if (addop==op_gt) mn_pushboolean( s, MnToNumber(left) >= MnToNumber(right) );
				else if (addop==op_lt) mn_pushboolean( s, MnToNumber(left) <= MnToNumber(right) );
			}
			break;

		case op_not :
			{
				OvBool ret = mn_toboolean(s,-1);
				mn_pop(s,1);
				mn_pushboolean(s,!ret);
			}
			break;

		case op_gt : case op_lt :
			{
				MnValue left  = ut_getstack(s,-2);
				MnValue right = ut_getstack(s,-1);
				mn_pop(s,2);
				if (op==op_gt) mn_pushboolean( s, MnToNumber(left) > MnToNumber(right) );
				else if (op==op_lt) mn_pushboolean( s, MnToNumber(left) < MnToNumber(right) );
			}
			break;

		case op_setglobal :
		case op_getglobal :
			{
				OvByte idx;
				emcee >> idx;
				if (op==op_setglobal)
				{
					ut_insertstack( s, -1, ut_getconst(s->func,idx) );
					mn_setglobal(s);
				}
				else
				{
					ut_pushvalue( s, ut_getconst(s->func,idx) );
					mn_getglobal(s);
				}
			}
			break;

		case op_popstack :
		case op_setstack :
		case op_getstack :
			{
				OvByte idx;
				emcee >> idx;
				if (op==op_setstack) mn_setstack(s,idx);
				else if (op==op_getstack)mn_getstack(s,idx);
				else mn_pop(s,idx);
			}
			break;


		case op_setfield :
		case op_getfield :
			{
				if (op==op_setfield) 
				{
					ut_setfield(s,ut_getstack(s,-3),ut_getstack(s,-2),ut_getstack(s,-1));
					mn_pop(s,1);
				}
				else
				{
					MnValue val =ut_getfield(s,ut_getstack(s,-2),ut_getstack(s,-1));
					mn_pop(s,2);
					ut_pushvalue(s,val);
				}
			}
			break;
		case op_const_nil : mn_pushnil(s); break;
		case op_const_true : mn_pushboolean(s,true); break;
		case op_const_false : mn_pushboolean(s,false); break;

		case op_const_zero : ut_pushvalue( s, MnValue(0.0) ); break;
		case op_const_one : ut_pushvalue( s, MnValue(1.0) ); break;
		case op_const_num :
			{
				MnNumber num;
				emcee >> num;
				ut_pushvalue( s, MnValue(num) );
			}
			break;
		case op_const :
			{
				OvByte idx;
				emcee >> idx;
				ut_pushvalue( s, ut_getconst(s->func,idx) );
			}
			break;

		case op_newclosure :
			{
				OvByte funcidx, nupvals;
				emcee >> funcidx >> nupvals;
				MnClosure* cls = ut_newMclosure(s);
				cls->u.m->func = ut_getconst(s->func,funcidx);
				ut_pushvalue( s, MnValue( MOT_CLOSURE, cls ) );
			}
			break;

		case op_settop :
			{
				OvInt top;
				emcee >> top;
				mn_settop(s,top);
			}
			break;

		case op_call :
			{
				OvByte nargs,nrets;
				emcee >> nargs >> nrets;
				mn_call( s, nargs, nrets );
			}
			break;

		case op_jump :
			{
				OvInt step = 0;
				emcee >> step;
				s->pc += step;
			}
			break;

		case op_fjump :
			{
				OvInt step = 0;
				emcee >> step;
				if ( !mn_toboolean(s,-1) ) s->pc += step;
				mn_pop(s,1);
			}
			break;

		case op_return :
			{
				OvByte nrets;
				emcee >> nrets;
				OvBool isend = (entrycall == s->ci);
				ut_restore_ci(s, nrets);
				if ( isend ) return; else break;
			}
		}
	}
}