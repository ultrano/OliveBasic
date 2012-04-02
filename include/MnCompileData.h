#pragma once

enum CmTokenType
{
	tt_identifier = 256,
	tt_eos		  = 257,
	tt_unknown	  = 258,
	tt_number	  = 259,
	tt_string	  = 260,
	tt_char		  = 261,
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

	OvHash32 hash;	//!< symbol name hash
	OvUInt	 level; //!< where symb is
	MnIndex	 index; //!< local stack index

};

class CmFuncinfo
{
public:
	CmFuncinfo*  last;
	MnMFunction* func;
	OvUInt		 level;
	OvBufferOutputStream codestream;

	CmFuncinfo() : last(NULL), func(NULL), level(0), codestream(NULL) {};
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
	OvVector<CmSyminfo>	locals;

	CmFuncinfo*			fi;

	CmCompiler( MnState* _s ) : s(_s), tokpos(0) {};
};

namespace statement
{
	typedef OvBool (*statfunc)(CmCompiler*);

	OvBool option( CmCompiler* cm, statfunc func );
	OvBool	match( CmCompiler* cm, statfunc func );

	namespace multi_stat
	{
		void	compile( CmCompiler* cm );
	}

	namespace single_stat
	{
		void	compile( CmCompiler* cm );
	}

	namespace local
	{
		void	compile( CmCompiler* cm );
	}

	namespace addsymb
	{
		void	compile( CmCompiler* cm );
	}
	namespace block
	{
		void	compile( CmCompiler* cm );
	}

	namespace expression
	{
		void	compile( CmCompiler* cm );
	}

	namespace expr10
	{
		void	compile( CmCompiler* cm );
	}

	namespace expr9
	{
		void	compile( CmCompiler* cm );
	}

	namespace expr8
	{
		void	compile( CmCompiler* cm );
	}

	namespace expr7
	{
		void	compile( CmCompiler* cm );
	}

	namespace expr6
	{
		void	compile( CmCompiler* cm );
	}

	namespace expr5
	{
		void	compile( CmCompiler* cm );
	}

	namespace expr4
	{
		void	compile( CmCompiler* cm );
	}

	namespace expr3
	{
		void	compile( CmCompiler* cm );
	}

	namespace expr2
	{
		void	compile( CmCompiler* cm );
	}

	namespace expr1
	{
		void	compile( CmCompiler* cm );
	}

	namespace term
	{
		void	compile( CmCompiler* cm );
	}

	namespace postexpr
	{
		void	compile( CmCompiler* cm );
	}

	namespace primary
	{
		CmExprType	compile( CmCompiler* cm );
	}

	namespace funcdesc
	{
		void	compile( CmCompiler* cm );
	}

	namespace funcargs
	{
		void	compile( CmCompiler* cm );
	}

	namespace callargs
	{
		void	compile( CmCompiler* cm );
	}

	namespace returnstat
	{
		void	compile( CmCompiler* cm );
	}

	namespace ifstat
	{
		void	compile( CmCompiler* cm );
	}

	namespace whilestat
	{
		void	compile( CmCompiler* cm );
	}
}