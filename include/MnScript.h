#pragma once
#include "OvTypeDef.h"

/* Mini Script version_lite */

typedef OvInt	MnIndex;
class MnState;
typedef OvInt (*MnCFunction)(MnState*);

MnState*		mn_openstate();
void			mn_closestate( MnState* s );
void			mn_lib_default( MnState* s );

void			mn_setstack( MnState* s, MnIndex idx );
void			mn_getstack( MnState* s, MnIndex idx );
void			mn_insertstack( MnState* s, MnIndex idx );

void			mn_setfield( MnState* s, MnIndex idx );
void			mn_getfield( MnState* s, MnIndex idx );

#define			mn_setglobal(s) mn_setfield((s),0)
#define			mn_getglobal(s) mn_getfield((s),0)

void			mn_setmeta( MnState* s, MnIndex idx );
void			mn_getmeta( MnState* s, MnIndex idx );

void			mn_setupval( MnState* s, MnIndex upvalidx );
void			mn_getupval( MnState* s, MnIndex upvalidx );

void			mn_settop( MnState* s, MnIndex idx );
MnIndex			mn_gettop( MnState* s );
#define			mn_pop(s,n) mn_settop( (s), -(n)-1 )

void			mn_newtable( MnState* s );
void			mn_newarray( MnState* s );
void			mnd_newgarbege( MnState* s );
void			mn_newclosure( MnState* s, MnCFunction proto, OvInt nupvals );
void			mn_pushfunction( MnState* s, MnCFunction proto );
void			mn_pushnil( MnState* s );
void			mn_pushboolean( MnState* s, OvBool v );
void			mn_pushnumber( MnState* s, OvReal v );
void			mn_pushstring( MnState* s, const OvString& v );
void			mn_pushstack( MnState* s, MnIndex idx );

OvBool			mn_isnil( MnState* s, MnIndex idx );
OvBool			mn_isboolean( MnState* s, MnIndex idx );
OvBool			mn_isnumber( MnState* s, MnIndex idx );
OvBool			mn_isstring( MnState* s, MnIndex idx );

OvBool			mn_toboolean( MnState* s, MnIndex idx );
OvReal			mn_tonumber( MnState* s, MnIndex idx );
OvString		mn_tostring( MnState* s, MnIndex idx );

void			mn_call( MnState* s, OvInt nargs, OvInt nrets );

OvInt			mn_collect_garbage( MnState* s );


void mn_do_asm( MnState* s, const OvString& file, MnIndex idx );