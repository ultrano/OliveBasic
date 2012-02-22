#pragma once
#include "MnInternal.h"

enum toktype
{
	tt_string		= 256,
	tt_number		= 257,
	tt_identifier	= 258,
	tt_eos			= 259,
};
struct s_token
{
	s_token() : next(NULL), prev(NULL), type(0) {};
	s_token*	next;
	s_token*	prev;

	OvChar		type;
	union
	{
		OvReal	num;
		OvString* str;
	};
};

struct compile_state
{
	typedef OvSet<OvString> set_str;

	MnState*		s;
	OvChar			c;
	OvInputStream*	is;

	set_str			strset;

	s_token*		head;
	s_token*		itor;

	void		prev() { itor = (itor)? itor->prev : NULL; };
	void		next() { itor = (itor)? itor->next : NULL; };

};

OvString* cs_new_str( compile_state* cs, OvString& str ) 
{
	compile_state::set_str::iterator itor = cs->strset.find( str );
	if ( itor == cs->strset.end() )
	{
		cs->strset.insert( str );
		itor = cs->strset.find( str );
	}
	return &(*itor);
}

void cs_push_tok( compile_state* cs, s_token* tok ) 
{
	if ( cs->head ) cs->head->prev = tok;
	tok->next	= cs->head;
	cs->head	= tok;
}

void cs_tok_str( compile_state* cs, OvString& str )
{
	s_token* tok = new(ut_alloc( sizeof(s_token) )) s_token;
	tok->type	= tt_string;
	tok->str	= cs_new_str(cs, str);

	cs_push_tok(cs, tok);
}

void cs_tok_id( compile_state* cs, OvString& str )
{
	s_token* tok = new(ut_alloc( sizeof(s_token) )) s_token;
	tok->type	= tt_identifier;
	tok->str	= cs_new_str(cs, str);

	cs_push_tok(cs, tok);
}

void cs_tok_num( compile_state* cs, OvReal& num )
{
	s_token* tok = new(ut_alloc( sizeof(s_token) )) s_token;
	tok->type	= tt_number;
	tok->num	= num;

	cs_push_tok(cs, tok);
}

void cs_tok( compile_state* cs, OvChar c )
{
	s_token* tok = new(ut_alloc( sizeof(s_token) )) s_token;
	tok->type	= c;

	cs_push_tok(cs, tok);
}

void cs_scan( compile_state* cs )
{
	OvChar& c = cs->c;
#define cp_read() (cs->is->Read(c))

	while ( true )
	{
		if ( c == EOF  )
		{
			cs_tok( cs, tt_eos );
			return ;
		}
		else if ( isdigit(c) )
		{
			OvReal num = 0;
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

			cs_tok_num( cs, num );
		}
		else if ( c == '"' )
		{
			OvString str;
			cp_read();
			do
			{
				str.push_back(c);
			} while ( cp_read() && c != '"' );
			cp_read();

			cs_tok_str( cs, str );
		}
		else if ( isalpha(c) || c == '_' )
		{
			OvString str;
			do
			{
				str.push_back(c);
			} while ( cp_read() && (isalnum(c) || c == '_') );

			cs_tok_id( cs, str );
		}
		else
		{
			OvChar ret = c;
			cp_read();

			cs_tok( cs, ret );
		}
	}
#undef cp_read
}
