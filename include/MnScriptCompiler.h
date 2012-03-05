#pragma once
struct compile_state;
class 	sm_func;
class 	sm_block;
class 	sm_exp;
struct  expdesc;

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
#define cs_const(idx)	(cs_bit1(1,asize) | (cs_bit1(asize,0) & cs_index((idx))))

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

	op_getfield,
	op_setfield,

	op_newclosure,

	op_block_begin,
	op_block_end,
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

enum keyworld
{
	kw_unknown	= 0,
	kw_true,
	kw_false,
	kw_function,
	kw_local,
	kw_for,
	kw_while,
	kw_if,
};

const OvChar* g_kwtable[] = 
{
	"unknown",
	"true",
	"false",
	"function",
	"local",
	"for",
	"while",
	"if",
};

void		cs_tnext( compile_state* cs );
void		cs_tprev( compile_state* cs );
OvBool		cs_ttype( compile_state* cs, OvInt type );
OvBool		cs_toptional( compile_state* cs, OvInt type );
OvBool		cs_texpected( compile_state* cs, OvInt type );
OvReal&		cs_tnum( compile_state* cs );
OvString&	cs_tstr( compile_state* cs );
OvBool		cs_keyworld( compile_state* cs, keyworld kw );
OvString*	cs_new_str( compile_state* cs, OvString& str );
s_token*	cs_new_tok( compile_state* cs, OvChar type );
void		cs_scan( compile_state* cs );
void		cs_compile( MnState* s, const OvString& file );
void		cs_scan_file( compile_state* cs, const OvString& file );

void		stat_local( compile_state* cs );
void		stat_common_block( compile_state* cs );
void		stat_nest_block( compile_state* cs );
void		stat_func_block( compile_state* cs );
void		stat_exp( compile_state* cs, expdesc& desc );
OvBool		stat( compile_state* cs );
void		statements( compile_state* cs );
void		stat_entrance( compile_state* cs );

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
	~compile_state()
	{
		while ( head )
		{
			tok = head;
			head = head->next;
			ut_free(tok);
		}
	}

	MnState*		s;
	OvChar			c;
	OvInputStream*	is;

	set_str			strset;

	s_token*		head;
	s_token*		tail;

	s_token*		tok;

	sm_func*		funcstat;

};

class sm_func
{
public:
	MnMFunction*	func;
	sm_block*		block;

	sm_func() : func(NULL), block(NULL) {};

	void	addcode( const MnInstruction& i );
	OvInt	codesize();

	OvShort	findconst( const MnValue& val );
	OvShort findvar( const OvString& name );

};

class sm_block
{
public:
	typedef OvVector<OvString> vec_str;
	vec_str			vars;
	sm_block*		outer;

	sm_block() : outer(NULL) {};

	OvInt	nvars();
	void	addvar( const OvString& name );
	OvShort	findvar( const OvString& name );

};


struct expdesc;

enum exptype
{
	enone,
	etemp,		//< [reg1:stack index]
	econst,		//< [reg1:const index]
	evariable,	//< [reg1:stack index]
	eclosure,
	efield,		//< [reg1:stack index] [reg2:key index in const or stack]
};

struct expdesc
{
	expdesc(){};
	expdesc( exptype t, OvShort r ) : type(t), reg(r) {};
	exptype type;
	OvShort reg;
};

class sm_exp
{
public:
	compile_state*	cs;
	OvInt			ntemp;
	OvInt			nvars;
	OvVector<expdesc> targets;

	const expdesc&	get( MnIndex idx );
	OvShort			alloc_temp();

	void			push( exptype t, OvShort r );
	void			push();
	void			pop( OvInt n );
	void			pop();

	void			statexp();
	void			exp_order3();
	void			exp_order2();
	void			exp_order1();
	void			field2reg();
	void			term();
	void			primary();
};

OvShort		sm_exp::alloc_temp() { return nvars + (++ntemp); };
void		sm_exp::push( exptype t, OvShort r ) { targets.push_back( expdesc( t, r ) ); }
void		sm_exp::push() { push( etemp, alloc_temp() ); }
void		sm_exp::pop() { if ( get(-1).type == etemp ) --ntemp; targets.pop_back(); }
void		sm_exp::pop( OvInt n ) { if ( n > 0) while( n-- ) pop(); };

