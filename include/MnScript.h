#pragma once
#include "OvTypeDef.h"

typedef OvInt	MnIndex;
class MnState;
typedef OvInt (*MnCFunction)(MnState*);

MnState*		mn_open_state();
void			mn_close_state( MnState* s );
void			mn_default_lib( MnState* s );

void			mn_set_stack( MnState* s, MnIndex idx );
void			mn_get_stack( MnState* s, MnIndex idx );

void			mn_set_field( MnState* s, MnIndex idx );
void			mn_get_field( MnState* s, MnIndex idx );

#define			mn_set_global(s) mn_set_field((s),0)
#define			mn_get_global(s) mn_get_field((s),0)

void			mn_set_metatable( MnState* s, MnIndex idx );
void			mn_get_metatable( MnState* s, MnIndex idx );

void			mn_set_upval( MnState* s, MnIndex clsidx, MnIndex upvalidx );
void			mn_get_upval( MnState* s, MnIndex clsidx, MnIndex upvalidx );

void			mn_set_top( MnState* s, MnIndex idx );
MnIndex			mn_get_top( MnState* s );
#define			mn_pop(s,n) mn_set_top( (s), -(n)-1 )

void			mn_new_table( MnState* s );
void			mn_new_array( MnState* s );
void			mnd_new_garbege( MnState* s );
void			mn_new_closure( MnState* s, MnCFunction proto, OvInt nupvals );
void			mn_push_function( MnState* s, MnCFunction proto );
void			mn_push_nil( MnState* s );
void			mn_push_boolean( MnState* s, OvBool v );
void			mn_push_number( MnState* s, OvReal v );
void			mn_push_string( MnState* s, const OvString& v );
void			mn_push_stack( MnState* s, MnIndex idx );

OvBool			mn_is_nil( MnState* s, MnIndex idx );
OvBool			mn_is_boolean( MnState* s, MnIndex idx );
OvBool			mn_is_number( MnState* s, MnIndex idx );
OvBool			mn_is_string( MnState* s, MnIndex idx );

OvBool			mn_to_boolean( MnState* s, MnIndex idx );
OvReal			mn_to_number( MnState* s, MnIndex idx );
const OvString&	mn_to_string( MnState* s, MnIndex idx );

void			mn_call( MnState* s, OvInt nargs, OvInt nrets );

void			mn_collect_garbage( MnState* s );


void mn_load_asm( MnState* s, const OvString& file, MnIndex idx );