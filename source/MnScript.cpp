#include "MnScript.h"
#include "OvMemObject.h"
#include "OvSolidString.h"
#include "OvFile.h"

#define	MOT_UNKNOWN		(0)
#define	MOT_UPVAL		(10)
#define	MOT_FUNCPROTO	(11)

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
struct MnInstruction;

typedef OvUShort MnOperand;

enum MnCLType { CCL= 1, MCL = 2 };
struct MnTypeStr { OvInt type; const char* str; };
const MnTypeStr g_type_str[] = 
{
	{ MOT_UNKNOWN, "unknown" },
	{ MOT_NIL, "nil" },
	{ MOT_BOOLEAN, "boolean" },
	{ MOT_NUMBER, "number" },
	{ MOT_STRING, "string" },

	{ MOT_TABLE, "table" },
	{ MOT_ARRAY, "array" },
	{ MOT_CLOSURE, "closure" },
	{ MOT_USERDATA, "userdata" },
	{ MOT_MINIDATA, "minidata" },
	{ MOT_UPVAL, "upval" },
	{ MOT_FUNCPROTO, "funcproto" },
};

#define METHOD_EQ ("__eq")
#define METHOD_LT ("__lt")
#define METHOD_GT ("__gt")
#define METHOD_NEWINDEX ("__newindex")
#define METHOD_INDEX ("__index")
#define METHOD_CALL ("__call")
#define METHOD_ADD ("__add")
#define METHOD_SUB ("__sub")
#define METHOD_MUL ("__mul")
#define METHOD_DIV ("__div")

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

	MnCallInfo*	 ci;
	MnInstruction* pc;

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

MnValue			ut_getmeta( MnState* s, const MnValue& c );
void			ut_setmeta( MnState* s, MnValue& c, const MnValue& m );

void			ut_setupval( MnState* s, MnIndex upvalidx,MnValue& v );
MnValue			ut_getupval( MnState* s, MnIndex upvalidx );

void			ut_pushvalue( MnState* s, const MnValue& v );

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
	ut_dec_ref(*this);
	type = v.type;
	u	 = v.u;
	ut_inc_ref(*this);
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
	return ut_str2type( MnIsString(val)? MnToString(val)->get_str().c_str() : "unknown" );
}

////////////////////*    version info    *///////////////////

void mn_version( OvByte& major, OvByte& minor, OvByte& patch )
{
	major = VERSION_MAJOR;
	minor = VERSION_MINOR;
	patch = VERSION_PATCH;
}

////////////////////*    open and close    *///////////////////

MnState* mn_openstate()
{
	MnState* s = new(ut_alloc(sizeof(MnState))) MnState;
	s->begin = s->end = NULL;
	s->base = (MnValue*)-1;
	s->top  = (MnValue*)+0;
	s->ci	= NULL;
	s->pc	= NULL;
	ut_ensure_stack(s,1);
	return s;
}

void mn_closestate( MnState* s )
{
	if ( s )
	{
		while ( s->ci ) { MnCallInfo* ci = s->ci; s->ci = ci->prev; ut_free(ci); }
		s->begin = s->end = s->base = s->top = NULL;
		s->stack.clear();
		s->global.clear();
		s->openeduv.clear();
		s->upvals.clear();
		s->strtable.clear();
		mn_collect_garbage(s);
		ut_free(s);
	}
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

void ut_setglobal( MnState* s, MnValue& n, const MnValue& val )
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
			s->global[hash] = val;
		}
	}
}

MnValue ut_getglobal( MnState* s, MnValue& n )
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

