#include "MnScript.h"
#include "MnInternal.h"

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
	int a = sizeof(MnInstruction);
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

void mn_setstack( MnState* s, MnIndex idx )
{
	ut_setstack( s, idx, ut_getstack(s,-1));
	mn_pop(s,1);
}

void mn_getstack( MnState* s, MnIndex idx )
{
	ut_pushvalue( s, ut_getstack(s,idx) );
}

void mn_insert( MnState* s, MnIndex idx )
{
	ut_insertstack( s, idx, ut_getstack( s, -1 ) );
}

void mn_remove( MnState* s, MnIndex idx )
{
	MnValue* itor = ut_getstack_ptr( s, idx );;
	if ( itor && itor > s->base && itor < s->top )
	{
		while ( ++itor < s->top ) *(itor-1) = *itor;
		--s->top;
	}
}

void mn_swap( MnState* s, MnIndex idx1, MnIndex idx2 )
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

void mn_replace( MnState* s, MnIndex dst, MnIndex src )
{
	MnValue* val = ut_getstack_ptr( s, src );
	if ( val ) ut_setstack( s, dst, *val );
}

void mn_setfield( MnState* s, MnIndex idx )
{
	if ( idx && mn_gettop(s) >= 2 )
	{
		if ( idx )	ut_setfield( s, ut_getstack(s,idx), ut_getstack(s,-2), ut_getstack(s,-1) );
		mn_pop(s,2);
	}
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

void mn_setmeta( MnState* s, MnIndex idx )
{
	MnValue v = ut_getstack(s,idx);
	MnValue m = ut_getstack(s,-1);
	ut_setmeta(s,v,m);
	mn_pop(s,1);
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

OvBool mn_toboolean( MnState* s, MnIndex idx )
{
	return ut_toboolean( ut_getstack( s, idx ) );

}

MnNumber mn_tonumber( MnState* s, MnIndex idx )
{
	return ut_tonumber( ut_getstack( s, idx ) );

}


OvString mn_tostring( MnState* s, MnIndex idx )
{
	return ut_tostring( ut_getstack( s, idx ) );
}

void* mn_touserdata( MnState* s, MnIndex idx )
{
	return ut_touserdata( ut_getstack( s, idx ) );
}

void* mn_tominidata( MnState* s, MnIndex idx )
{
	return ut_tominidata( ut_getstack( s, idx ) );
}
OvInt mn_type( MnState* s, MnIndex idx )
{
	return ut_type(ut_getstack( s, idx ));
}

OvString mn_typename( MnState* s, MnIndex idx )
{
	return ut_typename( ut_getstack( s, idx ) );
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
			r = cp_exec_func( s, MnToFunction( mcl->func ) );
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
