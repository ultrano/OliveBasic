#pragma once
#include "OvRefable.h"

typedef OvInt	MnIndex;
class MnState;

MnState*		mn_open_state();
void			mn_close_state( MnState* s );

void			mn_set_global( MnState* s, const OvString& name );
void			mn_get_global( MnState* s, const OvString& name );

void			mn_new_table( MnState* s );
void			mn_set_table( MnState* s, MnIndex idx );
void			mn_get_table( MnState* s, MnIndex idx );

void			mn_set_top( MnState* s, MnIndex idx );
MnIndex			mn_get_top( MnState* s );

#define			mn_pop(s,n) mn_set_top( (s), -(n)-1 )

void			mn_push_boolean( MnState* s, OvBool v );
void			mn_push_number( MnState* s, OvReal v );
void			mn_push_string( MnState* s, const OvString& v );

OvBool			mn_is_boolean( MnState* s, MnIndex idx );
OvBool			mn_is_number( MnState* s, MnIndex idx );
OvBool			mn_is_string( MnState* s, MnIndex idx );

OvBool			mn_to_boolean( MnState* s, MnIndex idx );
OvReal			mn_to_number( MnState* s, MnIndex idx );
const OvString&	mn_to_string( MnState* s, MnIndex idx );