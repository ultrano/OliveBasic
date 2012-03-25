#pragma once

enum CmTokenType
{
	tt_identifier = 256,
	tt_number	  = 257,
	tt_string	  = 258,
	tt_eos		  = 259,
	tt_unknown	  = 260,
};

class CmToken
{
public:
	OvInt	type;
	OvUInt row, col;
	MnValue val;
	CmToken() : type(tt_unknown) , row(0), col(0) {};
	CmToken( OvInt t, OvUInt r, OvUInt c ) : type(t) , row(r), col(c) {};
};

class CmSyminfo
{
public:

	OvByte	 type;
	OvHash32 hash;
	OvUInt	 level;
	MnIndex	 index;

};

class CmFuncinfo
{
public:
	CmFuncinfo*  last;
	MnMFunction* func;
	OvUInt		 level;

	CmFuncinfo() : last(NULL), func(NULL), level(0) {};
};

enum CmExprType
{
	et_none,
	et_temp,
	et_const,
	et_field,
	et_method,
	et_variable,
};

class CmExpression
{
public:
	CmExprType	  type;
	MnIndex		  idx;
	MnIndex		  extra;

	CmExpression() : type(et_none), idx(0), extra(0) {};
};

class CmCompiler
{
public:

	MnState*			s;

	OvVector<CmToken>	tokens;
	OvUInt				tokpos;
	OvVector<OvUInt>	savepos;

	OvVector<OvUInt>	level;
	OvVector<CmSyminfo>	symbols;
	OvVector<CmExpression> exprs;
	OvUInt				tempcount;

	CmFuncinfo*			fi;

	CmCompiler( MnState* _s ) : s(_s), tokpos(0) {};
};

namespace statement
{
	typedef OvBool (*statfunc)(CmCompiler*);

	OvBool option( CmCompiler* cm, statfunc func );

	OvBool	match( CmCompiler* cm, statfunc func );

	void	pushtemp( CmCompiler* cm );
	void	popexpr( CmCompiler* cm, OvSize count );
	void	addbiop( CmCompiler* cm, opcode op );

	namespace multi_stat
	{
		OvBool	test( CmCompiler* cm );
		OvBool	interpret( CmCompiler* cm );
	}

	namespace single_stat
	{
		OvBool	test( CmCompiler* cm );
		OvBool	interpret( CmCompiler* cm );
	}

	namespace local
	{
		OvBool	test( CmCompiler* cm );
		OvBool	interpret( CmCompiler* cm );
	}

	namespace block
	{
		OvBool	test( CmCompiler* cm );
		OvBool	interpret( CmCompiler* cm );
	}

	namespace expression
	{
		OvBool	test( CmCompiler* cm );
		OvBool	interpret( CmCompiler* cm );
	}

	namespace expr10
	{
		OvBool	test( CmCompiler* cm );
		OvBool	interpret( CmCompiler* cm );
	}

	namespace expr9
	{
		OvBool	test( CmCompiler* cm );
		OvBool	interpret( CmCompiler* cm );
	}

	namespace expr8
	{
		OvBool	test( CmCompiler* cm );
		OvBool	interpret( CmCompiler* cm );
	}

	namespace expr7
	{
		OvBool	test( CmCompiler* cm );
		OvBool	interpret( CmCompiler* cm );
	}

	namespace expr6
	{
		OvBool	test( CmCompiler* cm );
		OvBool	interpret( CmCompiler* cm );
	}

	namespace expr5
	{
		OvBool	test( CmCompiler* cm );
		OvBool	interpret( CmCompiler* cm );
	}

	namespace expr4
	{
		OvBool	test( CmCompiler* cm );
		OvBool	interpret( CmCompiler* cm );
	}

	namespace expr3
	{
		OvBool	test( CmCompiler* cm );
		OvBool	interpret( CmCompiler* cm );
	}

	namespace expr2
	{
		OvBool	test( CmCompiler* cm );
		OvBool	interpret( CmCompiler* cm );
	}

	namespace expr1
	{
		OvBool	test( CmCompiler* cm );
		OvBool	interpret( CmCompiler* cm );
	}

	namespace term
	{
		OvBool	test( CmCompiler* cm );
		OvBool	interpret( CmCompiler* cm );
	}

	namespace postexpr
	{
		OvBool	test( CmCompiler* cm );
		OvBool	interpret( CmCompiler* cm );
	}

	namespace primary
	{
		OvBool	test( CmCompiler* cm );
		OvBool	interpret( CmCompiler* cm );
	}

	namespace funcdesc
	{
		OvBool	test( CmCompiler* cm );
		OvBool	interpret( CmCompiler* cm );
	}

	namespace funcargs
	{
		OvBool	test( CmCompiler* cm );
		OvBool	interpret( CmCompiler* cm );
	}

	namespace callargs
	{
		OvBool	test( CmCompiler* cm );
		OvBool	interpret( CmCompiler* cm );
	}

	namespace returnstat
	{
		OvBool	test( CmCompiler* cm );
		OvBool	interpret( CmCompiler* cm );
	}

	namespace ifstat
	{
		OvBool	test( CmCompiler* cm );
		OvBool	interpret( CmCompiler* cm );
	}

	namespace whilestat
	{
		OvBool	test( CmCompiler* cm );
		OvBool	interpret( CmCompiler* cm );
	}
}