#pragma once

#define cs_bit1( n, p ) ((~((~((MnInstruction)0))<<(n)))<<(p))
#define cs_bit0( n, p ) (~cs_bit1( n, p ))

#define cs_getbit(i,n,p) ((cs_bit1( (n), (p) ) & (i) )>>(p))
#define cs_setbit(i,n,p,b) (i = (cs_bit1( (n), (p) ) & ((b)<<(p))) | (cs_bit0( (n), (p) ) & (i)))

#define opsize	(6)
#define asize 	(8)
#define bsize 	(9)
#define csize 	(9)

#define oppos	(0)
#define apos	(oppos + opsize)
#define bpos  	(apos  + asize)
#define cpos  	(bpos  + bsize)

#define cs_getop(i)		(cs_getbit( i, opsize, oppos ))
#define cs_setop(i,op)	(cs_setbit( i, opsize, oppos, op ))

#define cs_geta(i)		(cs_getbit( i, asize, apos ))
#define cs_seta(i,a)	(cs_setbit( i, asize, apos, a ))

#define cs_getb(i)		(cs_getbit( i, bsize, bpos ))
#define cs_setb(i,b)	(cs_setbit( i, bsize, bpos, b ))

#define cs_getc(i)		(cs_getbit( i, csize, cpos ))
#define cs_setc(i,c)	(cs_setbit( i, csize, cpos, c ))

#define cs_isconst(v)	(!!(cs_bit1(1,apos)&(v)))
#define cs_index(v)		(cs_bit1(asize,0)&(v))
#define cs_const(idx)	(cs_bit1(1,apos) | (cs_bit1(asize,0) & cs_index(idx)))

#define cs_code(op,a,b,c) ( (cs_bit1(opsize,oppos)&((op)<<oppos)) | (cs_bit1(asize,apos)&((a)<<apos)) | (cs_bit1(bsize,bpos)&((b)<<bpos)) | (cs_bit1(csize,cpos)&((c)<<cpos)) )

/*
op : 6
a : 8
b:  9
c : 9
*/

enum opcode
{
	op_none = 0,
	op_add,
	op_sub,
	op_mul,
	op_div,

	op_gettable,
	op_settable,
};

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

	OvInt		type;
	union
	{
		OvReal	num;
		OvString* str;
	};
};

struct compile_state
{
	typedef OvSet<OvString> set_str;

	compile_state( MnState* state )
		: s(state)
		, is(NULL)
		, head(NULL)
		, tail(NULL)
		, tok(NULL)
	{
	}

	MnState*		s;
	OvChar			c;
	OvInputStream*	is;

	set_str			strset;

	s_token*		head;
	s_token*		tail;

	s_token*		tok;

	MnMFunction*	func;

};

void		cs_tnext( compile_state* cs ) 	{ cs->tok = (cs->tok)? cs->tok->next : NULL; };
void		cs_tprev( compile_state* cs ) 	{ cs->tok = (cs->tok)? cs->tok->next : NULL; };

OvBool		cs_ttype( compile_state* cs, OvInt type ) { return ( cs->tok )? (cs->tok->type == type) : false; };
OvBool		cs_tstep( compile_state* cs, OvInt type )
{
	if ( cs_ttype(cs,type) ) { cs_tnext(cs); return true; }
	return false;
}

OvReal&		cs_tnum( compile_state* cs ) { static OvReal temp=0; return ( cs->tok )? cs->tok->num:temp;};
OvString&	cs_tstr( compile_state* cs ) { static OvString temp=0; return ( cs->tok )? *cs->tok->str:temp;};

OvString*	cs_new_str( compile_state* cs, OvString& str );
s_token*	cs_new_tok( compile_state* cs, OvChar type );
void		cs_scan( compile_state* cs );
void		cs_compile( MnState* s, const OvString& file );

void		cs_scan_file( compile_state* cs, const OvString& file )
{
	OvFileInputStream fis( file );
	cs->is = &fis;
	cs->is->Read(cs->c);
	cs_scan(cs);
}

