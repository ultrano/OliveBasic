#pragma once
struct compile_state;
class 	sm_func;
class 	sm_block;
class 	sm_exp;

enum toktype
{
	tt_identifier	= 256,
	tt_string		= 257,
	tt_number		= 258,
	tt_eos			= 259,
};
struct s_token
{
	s_token() : next(NULL), prev(NULL), type(0) {};
	s_token( const OvString* s ) : next(NULL), prev(NULL), type(0) { str = s; };
	s_token*	next;
	s_token*	prev;

	OvInt		type;
	union
	{
		OvReal	num;
		const OvString* str;
	};
};

enum exptype
{
	enone,
	etemp,		
	econst,		
	evariable,	
	eupval,
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

enum keyword
{
	kw_unknown	= 0,
	kw_nil,
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
	"nil",
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

OvShort		fs_findconst( sm_func* fs, const MnValue& val );
void		fs_addcode( sm_func* fs, const MnInstruction& i );

void		cs_tnext( compile_state* cs );
void		cs_tprev( compile_state* cs );
OvBool		cs_ttype( compile_state* cs, OvInt type );
OvBool		cs_toptional( compile_state* cs, OvInt type );
OvBool		cs_texpected( compile_state* cs, OvInt type );
OvReal&		cs_tnum( compile_state* cs );
const OvString&	cs_tstr( compile_state* cs );
OvBool		cs_keyword( compile_state* cs, keyword kw );
OvBool		cs_kwoptional( compile_state* cs, keyword kw );
const OvString*	cs_new_str( compile_state* cs, OvString& str );
s_token*	cs_new_tok( compile_state* cs, OvChar type );
void		cs_scan( compile_state* cs );
void		cs_scan_file( compile_state* cs, const OvString& file );
MnValue		load_entrance( MnState* s, const OvString& file );

void		cs_begin_func( compile_state* cs );
void		cs_end_func( compile_state* cs );
void		cs_begin_block( compile_state* cs );
void		cs_end_block( compile_state* cs );

OvBool		stat( compile_state* cs );
void		stat_local( compile_state* cs );
void		stat_global( compile_state* cs );
void		stat_block( compile_state* cs );
void		stat_return( compile_state* cs );
void		stat_if( compile_state* cs );
void		stat_while( compile_state* cs );
void		stat_break( compile_state* cs );
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
		, fs(NULL)
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
	sm_func*		fs;
	OvVector<OvInt> breaks;
};

struct upvaldesc
{
	OvString name;
	expdesc	 exp;
};
class sm_func
{
public:

	MnMFunction*	f;
	sm_block*		bs;
	OvUInt			maxstack;
	sm_func*		last;
	OvVector<upvaldesc> upvals;
};

class sm_block
{
	typedef OvVector<OvString> vec_str;
public:
	sm_func*		fs;
	sm_block*		outer;
	vec_str			locals;

	void	addlocal( const OvString& name );
	OvShort	findlocal( const OvString& name );
	OvInt	nlocals();

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
	OvShort			push();
	void			pop();
	OvShort			regist();

	void			statexp();
	void			exp_order();
	void			exp_order10();
	void			exp_order9();
	void			exp_order8();
	void			exp_order7();
	void			exp_order6();
	void			exp_order5();
	void			exp_order4();
	void			exp_order3();
	void			exp_order2();
	void			exp_order1();