const expdesc&	sm_exp::get( MnIndex idx )
{
	if ( idx < 0 && targets.size() >= abs(idx) )
	{
		return targets.at( targets.size() + idx );
	}
	else if ( idx > 0 && targets.size() >= idx )
	{
		return targets.at( -1 + idx );
	}
	static expdesc noneexp(enone,0);
	return noneexp;
}

//////////////////////////////////////////////////////////////////////////

void	sm_func::addcode( const MnInstruction& i ) { func->codes.push_back( i );	}
OvInt	sm_func::codesize() { return func->codes.size(); } ;

OvShort	sm_func::findconst( const MnValue& val )
{
	if ( MnIsNumber(val) || MnIsString(val) )
	{
		for ( MnIndex i = 0; i < func->consts.size(); ++i )
		{
			const MnValue& v = func->consts[i];
			if ( MnIsNumber(val) && MnIsNumber(v) && (MnToNumber(v) == MnToNumber(val)) )
			{
				return cs_const( i );
			}
			else if ( MnIsString(val) && MnIsString(v) && (MnToString(v)->get_str() == MnToString(val)->get_str()) )
			{
				return cs_const( i );
			}
		}
	}
	func->consts.push_back( val );
	return cs_const( func->consts.size() - 1 );
}

OvShort sm_func::findvar( const OvString& name )
{
	return block->findvar( name );
}

//////////////////////////////////////////////////////////////////////////

OvInt	sm_block::nvars()
{
	return vars.size() + (outer? outer->nvars():0);
};

void	sm_block::addvar( const OvString& name )
{
	for each ( const OvString& n in vars ) if ( n == name ) return ;
	vars.push_back(name);
};

OvShort	sm_block::findvar( const OvString& name )
{
	for ( MnIndex i = 0 ; i < vars.size() ; ++i )
	{
		if ( vars[i] == name ) return i ;
	}
	return -1;
}

//////////////////////////////////////////////////////////////////////////

void	sm_exp::statexp()
{
	exp_order2();
}

void	sm_exp::exp_order3()
{
	term();
	while ( cs_toptional(cs,'=') )
	{
		exp_order2();
	}
}

void	sm_exp::exp_order2()
{
	exp_order1();
	while ( cs_ttype( cs, '+' ) || cs_ttype( cs, '-' ) )
	{
		opcode op = cs_toptional( cs, '+' )? op_add : cs_toptional( cs, '-' )? op_sub : op_none;
		exp_order1();
		expdesc exp1 = get(-2);
		expdesc exp2 = get(-1);
		pop(2);
		push();
		cs->funcstat->addcode( cs_code( op, get(-1).reg, exp1.reg, exp2.reg ) );
	}
}

void	sm_exp::exp_order1()
{
	field2reg();
	while ( cs_ttype( cs, '*' ) || cs_ttype( cs, '/' ) )
	{
		opcode op = cs_toptional( cs, '*' )? op_mul : cs_toptional( cs, '/' )? op_div : op_none;
		field2reg();
		expdesc exp1 = get(-2);
		expdesc exp2 = get(-1);
		pop(2);
		push();
		cs->funcstat->addcode( cs_code( op, get(-1).reg, exp1.reg, exp2.reg ) );
	}
}

void	sm_exp::field2reg()
{
	term();
	switch ( get(-1).type )
	{
	case efield :
		expdesc con = get(-2);
		expdesc key = get(-1);
		pop(2);
		push();
		cs->funcstat->addcode( cs_code( op_getfield, get(-1).reg, con.reg, key.reg ) );
	}
}

void	sm_exp::term()
{
	primary();
}

