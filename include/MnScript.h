#pragma once
#include "OvTypeDef.h"

typedef OvInt	MnIndex;
class MnState;
typedef OvInt (*MnFunction)(MnState*);

MnState*		mn_open_state();
void			mn_close_state( MnState* s );

void			mn_set_field( MnState* s, MnIndex idx );
void			mn_get_field( MnState* s, MnIndex idx );

void			mn_set_metatable( MnState* s, MnIndex idx );
void			mn_get_metatable( MnState* s, MnIndex idx );

void			mnd_new_garbege( MnState* s );

void			mn_set_top( MnState* s, MnIndex idx );
MnIndex			mn_get_top( MnState* s );
MnIndex			mn_get_gtop( MnState* s );

#define			mn_pop(s,n) mn_set_top( (s), -(n)-1 )

void			mn_new_table( MnState* s );
void			mn_push_nil( MnState* s );
void			mn_push_boolean( MnState* s, OvBool v );
void			mn_push_number( MnState* s, OvReal v );
void			mn_push_string( MnState* s, const OvString& v );
void			mn_push_function( MnState* s, MnFunction proto );

OvBool			mn_is_nil( MnState* s, MnIndex idx );
OvBool			mn_is_boolean( MnState* s, MnIndex idx );
OvBool			mn_is_number( MnState* s, MnIndex idx );
OvBool			mn_is_string( MnState* s, MnIndex idx );

OvBool			mn_to_boolean( MnState* s, MnIndex idx );
OvReal			mn_to_number( MnState* s, MnIndex idx );
const OvString&	mn_to_string( MnState* s, MnIndex idx );

void			mn_call( MnState* s, OvInt nargs );

void			mn_collect_garbage( MnState* s );