OvBool ut_meta_newindex( MnState* s, MnValue& c, MnValue& n, MnValue& v ) 
{
	ut_pushvalue( s, ut_getmeta( s, c ) );
	mn_pushstring( s, METHOD_NEWINDEX );
	mn_getfield( s, -2 );

	if ( mn_isfunction( s, -1 ) )
	{
		ut_pushvalue( s, c );
		ut_pushvalue( s, n );
		ut_pushvalue( s, v );
		mn_call( s, 3, 0 );
		return true;
	}
	else
	{
		mn_pop( s, 2 );
		return false;
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
			OvHash32 hash = MnToString(n)->get_hash();
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
	ut_pushvalue( s, ut_getmeta( s, c ) );
	mn_pushstring( s, METHOD_INDEX );
	mn_getfield( s, -2 );
	if ( mn_isfunction( s, -1 ) )
	{
		ut_pushvalue( s, c );
		ut_pushvalue( s, n );
		mn_call( s, 2, 1 );

		MnValue ret = ut_getstack( s, -1 );
		mn_pop( s, 1 );
		return ret;
	}
	mn_pop(s,1);
	return MnValue();
}

MnValue ut_gettable( MnState* s, MnValue& t, MnValue& n )
{
	if ( MnIsTable(t) )
	{
		MnTable* tbl = MnToTable(t);
		if ( MnIsString(n) )
		{
			OvHash32 hash = MnToString(n)->get_hash();
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
			MnIndex idx = (MnIndex)MnToNumber(n);
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
			MnIndex idx = (MnIndex)MnToNumber(n);
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
	MnValue c;
	if ( s->base >= s->begin ) c  = *s->base;
	
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

MnValue ut_getupval( MnState* s, MnIndex upvalidx )
{
	MnValue c;
	if ( s->base >= s->begin ) c  = *s->base;

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

OvBool ut_isglobal( MnState* s, OvHash32 hash )
{
	return ( s->global.find( hash ) != s->global.end() );
}

void mn_setstack( MnState* s, MnIndex idx )
{
	ut_setstack( s, idx, ut_getstack(s,-1));
	mn_pop(s,1);
}

void mn_getstack( MnState* s, MnIndex idx )
{
	ut_pushvalue( s, ut_getstack(s,idx) );
}

void ut_insertstack(MnState* s, MnIndex idx, MnValue& val )
{
	MnValue* itor = ut_getstack_ptr( s, idx );;
	if ( itor && itor < s->top )
	{
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

void mn_insertstack( MnState* s, MnIndex idx )
{
	ut_insertstack( s, idx, ut_getstack( s, -1 ) );
}

void mn_removestack( MnState* s, MnIndex idx )
{
	MnValue* itor = ut_getstack_ptr( s, idx );;
	if ( itor && itor > s->base && itor < s->top )
	{
		while ( ++itor < s->top ) *(itor-1) = *itor;
		--s->top;
	}
}

void mn_swapstack( MnState* s, MnIndex idx1, MnIndex idx2 )
{
	MnValue* val1 = ut_getstack_ptr( s, idx1 );
	MnValue* val2 = ut_getstack_ptr( s, idx2 );
	if ( val1 && val2 )
	{
		MnValue& temp = *val1;
		*val1 = *val2;
		*val2 = temp;
	}
}

void mn_replacestack( MnState* s, MnIndex dst, MnIndex src )
{
	MnValue* val = ut_getstack_ptr( s, src );
	if ( val ) ut_setstack( s, dst, *val );
}

void ut_setfield( MnState* s, MnValue& c, MnValue& n, MnValue& v )
{
	if ( MnIsTable(c) )		 ut_settable( s, c, n, v);
	else if ( MnIsArray(c) ) ut_setarray( s, c, n, v);
	else if ( MnIsUserData(c) || MnIsMiniData(c) )  ut_meta_newindex( s, c, n, v );
}
void mn_setfield( MnState* s, MnIndex idx )
{
	if ( idx && mn_gettop(s) >= 2 )
	{
		if ( idx )	ut_setfield( s, ut_getstack(s,idx), ut_getstack(s,-2), ut_getstack(s,-1) );
		mn_pop(s,2);
	}
}

MnValue ut_getfield( MnState* s, MnValue& c, MnValue& n )
{
	if ( MnIsTable(c) )		return ut_gettable( s, c, n);
	else if ( MnIsArray(c) )return ut_getarray( s, c, n);
	else if ( MnIsUserData(c) && MnIsMiniData(c) ) return ut_meta_index( s, c, n );
	return MnValue();
}

void mn_getfield( MnState* s, MnIndex idx )
{
	if ( idx && mn_gettop(s) >= 1 )
	{
		MnValue val;
		if ( idx )	val = ut_getfield( s, ut_getstack(s,idx), ut_getstack(s,-1) );
		mn_pop(s,1);
		ut_pushvalue( s, val );
	}
}

void mn_setglobal( MnState* s )
{
	if ( mn_gettop(s) >= 2 )
	{
		ut_setglobal( s, ut_getstack(s,-2), ut_getstack(s,-1));
		mn_pop(s,2);
	}
}

void mn_getglobal( MnState* s )
{
	if (  mn_gettop(s) >= 1 )
	{
		MnValue val = ut_getglobal( s, ut_getstack(s,-1) );
		mn_pop(s,1);
		ut_pushvalue( s, val );
	}
}

void ut_setmeta( MnState* s, MnValue& c, MnValue& m )
{
	if ( MnIsTable(c) )			MnToTable(c)->metatable = m;
	else if ( MnIsArray(c) )	MnToArray(c)->metatable = m;
	else if ( MnIsUserData(c) ) MnToUserData(c)->metatable = m;
	else if ( MnIsMiniData(c) ) MnToMiniData(c)->metatable = m;
}

void mn_setmeta( MnState* s, MnIndex idx )
{
	MnValue v = ut_getstack(s,idx);
	MnValue m = ut_getstack(s,-1);
	ut_setmeta(s,v,m);
	mn_pop(s,1);
}

MnValue ut_getmeta( MnState* s, const MnValue& c )
{
	if ( MnIsTable(c) )			return MnToTable(c)->metatable;
	else if ( MnIsArray(c) )	return MnToArray(c)->metatable;
	else if ( MnIsUserData(c) ) return MnToUserData(c)->metatable;
	else if ( MnIsMiniData(c) ) return MnToMiniData(c)->metatable;
	return MnValue();
}

void mn_getmeta( MnState* s, MnIndex idx )
{
	ut_pushvalue( s, ut_getmeta(s,ut_getstack(s,idx)) );
}

void mn_setupval( MnState* s, MnIndex upvalidx )
{
	ut_setupval(s, upvalidx, ut_getstack(s,-1));
}

void mn_getupval( MnState* s, MnIndex upvalidx )
{
	ut_pushvalue(s, ut_getupval(s, upvalidx ) );
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


void ut_ensure_stack( MnState* s, OvInt sz )
{
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

///////////////////////* get/set top *///////////////////////

void mn_settop( MnState* s, MnIndex idx )
{
	MnValue* newtop = s->top;
	if ( idx < 0 )
	{
		newtop = s->top + ( idx + 1 );
		if ( newtop <= s->base ) newtop = s->base + 1;
	}
	else if ( idx >= 0 )
	{
		OvSize sz = idx + (s->base - s->begin + 1);
		if ( ut_stack_size(s) < sz ) ut_ensure_stack( s, sz );
		newtop = s->base + ( idx + 1 );
	}

	if ( s->top > newtop ) while ( s->top != newtop ) *(--s->top) = MnValue();
	s->top = newtop;
}

MnIndex mn_gettop( MnState* s )
{
	return (s->top - s->base) - 1;
}

///////////////////////*   kind of push    *//////////////////////

void ut_pushvalue( MnState* s, const MnValue& v )
{
	MnIndex top = mn_gettop(s);
	mn_settop( s, ++top );
	ut_setstack( s, top, v );
}

void mn_newtable( MnState* s )
{
	ut_pushvalue( s, MnValue( MOT_TABLE, ut_newtable(s) ) );
}

void mn_newarray( MnState* s )
{
	ut_pushvalue( s, MnValue( MOT_ARRAY, ut_newarray(s) ) );
}

void* mn_newminidata( MnState* s, OvInt sz )
{
	MnMiniData* mini = ut_newminidata(s,sz);
	ut_pushvalue( s, MnValue( MOT_MINIDATA, mini ) );
	return mini->ptr;
}

void mn_newclosure( MnState* s, MnCFunction proto, OvInt nupvals )
{
	MnClosure* cl = ut_newCclosure(s);
	cl->u.c->func = proto;
	cl->upvals.reserve(nupvals);
	for ( OvInt i = 0 ; i < nupvals ; ++i )
	{
		cl->upvals.push_back( ut_getstack(s, i - nupvals ) );
	}
	mn_pop(s,nupvals);
	ut_pushvalue( s, MnValue(MOT_CLOSURE,cl) );
}

void mn_pushfunction( MnState* s, MnCFunction proto )
{
	mn_newclosure(s,proto,0);
}

void mn_pushnil( MnState* s )
{
	ut_pushvalue( s, MnValue() );
}

void mn_pushboolean( MnState* s, OvBool v )
{
	ut_pushvalue( s, MnValue( (OvBool)v ) );
}

void mn_pushnumber( MnState* s, MnNumber v )
{
	ut_pushvalue( s, MnValue( (MnNumber)v ) );
}

void mn_pushstring( MnState* s, const OvString& v )
{
	ut_pushvalue( s, MnValue( MOT_STRING, ut_newstring( s, v ) ) );
}

void mn_pushuserdata( MnState* s, void* v )
{
	ut_pushvalue( s, MnValue( MOT_USERDATA, ut_newuserdata(s,v) ) );
}

void mn_pushstack( MnState* s, MnIndex idx )
{
	ut_pushvalue( s, ut_getstack(s, idx) );
}

/////////////////////*  all kinds of "is"    *///////////////////////////

OvBool mn_isnil( MnState* s, MnIndex idx )
{
	return MnIsNil( ut_getstack( s, idx ) );
}

OvBool mn_isboolean( MnState* s, MnIndex idx )
{
	return MnIsBoolean( ut_getstack( s, idx ) );
}

OvBool mn_isnumber( MnState* s, MnIndex idx )
{
	return MnIsNumber( ut_getstack( s, idx ) );
}

OvBool mn_isstring( MnState* s, MnIndex idx )
{
	return MnIsString( ut_getstack( s, idx ) );
}

OvBool mn_isfunction( MnState* s, MnIndex idx )
{
	return MnIsClosure( ut_getstack( s, idx ) );
}

OvBool mn_isuserdata( MnState* s, MnIndex idx )
{
	return MnIsUserData( ut_getstack( s, idx ) );
}

OvBool mn_isminidata( MnState* s, MnIndex idx )
{
	return MnIsMiniData( ut_getstack( s, idx ) );
}

/////////////////////*  all kinds of "to"    *///////////////////////////

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

OvBool mn_toboolean( MnState* s, MnIndex idx )
{
	return ut_toboolean( ut_getstack( s, idx ) );

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
		if (ut_str2num( MnToString(val)->get_str(), num ) ) return num;;
	}
	return 0;
}

MnNumber mn_tonumber( MnState* s, MnIndex idx )
{
	return ut_tonumber( ut_getstack( s, idx ) );

}

OvString ut_tostring( const MnValue& val ) 
{
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

OvString mn_tostring( MnState* s, MnIndex idx )
{
	return ut_tostring( ut_getstack( s, idx ) );
}

void* ut_touserdata( MnValue& val )
{
	if ( MnIsUserData(val) )
	{
		return MnToUserData(val)->ptr;
	}
	return NULL;
}

void* mn_touserdata( MnState* s, MnIndex idx )
{
	return ut_touserdata( ut_getstack( s, idx ) );
}

void* ut_tominidata( MnValue& val )
{
	if ( MnIsMiniData(val) )
	{
		return MnToMiniData(val)->ptr;
	}
	return NULL;
}

void* mn_tominidata( MnState* s, MnIndex idx )
{
	return ut_tominidata( ut_getstack( s, idx ) );
}

OvInt mn_type( MnState* s, MnIndex idx )
{
	return ut_getstack( s, idx ).type;
}

OvInt mn_collect_garbage( MnState* s )
{
	for ( MnState::map_hash_val::iterator itor = s->global.begin()
		; itor != s->global.end()
		; ++itor )
	{
		MnMarking(itor->second);
	}
	MnValue* itor = s->top;
	while ( itor && --itor >= s->begin ) MnMarking((*itor));

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

		ut_delete_garbage(dead);

		dead = next;
	}
	return num;
}

OvInt ex_ensure_stack( MnState* s )
{
	if ( mn_isnumber( s, 1 ) )
	{
		ut_ensure_stack( s, mn_tonumber(s,1) );
	}
	return 0;
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
		if ( MnIsNumber(v) )		printf( "[number] : %d", ut_tonumber(v) );
		else if ( MnIsString(v) )	printf( "[string] : %s", ut_tostring(v).c_str() );
		else if ( MnIsTable(v) )	printf( "[table]" );
		else if ( MnIsArray(v) )	printf( "[array]" );
		else if ( MnIsClosure(v) )	printf( "[closure]" );
		else if ( MnIsFunction(v) )	printf( "[function]" );
		else if ( MnIsUserData(v) )		printf( "[userdata]" );
		else if ( MnIsUpval(v) )	printf( "[upval]" );
		else if ( MnIsNil(v) )		printf( "[nil]" );
		else if ( MnIsBoolean(v) )	printf( "[boolean] : %s", ut_toboolean(v)? "true":"false" );
		else						printf("[unknown]");
		
		printf("\n");
	}
	printf("--stack begin--\n");
	return 0;
}

OvInt ex_do_asm( MnState* s )
{
	mn_do_asm( s, mn_tostring(s,1), 0 );
	return 0;
}

void mn_lib_default( MnState* s )
{
	mn_pushstring( s, "length" );
	mn_pushfunction( s, ex_global_length );
	mn_setglobal( s );

	mn_pushstring( s, "resize" );
	mn_pushfunction( s, ex_global_resize );
	mn_setglobal( s );

	mn_pushstring( s, "collect_garbage" );
	mn_pushfunction( s, ex_collect_garbage );
	mn_setglobal( s );

	mn_pushstring( s, "print" );
	mn_pushfunction( s, ex_print);
	mn_setglobal( s );

	mn_pushstring( s, "tostring" );
	mn_pushfunction( s, ex_tostring);
	mn_setglobal( s );

	mn_pushstring( s, "tonumber" );
	mn_pushfunction( s, ex_tonumber);
	mn_setglobal( s );

	mn_pushstring( s, "dump_stack" );
	mn_pushfunction( s, ex_dump_stack);
	mn_setglobal( s );

	mn_pushstring( s, "do_asm" );
	mn_pushfunction( s, ex_do_asm);
	mn_setglobal( s );

	mn_pushstring( s, "ensure_stack" );
	mn_pushfunction( s, ex_ensure_stack);
	mn_setglobal( s );

	mn_pushstring( s, "stack_size" );
	mn_pushfunction( s, ex_stack_size);
	mn_setglobal( s );
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
	MOP_RET,

	MOP_ADD,
	MOP_SUB,
	MOP_MUL,
	MOP_DIV,

	MOP_CALL,

	MOP_LOG,
};
//////////////////////////////////////////////////////////////////////////

MnValue ut_method_arith( MnState* s, MnOperate op, const MnValue& a, const MnValue& b )
{
	if ( MnIsNumber(a) )
	{
		MnNumber ret = MnToNumber(a);
		if ( op == MOP_ADD ) ret += ut_tonumber(b);
		else if ( op == MOP_SUB ) ret -= ut_tonumber(b);
		else if ( op == MOP_MUL ) ret *= ut_tonumber(b);
		else if ( op == MOP_DIV ) ret /= ut_tonumber(b);
		return MnValue( ret );
	}
	else if ( MnIsString(a) )
	{
		OvString ret = MnToString(a)->get_str();
		if ( op == MOP_ADD ) ret += ut_tostring(b);
		return MnValue( MOT_STRING, ut_newstring( s, ret ) );
	}
	else
	{
		OvString method =	(op == MOP_ADD)? METHOD_ADD : (op == MOP_SUB)? METHOD_SUB : (op == MOP_MUL)? METHOD_MUL : (op == MOP_DIV)? METHOD_DIV : "";
		ut_pushvalue( s, ut_getmeta( s, a ) );
		mn_pushstring(s,method);
		mn_getfield(s,-2);

		ut_pushvalue( s, a );
		ut_pushvalue( s, b );
		mn_call(s,2,1);

		MnValue ret = ut_getstack( s, -1 );
		mn_pop(s,2);

		return ret;
	}
	return MnValue();
}

//////////////////////////////////////////////////////////////////////////

MnValue ut_method_logical( MnState* s,MnOperate op, MnValue& a, MnValue& b )
{
	if ( MnIsNumber(a) && MnIsNumber(b) )
	{
		if ( op == MOP_EQ ) return MnValue((OvBool)(ut_tonumber(a) == ut_tonumber(b)));
		else if ( op == MOP_LT ) return MnValue((OvBool)(ut_tonumber(a) < ut_tonumber(b)));
		else if ( op == MOP_GT ) return MnValue((OvBool)(ut_tonumber(a) > ut_tonumber(b)));
	}
	else if ( MnIsString(a) && MnIsString(b) )
	{
		if ( op == MOP_EQ ) return MnValue( (OvBool)( ut_tostring(a) == ut_tostring(b) ) );
	}
	else
	{
		OvString method =	(op == MOP_EQ)? METHOD_EQ : (op == MOP_LT)? METHOD_LT : (op == MOP_GT)? METHOD_GT : "";
		ut_pushvalue( s, ut_getmeta( s, a ) );
		mn_pushstring(s,method);
		mn_getfield(s,-2);

		ut_pushvalue( s, a );
		ut_pushvalue( s, b );
		mn_call(s,2,1);

		MnValue ret = ut_getstack( s, -1 );
		mn_pop(s,2);

		return ret;
	}
	return MnValue();
}

//////////////////////////////////////////////////////////////////////////

OvInt ut_exec_func( MnState* s, MnMFunction* func )
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
		case MOP_NEWTABLE:	mn_newtable( s ); break;
		case MOP_NEWARRAY:	mn_newarray( s ); break;
		case MOP_NEWCLOSURE:
			{
				MnClosure* cls = ut_newMclosure(s);
				MnClosure::MClosure* mcls = cls->u.m;
				mcls->func = func->consts[i.ax-1];
				cls->upvals.resize( i.bx );
				ut_pushvalue(s,MnValue(MOT_CLOSURE,cls));
			}
			break;

		case MOP_SET_STACK:	mn_setstack( s, i.eax ); break;
		case MOP_GET_STACK:	mn_getstack( s, i.eax ); break;
		case MOP_INSERT_STACK:	mn_insertstack( s, i.eax ); break;

		case MOP_SET_FIELD:	mn_setfield( s, i.eax); break;
		case MOP_GET_FIELD:	mn_getfield( s, i.eax); break;

		case MOP_SET_GLOBAL:	mn_setglobal( s ); break;
		case MOP_GET_GLOBAL:	mn_getglobal( s ); break;

		case MOP_SET_META:	mn_setmeta( s, i.eax ); break;
		case MOP_GET_META:	mn_getmeta( s, i.eax ); break;

		case MOP_SET_UPVAL:	mn_setupval( s, i.eax ); break;
		case MOP_GET_UPVAL:	mn_getupval( s, i.eax ); break;

		case MOP_PUSH:		ut_pushvalue( s, func->consts[i.eax-1] ); break;
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
		case MOP_RET: return i.eax;

		case MOP_NOT:
			{
				OvBool b = mn_toboolean(s,-1);
				mn_pop(s,1);
				mn_pushboolean(s,!b);
			}
			break;

		case MOP_CMP:
			{
				OvBool b = mn_toboolean(s,-1);
				mn_pop(s,1);
				if ( !b ) ++s->pc;
			}
			break;

		case MOP_EQ:
		case MOP_LT:
		case MOP_GT:
			{
				MnValue ret = ut_method_logical( s, (MnOperate)i.op, ut_getstack( s, -2 ), ut_getstack( s, -1 ) );
				mn_pop(s,2);
				ut_pushvalue(s,ret);
			}
			break;

		case MOP_ADD:
		case MOP_SUB:
		case MOP_MUL:
		case MOP_DIV:
			{
				MnValue ret = ut_method_arith( s, (MnOperate)i.op, ut_getstack( s, -2 ), ut_getstack( s, -1 ) );
				mn_pop(s,2);
				ut_pushvalue(s,ret);
			}
			break;

		case MOP_NONEOP:
		default:
			return 0;
		}
	}
	return 0;
}

MnValue ut_meta_call( MnState* s, MnValue& c ) 
{
	MnValue meta = ut_getmeta( s, c );
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

void mn_call( MnState* s, OvInt nargs, OvInt nrets )
{
	nargs = max(nargs,0);
	MnIndex funcidx = -(1 + nargs);
	MnValue* func = ut_getstack_ptr(s, funcidx );

	if ( func && !MnIsClosure(*func) )
	{
		ut_setstack( s, funcidx, ut_meta_call( s, *func ) );
		func = ut_getstack_ptr(s, funcidx );
	}

	MnCallInfo* ci = ( MnCallInfo* )ut_alloc( sizeof( MnCallInfo ) );
	ci->prev	= s->ci;
	ci->savepc	= s->pc;
	ci->base	= s->base - s->begin;

	s->ci    = ci;
	s->base  = func;

	OvInt r = 0;
	if ( func && MnIsClosure(*func) )
	{
		MnClosure* cls = MnToClosure(*func);
		ci->cls  = cls;
		if ( cls->type == CCL )
		{
			MnClosure::CClosure* ccl = cls->u.c;
			r = ccl->func(s);
		}
		else
		{
			MnClosure::MClosure* mcl = cls->u.m;
			r = ut_exec_func( s, MnToFunction( mcl->func ) );
		}
	}

	ut_close_upval( s, s->base );
	ut_ensure_stack( s, (s->base - s->begin) + max( nrets, r ) );

	func = s->base;
	MnValue* newtop = func + nrets;
	MnValue* first_ret  = s->top - r;
	first_ret = max( func, first_ret );

	if ( r > 0 ) for ( OvInt i = 0 ; i < r ; ++i )  (*func++) = (*first_ret++);

	mn_settop( s, nrets - 1 );
	while ( func < newtop ) (*func++) = MnValue();
	while ( newtop < s->top ) *(--s->top) = MnValue();

	ci = s->ci;
	s->top	= newtop;
	s->base = ci->base + s->begin;
	s->pc	= ci->savepc;
	s->ci	= ci->prev;
	ut_free(ci);
}

struct MnCompileState : public OvMemObject
{
	MnState* state;
	OvInputStream* is;
	OvChar c;
	MnValue	errfunc;
};

void cp_build_func( MnCompileState* cs, MnMFunction* func );
void mn_do_asm( MnState* s, const OvString& file, MnIndex idx )
{
	OvFileInputStream fis( file );
	MnCompileState cs;
	cs.state = s;
	cs.is = &fis;
	cs.is->Read(cs.c);
	cs.errfunc = ut_getstack(s,idx);

	MnMFunction* func = ut_newfunction(s);
	cp_build_func(&cs,func);
	MnClosure* cls = ut_newMclosure( s );
	cls->u.m->func = MnValue(MOT_FUNCPROTO,func);
	ut_pushvalue(s,MnValue(MOT_CLOSURE,cls));
	mn_call(s,0,0);
}

enum eErrCode
{
	eErrCode_Unknown,
};

void cp_call_errfunc( MnCompileState* cs, OvInt errcode, const OvString& msg )
{
	ut_pushvalue( cs->state, cs->errfunc );
	mn_pushnumber( cs->state, errcode );
	mn_pushstring( cs->state, msg );
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
	else if ( str == "ret" )   return MOP_RET;

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
			for ( MnIndex i = 0 ; i < func->consts.size() ; ++i )
			{
				MnValue& val = func->consts[i];
				if ( MnIsBoolean(val) && MnToBoolean(val) == (str=="true") ) return (i+1);
			}
			func->consts.push_back( MnValue( (OvBool)(str=="true") ) );
		}
		else if ( str == "nil" )
		{
			for ( MnIndex i = 0 ; i < func->consts.size() ; ++i )
			{
				MnValue& val = func->consts[i];
				if ( MnIsNil(val) ) return (i+1);
			}
			func->consts.push_back( MnValue() );
		}
		return func->consts.size();
	}
	else if ( tok == eTString )
	{
		for ( MnIndex i = 0 ; i < func->consts.size() ; ++i )
		{
			MnValue& val = func->consts[i];
			if ( MnIsString(val) && MnToString(val)->get_str() == str ) return (i+1);
		}
		func->consts.push_back( MnValue( MOT_STRING, ut_newstring(cs->state,str) ) );
		return func->consts.size();
	}
	else if ( tok == eTNumber )
	{
		for ( MnIndex i = 0 ; i < func->consts.size() ; ++i )
		{
			MnValue& val = func->consts[i];
			if ( MnIsNumber(val) && MnToNumber(val) == num ) return (i+1);
		}
		func->consts.push_back( MnValue( (MnNumber)num ) );
		return func->consts.size();
	}
	else if ( (OvChar)tok == ':' )
	{
		do tok = cp_token(cs,num,str); while ( isspace((OvChar)tok) );
		if ( str == "func" )
		{
			MnMFunction* proto = ut_newfunction(cs->state);
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
		case MOP_RET:
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
