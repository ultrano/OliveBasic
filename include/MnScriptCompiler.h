#pragma once

enum toktype
{
	tt_string		= 256,
	tt_number		= 257,
	tt_identifier	= 258,
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

struct compiler_state
{
	void		prev() { itor = (itor)? itor->prev : NULL; };
	void		next() { itor = (itor)? itor->next : NULL; };
	s_token* head;
	s_token* itor;
};
/*

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
*/