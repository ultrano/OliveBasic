#pragma once
struct compile_state;
class 	sm_func;
class 	sm_block;
class 	sm_exp;

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

#define cs_isconst(v)	(!!(cs_bit1(1,asize)&(v)))
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

	op_move,

	op_getfield,
	op_setfield,

	op_getglobal,
	op_setglobal,

	op_newclosure,

	op_block_begin,
	op_block_end,

	op_call,
	op_return,
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
	kw_global,
	kw_for,
	kw_while,
	kw_if,
	kw_return,
	kw_do,
	kw_else,
	kw_break,
	kw_push,
	kw_pull,
	kw_or,
	kw_and,
	kw_eq,
	kw_neq,
	kw_leq,
	kw_geq,
};

const OvChar* g_kwtable[] = 
{
	"unknown",
	"true",
	"false",
	"function",
	"local",
	"global",
	"for",
	"while",
	"if",
	"return",
	"do",
	"else",
	"break",
	"<<",
	">>",
	"||",
	"&&",
	"==",
	"!=",
	"<=",
	">=",
};

OvShort		cs_findconst( compile_state* cs, const MnValue& val );
OvShort		cs_findconst( compile_state* cs, const MnNumber& val );
OvShort		cs_findconst( compile_state* cs, const OvString& val );

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
void		cs_scan_file( compile_state* cs, const OvString& file );
MnValue		load_entrance( MnState* s, const OvString& file );

void		stat_local( compile_state* cs );
void		stat_global( compile_state* cs );
void		stat_block( compile_state* cs );
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
	OvUInt			maxstack;

	sm_func() : func(NULL), block(NULL), maxstack(0) {};

	void	addcode( const MnInstruction& i );
	OvInt	codesize();

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

enum exptype
{
	enone,
	etemp,		
	econst,		
	evariable,	
	eglobal,
	eclosure,
	efield,		
};

struct expdesc
{
	expdesc(){};
	expdesc( exptype t, OvShort r ) : type(t), idx(r) {};
	exptype type;
	OvShort idx;
};

class sm_exp
{
public:
	compile_state*	cs;
	OvInt			ntemp;
	OvInt			nfixed;
	OvVector<expdesc> targets;

	sm_exp( compile_state* cs );

	OvShort			alloc_temp();

	expdesc&		get( MnIndex idx );
	OvShort			top();
	void			push( exptype type, OvShort idx );
	OvShort			push( exptype type );
	void			pop();
	OvShort			regist();

	void			statexp();
	void			exp_order();
	void			exp_order3();
	void			exp_order2();
	void			exp_order1();

	void			term();
	void			postexp();
	void			primary();
};

//////////////////////////////////////////////////////////////////////////

void	sm_func::addcode( const MnInstruction& i ) { func->codes.push_back( i );	}
OvInt	sm_func::codesize() { return func->codes.size(); } ;

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

sm_exp::sm_exp( compile_state* _cs )
: cs( _cs )
, ntemp( cs->funcstat->block->nvars() )
{
}

OvShort		sm_exp::alloc_temp() 
{
	OvShort idx = ntemp++;
	cs->funcstat->maxstack = max( cs->funcstat->maxstack, ntemp);
	return idx;
};

expdesc&		sm_exp::get( MnIndex idx )
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

OvShort sm_exp::top()
{
	switch ( get(-1).type )
	{
	case efield :
		{
			OvShort con = get(-2).idx;
			OvShort key = get(-1).idx;
			pop();pop();
			cs->funcstat->addcode( cs_code( op_getfield, push(etemp), con, key ) );
		}
		break;
	case eglobal :
		{
			OvShort idx = get(-1).idx;
			pop();
			cs->funcstat->addcode( cs_code( op_getglobal, push(etemp), idx, 0 ) );
		}
		break;
	}
	return get(-1).idx;
}

void		sm_exp::push( exptype type, OvShort idx )
{
	targets.push_back( expdesc(type,idx) );
}

OvShort		sm_exp::push( exptype type )
{
	push( type, alloc_temp() ); return targets.back().idx;
}

void		sm_exp::pop()
{
	expdesc exp = get(-1);
	if ( !cs_isconst(exp.idx) && cs_index(exp.idx) == (ntemp-1) ) --ntemp;
	targets.pop_back();
}

void	sm_exp::statexp()
{
	exp_order();
}

void	sm_exp::exp_order()
{
	exp_order3();
}
void	sm_exp::exp_order3()
{
	exp_order2();
	while ( cs_toptional( cs, '=' ) )
	{
		exp_order();
		OvShort val = top();pop();
		switch ( get(-1).type )
		{
		case efield :
			cs->funcstat->addcode( cs_code( op_setfield, get(-2).idx, get(-1).idx, val ) );
			break;
		case eglobal :
			cs->funcstat->addcode( cs_code( op_setglobal, 0, get(-1).idx, val ) );
			break;
		case etemp :
		case evariable :
			cs->funcstat->addcode( cs_code( op_move, get(-1).idx, val, 0 ) );
			break;
		}
	}
}