	void			term();
	void			postexp();
	void			primary();
	void			varexp( sm_func* fs, expdesc& exp );
	void			funcexp();

};

//////////////////////////////////////////////////////////////////////////

void	sm_block::addlocal( const OvString& name )
{
	for each ( const OvString& n in locals ) if ( n == name ) return ;
	locals.push_back(name);
	++(fs->maxstack);
}

OvInt	sm_block::nlocals()
{
	return locals.size() + (outer? outer->nlocals() : 0);
};

OvShort	sm_block::findlocal( const OvString& name )
{
	for ( MnIndex i = 0 ; i < locals.size() ; ++i )
	{
		if ( locals[i] == name ) return i + (outer? outer->nlocals() : 0) ;
	}
	return outer? outer->findlocal(name):-1;
}

//////////////////////////////////////////////////////////////////////////

sm_exp::sm_exp( compile_state* _cs )
: cs( _cs )
, ntemp( cs->fs->bs->nlocals() )
{
}

OvShort		sm_exp::alloc_temp() 
{
	OvShort idx = ntemp++;
	cs->fs->maxstack = max( cs->fs->maxstack, ntemp);
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
	static expdesc noneexp(enone,-1);
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
			fs_addcode( cs->fs, cs_code( op_getfield, push(), con, key ) );
		}
		break;
	case eupval :
		{
			OvShort idx = get(-1).idx;
			pop();
			fs_addcode( cs->fs, cs_code( op_getupval, push(), idx, 0 ) );
		}
		break;
	case eglobal :
		{
			OvShort idx = get(-1).idx;
			pop();
			fs_addcode( cs->fs, cs_code( op_getglobal, push(), idx, 0 ) );
		}
		break;
	}
	return get(-1).idx;
}

void		sm_exp::push( exptype type, OvShort idx )
{
	targets.push_back( expdesc(type,idx) );
}

OvShort		sm_exp::push()
{
	push( etemp, alloc_temp() ); return targets.back().idx;
}

void		sm_exp::pop()
{
	expdesc exp = get(-1);
	if ( exp.type == etemp && cs_index(exp.idx) == (ntemp-1) ) --ntemp;
	targets.pop_back();
}

void	sm_exp::statexp()
{
	exp_order();
}

void	sm_exp::exp_order()
{
	exp_order10();
}
void	sm_exp::exp_order10()
{
	exp_order9();
	while ( cs_toptional( cs, '=' ) )
	{
		exp_order();
		OvShort val = top();pop();
		switch ( get(-1).type )
		{
		case efield :
			fs_addcode( cs->fs, cs_code( op_setfield, get(-2).idx, get(-1).idx, val ) );
			break;
		case eglobal :
			fs_addcode( cs->fs, cs_code( op_setglobal, 0, get(-1).idx, val ) );
			break;
		case eupval :
			fs_addcode( cs->fs, cs_code( op_setupval, 0, get(-1).idx, val ) );
			break;
		case etemp :
		case evariable :
			fs_addcode( cs->fs, cs_code( op_move, get(-1).idx, val, 0 ) );
			break;
		}
	}
}

void	sm_exp::exp_order9()
{
	exp_order8();
	while ( true )
	{
		keyword kw = cs_kwoptional(cs,kw_eq)?  kw_eq  : cs_kwoptional(cs,kw_neq)? kw_neq :
			cs_kwoptional(cs,kw_leq)? kw_leq : cs_kwoptional(cs,kw_geq)? kw_geq : kw_unknown;
		opcode op = op_none;
		if ( kw == kw_unknown )
		{
			op = cs_toptional(cs,'>')? op_gt : cs_toptional(cs,'<')? op_lt : op_none;
		}
		if ( op == op_none && kw == kw_unknown ) break;
		OvShort reg1 = top();
		exp_order8();
		OvShort reg2 = top();

		if ( (kw == kw_leq) || (kw == kw_geq) )
		{
			OvShort temp1 = push();
			OvShort temp2 = push();
			pop();pop();pop();pop();
			op = (kw == kw_leq)? op_lt : (kw == kw_geq)? op_gt : op_none;
			fs_addcode( cs->fs, cs_code( op, temp1, reg1, reg2 ) );
			fs_addcode( cs->fs, cs_code( op_eq, temp2, reg1, reg2 ) );
			fs_addcode( cs->fs, cs_code( op_or, push(), temp1, temp2 ) );
		}
		else
		{
			pop();pop();
			op = (kw == kw_eq)? op_eq : (kw == kw_neq)? op_neq : op;
			fs_addcode( cs->fs, cs_code( op, push(), reg1, reg2 ) );
		}
	}
}

void	sm_exp::exp_order8()
{
	exp_order7();
	while ( cs_kwoptional( cs, kw_or ) )
	{
		exp_order7();
		OvShort reg2 = top();pop();
		OvShort reg1 = top();pop();
		fs_addcode( cs->fs, cs_code( op_or, push(), reg1, reg2 ) );
	}
}