MnInstruction&		cs_addcode( compile_state* cs, MnInstruction i )
{
	cs->func->codes.push_back( i );
	return i;
}

struct stat_func
{
	compile_state* cs;
	MnMFunction* last;
	stat_func( compile_state* cstate );
	~stat_func();

};

struct stat_block
{
	typedef OvVector<OvString> vec_str;
	compile_state*	cs;
	vec_str			vars;
	stat_block*		outer;

	stat_block( compile_state* cstate, stat_block* o );

};

struct expdesc;
void	cs_newconst( compile_state* cs, const MnValue& val, expdesc &exp );
void	cs_findvar( stat_block* block, const OvString& name, expdesc& exp );
OvShort cs_nvars( stat_block* block )
{
	return block? (block->vars.size() + ((block->outer)? cs_nvars(block->outer):0)):0;
}

enum exptype
{
	enone,
	etemp,		//< [reg1:stack index]
	econst,		//< [reg1:const index]
	evariable,	//< [reg1:stack index]
	efield,		//< [reg1:stack index] [reg2:key index in const or stack]
};
struct expdesc
{
	exptype type;
	OvShort reg1;
	OvShort reg2;
};

struct stat_exp
{
	compile_state*	cs;
	stat_block*		block;
	OvInt			ntemp;
	OvInt			nvars;
	expdesc			exp1;
	expdesc			exp2;

	stat_exp( compile_state* s, stat_block* b ) : cs(s), block(b), ntemp(0), nvars(cs_nvars(b)) 
	{
		statexp( exp1 );
	}

	OvShort		alloc_temp() { return nvars + (++ntemp); };
	void		release_temp() { if ( exp1.type == etemp ) ntemp>0?--ntemp:0;if ( exp2.type == etemp ) ntemp>0?--ntemp:0; };

	void	statexp( expdesc& exp )
	{
		stat_exp2( exp );
	}
	void	stat_exp2( expdesc& exp )
	{
		stat_exp1( exp );
		while ( cs_ttype( cs, '+' ) || cs_ttype( cs, '-' ) )
		{
			opcode op = cs_tstep( cs, '+' )? op_add : cs_tstep( cs, '-' )? op_sub : op_none;
			stat_exp1( exp2 );
			release_temp();
			exp.type = etemp;
			exp.reg1 = cs_getb(cs_addcode( cs, cs_code( op, alloc_temp(), exp.reg1, exp2.reg1 ) ));
		}
	}
	void	stat_exp1( expdesc& exp )
	{
		term2reg( exp );
		while ( cs_ttype( cs, '*' ) || cs_ttype( cs, '/' ) )
		{
			opcode op = cs_tstep( cs, '*' )? op_mul : cs_tstep( cs, '/' )? op_div : op_none;
			term2reg( exp2 );
			release_temp();
			exp.type = etemp;
			exp.reg1 = cs_getb(cs_addcode( cs, cs_code( op, alloc_temp(), exp.reg1, exp2.reg1 ) ));
		}
	}
	OvShort	term2reg( expdesc& exp )
	{
		term( exp );
		switch ( exp.type )
		{
		case etemp :
		case econst :
		case evariable :
			return exp.reg1;
		case efield :
			exp.type = etemp;
			exp.reg1 = cs_getb( cs_addcode( cs, cs_code( op_gettable, alloc_temp(), exp.reg1, exp.reg2 ) ) );
			return exp.reg1;
		}
	}
	void	term( expdesc& exp )
	{
		primary(exp);
	}
	void	primary( expdesc& exp )
	{
		if ( cs_ttype( cs, tt_number ) )
		{
			cs_newconst( cs, MnValue(cs_tnum(cs)), exp);
			;
			exp.reg2 = cs_index(exp.reg1);
			cs_tnext(cs);
		}
		else if ( cs_ttype( cs, tt_string ) )
		{
			cs_newconst( cs, MnValue(MOT_STRING,ut_newstring(cs->s,cs_tstr(cs))), exp);
			cs_tnext(cs);
		}
		else if ( cs_ttype( cs, tt_identifier ) )
		{
			cs_findvar( block, cs_tstr(cs), exp );
			cs_tnext(cs);
		}
		else if ( cs_tstep( cs, '(' ) )
		{
			statexp( exp );
			cs_tstep( cs, ')' );
		}
	}

};