void	sm_exp::exp_order2()
{
	exp_order1();
	while ( cs_ttype( cs, '+' ) || cs_ttype( cs, '-' ) )
	{
		opcode op = cs_toptional( cs, '+' )? op_add : cs_toptional( cs, '-' )? op_sub : op_none;
		exp_order1();
		OvShort reg2 = top();pop();
		OvShort reg1 = top();pop();
		cs->funcstat->addcode( cs_code( op, push(etemp), reg1, reg2 ) );
	}
}

void	sm_exp::exp_order1()
{
	term();
	while ( cs_ttype( cs, '*' ) || cs_ttype( cs, '/' ) )
	{
		opcode op = cs_toptional( cs, '*' )? op_mul : cs_toptional( cs, '/' )? op_div : op_none;
		term();
		OvShort reg2 = top();pop();
		OvShort reg1 = top();pop();
		cs->funcstat->addcode( cs_code( op, push(etemp), reg1, reg2 ) );
	}
}

void	sm_exp::term()
{
	postexp();
}

void	sm_exp::postexp()
{
	primary();

	while (true)
	{
		if ( cs_toptional(cs,'[') )
		{
			top();
			exp_order();
			top();
			get(-1).type = efield;
			cs_texpected(cs,']');
		}
		else if ( cs_toptional(cs,'(') )
		{
			OvShort func = top();pop();
			cs->funcstat->addcode( cs_code( op_move, push(etemp), func, 0 ) );
			func = top();
			OvShort narg = 0;
			if ( !cs_ttype(cs,')') ) do
			{
				exp_order();
				OvShort arg = top();pop();
				++narg;
				cs->funcstat->addcode( cs_code( op_move, push(etemp), arg, 0 ) );
			}
			while ( cs_toptional(cs,',') );
			cs->funcstat->addcode( cs_code( op_call, func, narg, 1 ) );
			while ( narg-- ) pop();
			cs_texpected(cs,')');
		}
		else break;
	}
}