void	sm_exp::exp_order7()
{
	exp_order6();
	while ( cs_kwoptional( cs, kw_and ) )
	{
		exp_order6();
		OvShort reg2 = top();pop();
		OvShort reg1 = top();pop();
		fs_addcode( cs->fs, cs_code( op_and, push(), reg1, reg2 ) );
	}
}

void	sm_exp::exp_order6()
{
	exp_order5();
	while ( !cs_keyword( cs, kw_or ) && cs_toptional( cs, '|' ) )
	{
		exp_order5();
		OvShort reg2 = top();pop();
		OvShort reg1 = top();pop();
		fs_addcode( cs->fs, cs_code( op_bit_or, push(), reg1, reg2 ) );
	}
}

void	sm_exp::exp_order5()
{
	exp_order4();
	while ( cs_toptional( cs, '^' ) )
	{
		exp_order4();
		OvShort reg2 = top();pop();
		OvShort reg1 = top();pop();
		fs_addcode( cs->fs, cs_code( op_bit_xor, push(), reg1, reg2 ) );
	}
}

void	sm_exp::exp_order4()
{
	exp_order3();
	while ( !cs_keyword( cs, kw_and ) && cs_toptional( cs, '&' ) )
	{
		exp_order3();
		OvShort reg2 = top();pop();
		OvShort reg1 = top();pop();
		fs_addcode( cs->fs, cs_code( op_bit_and, push(), reg1, reg2 ) );
	}
}

void	sm_exp::exp_order3()
{
	exp_order2();
	while ( cs_keyword( cs, kw_push ) || cs_keyword( cs, kw_pull ) )
	{
		opcode op = cs_kwoptional( cs, kw_push )? op_push : cs_kwoptional( cs, kw_pull )? op_pull : op_none;
		exp_order2();
		OvShort reg2 = top();pop();
		OvShort reg1 = top();pop();
		fs_addcode( cs->fs, cs_code( op, push(), reg1, reg2 ) );
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
		fs_addcode( cs->fs, cs_code( op, push(), reg1, reg2 ) );
	}
}

void	sm_exp::exp_order1()
{
	term();
	while ( cs_ttype( cs, '*' ) || cs_ttype( cs, '/' ) || cs_ttype( cs, '%' ) )
	{
		opcode op = cs_toptional( cs, '*' )? op_mul : cs_toptional( cs, '/' )? op_div : cs_toptional( cs, '%' )? op_mod : op_none;
		term();
		OvShort reg2 = top();pop();
		OvShort reg1 = top();pop();
		fs_addcode( cs->fs, cs_code( op, push(), reg1, reg2 ) );
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
			fs_addcode( cs->fs, cs_code( op_move, push(), func, 0 ) );
			func = top();
			OvShort narg = 0;
			if ( !cs_ttype(cs,')') ) do
			{
				exp_order();
				OvShort arg = top();pop();
				++narg;
				fs_addcode( cs->fs, cs_code( op_move, push(), arg, 0 ) );
			}
			while ( cs_toptional(cs,',') );
			fs_addcode( cs->fs, cs_code( op_call, func, narg, 1 ) );
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
		MnValue vnum(cs_tnum(cs));
		push( econst, fs_findconst( cs->fs, vnum ) );
		cs_tnext(cs);
	}
	else if ( cs_ttype( cs, tt_string ) )
	{
		MnValue vstr( MOT_STRING, ut_newstring(cs->s,cs_tstr(cs)) );
		push( econst, fs_findconst( cs->fs, vstr ) );
		cs_tnext(cs);
	}
	else if ( cs_keyword(cs,kw_true) || cs_keyword(cs,kw_false) )
	{
		push( econst, fs_findconst( cs->fs, MnValue( cs_kwoptional(cs,kw_true)? true : cs_kwoptional(cs,kw_false)? false : false ) ) );
	}
	else if ( cs_keyword(cs,kw_function) )
	{
		funcexp();
	}
	else if ( cs_ttype( cs, tt_identifier ) )
	{
		expdesc exp;
		varexp(cs->fs,exp);
		push( exp.type, exp.idx );
		cs_tnext(cs);
	}
	else if ( cs_toptional( cs, '(' ) )
	{
		exp_order();
		cs_toptional( cs, ')' );
	}
}