void	sm_exp::primary()
{
	if ( cs_ttype( cs, tt_number ) )
	{
		push( econst, cs->funcstat->findconst(MnValue(cs_tnum(cs)) ) );
		cs_tnext(cs);
	}
	else if ( cs_ttype( cs, tt_string ) )
	{
		push( econst, cs->funcstat->findconst( MnValue(MOT_STRING,ut_newstring(cs->s,cs_tstr(cs))) ) );
		cs_tnext(cs);
	}
	else if ( cs_ttype( cs, tt_identifier ) )
	{
		OvShort idx = cs->funcstat->findvar( cs_tstr(cs) );
		push( (( idx < 0 )? enone : evariable), idx );
		cs_tnext(cs);
	}
	else if ( cs_keyworld(cs,kw_function) )
	{
		cs_tnext(cs);
		sm_func* last = cs->funcstat;
		sm_func fstat;
		fstat.func = ut_newfunction(cs->s);

		cs->funcstat = &fstat;
		stat_func_block( cs );
		cs->funcstat = last;

		push();

		OvShort func = cs->funcstat->findconst( MnValue( MOT_FUNCPROTO, fstat.func ) );
		cs->funcstat->addcode( cs_code(op_newclosure,get(-1).reg,func,0) );
	}
	else if ( cs_toptional( cs, '(' ) )
	{
		statexp();
		cs_toptional( cs, ')' );
	}
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

OvReal&		cs_tnum( compile_state* cs ) { static OvReal temp=0; return (( cs->tok )? cs->tok->num:temp);};
OvString&	cs_tstr( compile_state* cs ) { static OvString temp; return (( cs->tok )? *(cs->tok->str):temp);};

void		cs_tnext( compile_state* cs ) 	{ cs->tok = (cs->tok)? cs->tok->next : NULL; };
void		cs_tprev( compile_state* cs ) 	{ cs->tok = (cs->tok)? cs->tok->next : NULL; };

OvBool		cs_ttype( compile_state* cs, OvInt type ) { return ( cs->tok )? (cs->tok->type == type) : false; };
OvBool		cs_toptional( compile_state* cs, OvInt type )
{
	if ( cs_ttype(cs,type) ) { cs_tnext(cs); return true; }
	return false;
}

OvBool		cs_texpected( compile_state* cs, OvInt type )
{
	if ( cs_ttype(cs,type) ) { cs_tnext(cs); return true; }
	return false;
}

OvBool		cs_keyworld( compile_state* cs, keyworld kw )
{
	if ( cs_ttype(cs,tt_identifier) ) return ( cs_tstr(cs) == g_kwtable[kw] );
	return false;
}

void		cs_scan_file( compile_state* cs, const OvString& file )
{
	OvFileInputStream fis( file );
	cs->is = &fis;
	cs->is->Read(cs->c);
	cs_scan(cs);
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

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void scan_test( MnState* s, const OvString& file )
{
	cs_compile( s, file);
}


void		cs_compile( MnState* s, const OvString& file )
{
	compile_state cs(s);
	cs_scan_file( &cs, file );

	stat_entrance(&cs);

}

void stat_entrance( compile_state* cs )
{
	MnValue func(MOT_FUNCPROTO,ut_newfunction(cs->s));
	sm_func fmain;
	sm_block block;
	fmain.func  = MnToFunction(func);
	fmain.block = &block;
	cs->funcstat = &fmain;
	statements(cs);
}

void statements( compile_state* cs )
{
	while ( stat(cs) );
}

OvBool stat( compile_state* cs )
{
	if ( cs_ttype(cs,'{') )				 	{ stat_nest_block( cs ); return true; }
	else if ( cs_keyworld(cs,kw_local) ) 	{ stat_local(cs); return true; }
	else if ( cs_toptional(cs,';') )		{ return true; }
	else if ( cs_toptional(cs,tt_eos) ) 		{ return false; }
	else
	{
		expdesc exp;
		stat_exp( cs, exp );
		return (exp.type != enone);
	}
	return false;
}

void		stat_exp( compile_state* cs, expdesc& desc )
{
	sm_exp exp;
	exp.cs = cs;
	exp.nvars = cs->funcstat->block->nvars();
	exp.ntemp = 0;
	exp.statexp();
	desc = exp.get(-1);
};

void stat_common_block( compile_state* cs ) 
{
	sm_block*	last = cs->funcstat->block;
	sm_block	block;
	cs->funcstat->block = &block;

	statements(cs);

	cs->funcstat->block = last;
}

void		stat_nest_block( compile_state* cs )
{
	if ( cs_texpected(cs,'{') )
	{
		cs->funcstat->addcode( cs_code(op_block_begin,0,0,0) );
		stat_common_block(cs);
		cs_texpected(cs,'}');
		cs->funcstat->addcode( cs_code(op_block_end,0,0,0) );
	}
};

void		stat_func_block( compile_state* cs )
{
	if ( cs_texpected(cs,'{') )
	{
		stat_common_block(cs);
		cs_texpected(cs,'}');
	}
};

void stat_local( compile_state* cs ) 
{
	if ( cs_keyworld(cs,kw_local) )
	{
		cs_tnext(cs);
		if ( cs_ttype(cs,tt_identifier) )
		{
			cs->funcstat->block->addvar( cs_tstr(cs) );
			expdesc exp;
			stat_exp( cs, exp );
		}
	}
}