void cs_newconst( compile_state* cs, const MnValue& val, expdesc &exp ) 
{
	exp.type = econst;
	if ( MnIsNumber(val) || MnIsString(val) )
	{
		for ( MnIndex i = 0; i < cs->func->consts.size(); ++i )
		{
			const MnValue& v = cs->func->consts[i];
			if ( MnIsNumber(val) && MnIsNumber(v) && (MnToNumber(v) == MnToNumber(val)) )
			{
				exp.reg1 = cs_const(i + 1);
				return ;
			}
			else if ( MnIsString(val) && MnIsString(v) && (MnToString(v)->get_str() == MnToString(val)->get_str()) )
			{
				exp.reg1 = cs_const(i + 1);
				return ;
			}
		}
	}
	cs->func->consts.push_back( val );
	exp.reg1 = cs_const( cs->func->consts.size() );
}

void cs_findvar( stat_block* block, const OvString& name, expdesc& exp )
{
	if ( block )
	{
		for ( MnIndex i = 0 ; i < block->vars.size() ; ++i )
		{
			if ( block->vars[i] == name )
			{
				exp.type = evariable;
				exp.reg1 = (i+1) + cs_nvars(block->outer);
			}
		}

		cs_findvar( block->outer, name, exp );
	}
	exp.type = enone;
}

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

s_token* cs_new_tok( compile_state* cs, OvInt type ) 
{
	s_token* tok = new(ut_alloc( sizeof(s_token) )) s_token;
	tok->type = type;

	if ( !cs->head )
	{
		cs->head = tok;
		cs->tok = tok;
	}
	if ( cs->tail ) cs->tail->next = tok;

	tok->prev = cs->tail;
	cs->tail  = tok;

	return tok;
}

void cs_scan( compile_state* cs )
{
	OvChar& c = cs->c;
#define cp_read() (cs->is->Read(c))

	while ( true )
	{
		if ( c == EOF  )
		{
			cs_new_tok( cs, tt_eos );
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

			cs_new_tok( cs, tt_number )->num = num;
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

			cs_new_tok( cs, tt_string )->str = cs_new_str(cs, str);
		}
		else if ( isalpha(c) || c == '_' )
		{
			OvString str;
			do
			{
				str.push_back(c);
			} while ( cp_read() && (isalnum(c) || c == '_') );

			cs_new_tok( cs, tt_identifier )->str = cs_new_str(cs, str);
		}
		else if ( isspace( c ) )
		{
			while ( isspace( c ) ) cp_read();
		}
		else
		{
			OvChar ret = c;
			cp_read();
			cs_new_tok( cs, (OvInt)ret );
		}
	}
#undef cp_read
}


stat_block::stat_block( compile_state* cstate, stat_block* o ) : cs(cstate), outer(o)
{
	while ( true )
	{
		if ( cs_tstep(cs,'{') )
		{
			stat_block( cs, this );
			cs_tstep(cs,'}');
		}
		else if ( cs_ttype(cs,'}') )
		{
			break;
		}
		else
		{
			stat_exp( cs, this );
		}
	}
};

stat_func::stat_func( compile_state* cstate ) : cs(cstate)
{
	last = cs->func;
	cs->func = ut_newfunction(cs->s);
}
stat_func::~stat_func()
{
	cs->func = last;
}

void		cs_compile( MnState* s, const OvString& file )
{
	compile_state cs(s);
	cs_scan_file( &cs, file );
	cs.func = ut_newfunction(s);
	stat_block(&cs,NULL);
}
void scan_test( MnState* s, const OvString& file )
{
	cs_compile( s, file);
}