void	sm_exp::varexp( sm_func* fs, expdesc& exp ) 
{
	if ( fs )
	{
		sm_block*	block = fs->bs;
		MnIndex idx = block->findlocal( cs_tstr(cs) );
		if ( idx >= 0 )
		{
			exp.type = evariable;
			exp.idx = idx;
			return;
		}
		else
		{
			const OvString& name = cs_tstr(cs);
			for ( MnIndex i = 0 ; i < fs->upvals.size() ; ++i )
			{
				if ( fs->upvals[i].name == name )
				{
					exp.type = eupval;
					exp.idx	 = cs_const(i);
					return;
				}
			}

			varexp( fs->last, exp );
			if ( exp.type != eglobal )
			{
				upvaldesc desc;
				desc.name	= name;
				desc.exp	= exp;

				exp.type	= eupval;
				exp.idx		= cs_const( fs->upvals.size() );

				fs->upvals.push_back( desc );

				return;
			}
		}
	}
	else
	{
		MnValue vstr( MOT_STRING, ut_newstring(cs->s,cs_tstr(cs)) );
		exp.type = eglobal;
		exp.idx = fs_findconst( cs->fs, vstr );;
	}
}

void sm_exp::funcexp() 
{
	if ( cs_keyword(cs,kw_function) )
	{
		cs_tnext(cs);

		cs_begin_func(cs);
		cs_begin_block(cs);

		cs_texpected(cs,'(');
		while ( cs_ttype(cs,tt_identifier) )
		{
			cs->fs->bs->addlocal( cs_tstr(cs) );
			cs_tnext(cs);
			if ( cs_toptional(cs,',') ) continue; else break;
		}
		cs_texpected(cs,')');

		cs_texpected(cs,'{');
		statements(cs);
		cs_texpected(cs,'}');

		sm_func* fs = cs->fs;
		cs->fs = fs->last;
		OvShort idx = fs_findconst( fs->last, MnValue( MOT_FUNCPROTO, fs->f ) );
		fs_addcode( fs->last, cs_code( op_newclosure, push(), idx, fs->upvals.size() ) );
		for each ( const upvaldesc& desc in fs->upvals )
		{
			fs_addcode( fs->last, cs_code( (desc.exp.type==eupval? op_getupval:op_move), desc.exp.idx, 0, 0 ) );
		}
		cs->fs = fs;

		cs_end_block(cs);
		cs_end_func(cs);
	}
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

OvShort fs_findconst( sm_func* fs, const MnValue& val )
{
	MnMFunction* func = fs->f;
	if ( MnIsNil(val) || MnIsBoolean(val) || MnIsNumber(val) || MnIsString(val) )
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
			else if ( MnIsBoolean(val) && MnIsBoolean(v) && (MnToBoolean(val)==MnToBoolean(v)) )
			{
				return cs_const( i );
			}
			else if ( MnIsNil(v) )
			{
				return cs_const( i );
			}
		}
	}
	func->consts.push_back( val );
	return cs_const( func->consts.size() - 1 );
}

OvReal&		cs_tnum( compile_state* cs ) { static OvReal temp=0; return (( cs->tok )? cs->tok->num:temp);};
const OvString&	cs_tstr( compile_state* cs ) { static OvString temp; return (( cs->tok )? *(cs->tok->str):temp);};

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

OvBool		cs_keyword( compile_state* cs, keyword kw )
{
	if ( cs_ttype(cs,tt_identifier) ) return ( cs_tstr(cs) == g_kwtable[kw] );
	else if ( cs->tok->type < tt_identifier )
	{
		s_token* tok = cs->tok;
		OvString str;
		while ( cs->tok->type < tt_identifier )
		{
			str.push_back( (char)cs->tok->type );
			cs_tnext(cs);
		}
		cs->tok = tok;
		return ( str == g_kwtable[kw] );
	}
	return false;
}

