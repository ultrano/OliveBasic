#pragma once
#include "OvRefable.h"

typedef OvInt	MnIndex;
class MnState;

OvSPtr<MnState> mn_open_state();
void			mn_close_state( OvWRef<MnState> s );

void			mn_set_field( OvWRef<MnState> s, MnIndex idx );
void			mn_get_field( OvWRef<MnState> s, MnIndex idx );

void			mn_set_top( OvWRef<MnState> s, MnIndex idx );
MnIndex			mn_get_top( OvWRef<MnState> s );

void			mn_push_boolean( OvWRef<MnState> s, OvBool v );
void			mn_push_number( OvWRef<MnState> s, OvReal v );
void			mn_push_string( OvWRef<MnState> s, const OvString& v );

OvBool			mn_is_boolean( OvWRef<MnState> s, MnIndex idx );
OvBool			mn_is_number( OvWRef<MnState> s, MnIndex idx );
OvBool			mn_is_string( OvWRef<MnState> s, MnIndex idx );

OvBool			mn_to_boolean( OvWRef<MnState> s, MnIndex idx );
OvReal			mn_to_number( OvWRef<MnState> s, MnIndex idx );
const OvString&	mn_to_string( OvWRef<MnState> s, MnIndex idx );