void	sm_exp::primary()
{
	if ( cs_ttype( cs, tt_number ) )
	{
		push( econst, cs_findconst( cs, cs_tnum(cs) ) );
		cs_tnext(cs);
	}
	else if ( cs_ttype( cs, tt_string ) )
	{
		push( econst, cs_findconst( cs, cs_tstr(cs) ) );
		cs_tnext(cs);
	}
	else if ( cs_keyworld(cs,kw_function) )
	{
		cs_tnext(cs);

		sm_func		fstat;
		sm_block	block;
		fstat.func		= ut_newfunction(cs->s);
		fstat.block		= &block;

		cs_texpected(cs,'(');
		while ( cs_ttype(cs,tt_identifier) )
		{
			block.addvar( cs_tstr(cs) );
			cs_tnext(cs);
		}
		cs_texpected(cs,')');

		sm_func*	last = cs->funcstat;
		cs->funcstat	 = &fstat;
		cs_texpected(cs,'{');
		statements(cs);
		cs_texpected(cs,'}');
		cs->funcstat = last;

		fstat.func->maxstack = fstat.maxstack;
		fstat.addcode( cs_code( op_return, 0, 0, 0 ) );

		OvShort func = cs_findconst( cs, MnValue( MOT_FUNCPROTO, fstat.func ) );
		cs->funcstat->addcode( cs_code( op_newclosure, push(eclosure), func, 0 ) );
	}
	else if ( cs_ttype( cs, tt_identifier ) )
	{
		sm_block* block = cs->funcstat->block;
		MnIndex idx = block->findvar( cs_tstr(cs) );
		if ( idx < 0 )
		{
			push( eglobal, cs_findconst( cs, cs_tstr(cs) ) );
		}
		else
		{
			push( evariable, idx );
		}

		cs_tnext(cs);
	}
	else if ( cs_toptional( cs, '(' ) )
	{
		exp_order();
		cs_toptional( cs, ')' );
	}
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

OvShort cs_findconst( compile_state* cs, const MnValue& val )
{
	MnMFunction* func = cs->funcstat->func;
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

OvShort		cs_findconst( compile_state* cs, const MnNumber& val )
{
	return cs_findconst( cs, MnValue(val) );
}

OvShort		cs_findconst( compile_state* cs, const OvString& val )
{
	return cs_findconst( cs, MnValue( MOT_STRING, ut_newstring(cs->s,val) ));
}

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

MnValue	load_entrance( MnState* s, const OvString& file )
{
	compile_state cs(s);
	cs_scan_file( &cs, file );

	stat_entrance(&cs);

	return MnValue( MOT_FUNCPROTO, cs.funcstat->func );
}

void stat_entrance( compile_state* cs )
{
	sm_func fmain;
	sm_block block;
	fmain.func  = ut_newfunction(cs->s);
	fmain.block = &block;
	cs->funcstat = &fmain;
	statements(cs);
	fmain.addcode( cs_code( op_return, 0, 0, 0 ) );
	fmain.func->maxstack = fmain.maxstack;

}

void statements( compile_state* cs )
{
	while ( stat(cs) );
}

OvBool stat( compile_state* cs )
{
	if ( cs_ttype(cs,'{') )				 	{ stat_block( cs ); return true; }
	else if ( cs_keyworld(cs,kw_local) ) 	{ stat_local(cs); return true; }
	else if ( cs_keyworld(cs,kw_global) ) 	{ stat_global(cs); return true; }
	else if ( cs_toptional(cs,';') )		{ return true; }
	else if ( cs_toptional(cs,tt_eos) ) 		{ return false; }
	else
	{
		sm_exp exp(cs);
		exp.statexp();
		return (exp.get(-1).type != enone);
	}
	return false;
}

void		stat_block( compile_state* cs )
{
	if ( cs_texpected(cs,'{') )
	{
		cs->funcstat->addcode( cs_code(op_block_begin,0,0,0) );
		
		sm_block*	last = cs->funcstat->block;
		sm_block	block;
		cs->funcstat->block = &block;
		statements(cs);
		cs->funcstat->block = last;

		cs_texpected(cs,'}');
		cs->funcstat->addcode( cs_code(op_block_end,0,0,0) );
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
			sm_exp exp(cs);
			exp.statexp();
		}
	}
}

void stat_global( compile_state* cs ) 
{
	if ( cs_keyworld(cs,kw_global) )
	{
		cs_tnext(cs);
		if ( cs_ttype(cs,tt_identifier) )
		{
			cs->funcstat->block->addvar( cs_tstr(cs) );
			sm_exp exp(cs);
			exp.statexp();
		}
	}
}

//////////////////////////////////////////////////////////////////////////

OvInt excuter_ver_0_0_3( MnState* s, MnMFunction* func )
{

#define iOP (cs_getop(i))

#define oA ( cs_geta(i) )
#define oB ( cs_getb(i) )
#define oC ( cs_getc(i) )

#define iA ( cs_index(oA) )
#define iB ( cs_index(oB) )
#define iC ( cs_index(oC) )

#define sA ( *(s->base + 1 + iA) )
#define sB ( *(s->base + 1 + iB) )
#define sC ( *(s->base + 1 + iC) )

#define cB ( func->consts[ iB ] )
#define cC ( func->consts[ iC ] )

#define vA sA
#define vB (cs_isconst( cs_getb(i) )? cB : sB)
#define vC (cs_isconst( cs_getc(i) )? cC : sC)

	mn_settop( s, func->maxstack );
	MnInstruction* pc = func->codes.size()? &func->codes[0] : NULL;
	while ( true )
	{
		MnInstruction& i = *pc++;
		switch ( iOP )
		{
		case op_add :
			vA = MnToNumber(vB) + MnToNumber(vC);
			break;
		case op_sub :
			vA = MnToNumber(vB) - MnToNumber(vC);
			break;
		case op_mul :
			vA = MnToNumber(vB) * MnToNumber(vC);
			break;
		case op_div :
			vA = MnToNumber(vB) / MnToNumber(vC);
			break;

		case op_move :
			vA = vB;
			break;

		case op_setglobal :
			ut_setglobal( s, vB, vC );
			break;
		case op_getglobal :
			vA = ut_getglobal( s, vB );
			break;

		case op_setfield :
			ut_setfield( s, vA, vB, vC );
			break;
		case op_getfield :
			vA = ut_getfield( s, vB, vC );
			break;

		case op_newclosure :
			{
				MnClosure* cls = ut_newMclosure( s );
				cls->u.m->func = vB;
				vA = MnValue( MOT_CLOSURE, cls );
			}
			break;

		case op_call :
			{
				MnCallInfo* ci = ( MnCallInfo* )ut_alloc( sizeof( MnCallInfo ) );
				ci->prev	= s->ci;
				ci->savepc	= s->pc;
				ci->base	= s->base - s->begin;
				ci->top		= s->top - s->begin;
				s->ci    = ci;

				ut_excute( s, (iA+1), oB );

				ci = s->ci;
				s->base = ci->base + s->begin;
				s->top	= ci->top + s->begin;
				s->pc	= ci->savepc;
				s->ci	= ci->prev;
				ut_free(ci);
			}
			break;
		case op_return :
			return oA;
			break;
		}
	}

	return 0;
#undef iOP

#undef oA
#undef oB
#undef oC

#undef sA
#undef sB
#undef sC
	   
#undef cB
#undef cC
	   
#undef vA
#undef vB
#undef vC
}