OvBool		cs_kwoptional( compile_state* cs, keyword kw )
{
	if ( cs_ttype(cs,tt_identifier) && ( cs_tstr(cs) == g_kwtable[kw] ) )
	{
		cs_tnext(cs);
		return true;
	}
	else if ( cs->tok->type < tt_identifier )
	{
		s_token* tok = cs->tok;
		OvString str;
		while ( cs->tok->type < tt_identifier )
		{
			str.push_back( (char)cs->tok->type );
			cs_tnext(cs);
		}
		if ( str == g_kwtable[kw] ) return true;
		else
		{
			cs->tok = tok;
			return false;
		}
	}
	return false;
}

void		cs_scan_file( compile_state* cs, const OvString& file )
{
	OvFileInputStream fis( file );
	cs->is = &fis;
	cs->is->Read(cs->c);
	cs_scan(cs);
}

const OvString* cs_new_str( compile_state* cs, OvString& str ) 
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

s_token* cs_new_tok( compile_state* cs, OvInt type, const OvString* s ) 
{
	s_token* tok = new(ut_alloc( sizeof(s_token) )) s_token(s);
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

			cs_new_tok( cs, tt_string, cs_new_str(cs, str) );
		}
		else if ( isalpha(c) || c == '_' )
		{
			OvString str;
			do
			{
				str.push_back(c);
			} while ( cp_read() && (isalnum(c) || c == '_') );

			cs_new_tok( cs, tt_identifier, cs_new_str(cs, str) );
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

void	cs_begin_func( compile_state* cs )
{
	sm_func* fs		= new(ut_alloc(sizeof(sm_func))) sm_func;
	fs->f			= ut_newfunction(cs->s);
	fs->bs			= NULL;
	fs->last		= cs->fs;
	fs->maxstack	= 0;

	cs->fs			= fs;;
}

void	cs_end_func( compile_state* cs )
{
	sm_func* fs	= cs->fs;
	fs->f->maxstack = fs->maxstack;
	fs_addcode( cs->fs, cs_code( op_return, 0, 0, 0 ) );
	cs->fs = fs->last;
	ut_free( fs );
}

void	cs_begin_block( compile_state* cs )
{
	sm_block* bs	= new(ut_alloc(sizeof(sm_block))) sm_block;
	bs->outer		= cs->fs->bs;
	bs->fs			= cs->fs;

	cs->fs->bs		= bs;
}

void	cs_end_block( compile_state* cs )
{
	sm_block* bs	= cs->fs->bs;
	cs->fs->bs		= bs->outer;
	OvShort base	= cs->fs->bs? cs->fs->bs->nlocals():0;
	ut_free( bs );
}

void		fs_addcode( sm_func* fs, const MnInstruction& i )
{
	fs->f->codes.push_back( i );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

MnValue	load_entrance( MnState* s, const OvString& file )
{
	compile_state cs(s);
	cs_scan_file( &cs, file );

	cs_begin_func(&cs);
	cs_begin_block(&cs);
	statements(&cs);
	fs_addcode( cs.fs, cs_code( op_return, 0, 0, 0 ) );
	MnMFunction* func = cs.fs->f;
	cs_end_block(&cs);
	cs_end_func(&cs);


	return MnValue( MOT_FUNCPROTO, func );
}

void statements( compile_state* cs )
{
	while ( stat(cs) );
}

OvBool stat( compile_state* cs )
{
	if ( cs_ttype(cs,'{') )				 	{ stat_block( cs ); return true; }
	else if ( cs_keyword(cs,kw_local) ) 	{ stat_local(cs); return true; }
	else if ( cs_keyword(cs,kw_global) ) 	{ stat_global(cs); return true; }
	else if ( cs_keyword(cs,kw_return) ) 	{ stat_return(cs); return true; }
	else if ( cs_keyword(cs,kw_if) ) 		{ stat_if(cs); return true; }
	else if ( cs_keyword(cs,kw_while) ) 	{ stat_while(cs); return true; }
	else if ( cs_keyword(cs,kw_break) ) 	{ stat_break(cs); return true; }
	else if ( cs_toptional(cs,';') )		{ return true; }
	else if ( cs_toptional(cs,tt_eos) ) 		{ return false; }
	else
	{
		sm_exp exp(cs);
		exp.statexp();
		cs_texpected(cs,';');
		return (exp.get(-1).type != enone);
	}
	return false;
}

void		stat_block( compile_state* cs )
{
	if ( cs_texpected(cs,'{') )
	{
		OvSize		begin	= cs->fs->bs->nlocals();
		cs_begin_block(cs);
		statements(cs);
		cs_texpected(cs,'}');
		cs_end_block(cs);
		fs_addcode( cs->fs, cs_code(op_close_upval,0,begin,0) );
	}
};

void		stat_if( compile_state* cs )
{
	if ( cs_kwoptional(cs,kw_if) )
	{
		cs_texpected(cs,'(');
		sm_exp exp(cs);
		exp.statexp();
		cs_texpected(cs,')');

		OvInt ifjmp = cs->fs->f->codes.size();
		fs_addcode( cs->fs, cs_code(op_fjp,0,0,exp.top()) );

		stat(cs);

		OvInt elsejmp = cs->fs->f->codes.size();

		if ( cs_kwoptional(cs,kw_else) )
		{
			fs_addcode( cs->fs, cs_code(op_jmp,0,0,0) );
			stat(cs);
			cs_setb( cs->fs->f->codes[elsejmp], cs->fs->f->codes.size() - elsejmp );
			++elsejmp;
		}
		cs_setb( cs->fs->f->codes[ifjmp], elsejmp - ifjmp );
	}
}

void		stat_while( compile_state* cs )
{
	if ( cs_kwoptional(cs,kw_while) )
	{
		OvInt begin = cs->fs->f->codes.size();

		cs_texpected(cs,'(');
		sm_exp exp(cs);
		exp.statexp();
		cs_texpected(cs,')');

		OvInt fix  = cs->breaks.size();
		OvInt fjmp = cs->fs->f->codes.size();
		fs_addcode( cs->fs, cs_code(op_fjp,0,0,exp.top()) );

		stat(cs);

		OvInt end = cs->fs->f->codes.size();
		fs_addcode( cs->fs, cs_code(op_jmp,0,0,0) );

		cs_setb( cs->fs->f->codes[fjmp], end - fjmp + 1 );
		cs_setb( cs->fs->f->codes[end], begin - end );
		for ( OvInt i = fix ; i < cs->breaks.size() ; ++i )
		{
			MnIndex brk = cs->breaks[i];
			cs_setb( cs->fs->f->codes[ brk ], end - brk + 1 );
		}
		cs->breaks.resize(fix);
	}
}

void		stat_break( compile_state* cs )
{
	if ( cs_kwoptional(cs,kw_break) )
	{
		fs_addcode( cs->fs, cs_code(op_close_upval,0,0,0) );
		cs->breaks.push_back( cs->fs->f->codes.size() );
		fs_addcode( cs->fs, cs_code(op_jmp,0,0,0) );
	}
}

void stat_local( compile_state* cs ) 
{
	if ( cs_keyword(cs,kw_local) )
	{
		cs_tnext(cs);
		if ( cs_ttype(cs,tt_identifier) )
		{
			cs->fs->bs->addlocal( cs_tstr(cs) );
			OvSize nfixed = cs->fs->bs->nlocals();
			cs->fs->maxstack = max(nfixed,cs->fs->maxstack);
			sm_exp exp(cs);
			exp.statexp();
			cs_texpected(cs,';');
		}
	}
}

void stat_global( compile_state* cs ) 
{
	if ( cs_keyword(cs,kw_global) )
	{
		cs_tnext(cs);
		if ( cs_ttype(cs,tt_identifier) )
		{
			sm_exp exp(cs);
			exp.statexp();
		}
	}
}

void stat_return( compile_state* cs )
{
	if ( cs_kwoptional(cs,kw_return) )
	{
		sm_exp eret(cs);
		eret.statexp();
		OvShort idx = eret.top();
		if ( !(idx < 0) )
		{
			fs_addcode( cs->fs, cs_code( op_move, cs->fs->maxstack++, idx, 0 ) );
		}

		fs_addcode( cs->fs, cs_code( op_return, !(idx < 0), 0, 0 ) );
	}
}

//////////////////////////////////////////////////////////////////////////
