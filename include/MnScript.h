#pragma once
#include "OvRefable.h"

typedef OvInt	MnIndex;

class MnState;

OvSPtr<MnState> mn_open_state();
void			mn_close_state( OvWRef<MnState> s );

void	mn_add_field( OvWRef<MnState> s, const OvString& name );

void	mn_push_field( OvWRef<MnState> s, const OvString& name );
void	mn_push_number( OvWRef<MnState> s, OvReal v );
void	mn_push_string( OvWRef<MnState> s, const OvString& v );