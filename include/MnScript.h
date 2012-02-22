#pragma once
#include "OvTypeDef.h"

/* Mini Script version_lite */

class MnState;

typedef OvReal	MnNumber;
typedef OvInt	MnIndex;
typedef OvInt (*MnCFunction)(MnState*);

#define	MOT_UNKNOWN		(0)
#define	MOT_NIL			(1)
#define	MOT_BOOLEAN		(2)
#define	MOT_NUMBER		(3)
#define	MOT_STRING		(4)
#define	MOT_TABLE		(5)
#define	MOT_ARRAY		(6)
#define	MOT_CLOSURE		(7)
#define	MOT_USERDATA	(8)
#define	MOT_MINIDATA	(9)

void			mn_version( OvByte& major, OvByte& minor, OvByte& patch );

MnState*		mn_openstate();
void			mn_closestate( MnState* s );
void			mn_lib_default( MnState* s );

void			mn_setstack( MnState* s, MnIndex idx );
void			mn_getstack( MnState* s, MnIndex idx );

void			mn_insert( MnState* s, MnIndex idx );
void			mn_remove( MnState* s, MnIndex idx );
void			mn_swap( MnState* s, MnIndex idx1, MnIndex idx2 );
void			mn_replace( MnState* s, MnIndex dst, MnIndex src );

void			mn_setfield( MnState* s, MnIndex idx );
void			mn_getfield( MnState* s, MnIndex idx );

void			mn_setglobal( MnState* s );
void			mn_getglobal( MnState* s );

void			mn_setmeta( MnState* s, MnIndex idx );
void			mn_getmeta( MnState* s, MnIndex idx );

void			mn_setupval( MnState* s, MnIndex upvalidx );
void			mn_getupval( MnState* s, MnIndex upvalidx );

void			mn_settop( MnState* s, MnIndex idx );
MnIndex			mn_gettop( MnState* s );
#define			mn_pop(s,n) mn_settop( (s), -(n)-1 )

void			mn_newtable( MnState* s );
void			mn_newarray( MnState* s );
void*			mn_newminidata( MnState* s, OvInt sz );
void			mn_newclosure( MnState* s, MnCFunction proto, OvInt nupvals );

void			mn_pushfunction( MnState* s, MnCFunction proto );
void			mn_pushnil( MnState* s );
void			mn_pushboolean( MnState* s, OvBool v );
void			mn_pushnumber( MnState* s, MnNumber v );
void			mn_pushstring( MnState* s, const OvString& v );
void			mn_pushuserdata( MnState* s, void* v );
void			mn_pushstack( MnState* s, MnIndex idx );

OvBool			mn_isnil( MnState* s, MnIndex idx );
OvBool			mn_isboolean( MnState* s, MnIndex idx );
OvBool			mn_isnumber( MnState* s, MnIndex idx );
OvBool			mn_isstring( MnState* s, MnIndex idx );
OvBool			mn_isfunction( MnState* s, MnIndex idx );
OvBool			mn_isuserdata( MnState* s, MnIndex idx );
OvBool			mn_isminidata( MnState* s, MnIndex idx );

OvBool			mn_toboolean( MnState* s, MnIndex idx );
MnNumber		mn_tonumber( MnState* s, MnIndex idx );
OvString		mn_tostring( MnState* s, MnIndex idx );
void*			mn_touserdata( MnState* s, MnIndex idx );
void*			mn_tominidata( MnState* s, MnIndex idx );

OvInt			mn_type( MnState* s, MnIndex idx );
OvString		mn_typename( MnState* s, MnIndex idx );

void			mn_call( MnState* s, OvInt nargs, OvInt nrets );

OvInt			mn_collect_garbage( MnState* s );


void			mn_do_asm( MnState* s, const OvString& file, MnIndex idx );

//////////////////////////////////////////////////////////////////////////

void			scan_test( const OvString& file );