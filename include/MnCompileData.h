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

class CmFuncinfo
{
public:
	CmFuncinfo*  last;
	MnMFunction* func;

	OvVector<OvHash32>	 locals;
	OvBufferOutputStream codestream;

	CmFuncinfo() : last(NULL), func(NULL), codestream(NULL) {};
};

enum CmExprType
{
	et_none,
	et_nil,
	et_const,
	et_number,
	et_boolean,
	et_rvalue,
	et_global,
	et_local,
	et_upval,
	et_field,
	et_call,
};

class CmExprInfo
{
public:
	CmExprType	  type;
	
	union
	{
		OvByte		  idx;
		MnNumber	  num;
		MnNumber	  blr;
	};

	CmExprInfo() : type(et_none), num(0) {};
};

class CmCompileException
{
public:
	CmCompileException( const OvString& _msg ) : msg(_msg) {};
	OvString msg;
};

class CmCompiler
{
public:

	MnState*			s;

	OvVector<CmToken>	tokens;
	OvUInt				tokpos;
	OvVector<OvUInt>	savepos;

	CmFuncinfo*			fi;
	CmExprInfo			exprinfo;

	CmCompiler( MnState* _s ) : s(_s), tokpos(0) {};
};

namespace statement
{
	typedef OvBool (*statfunc)(CmCompiler*);

	OvBool option( CmCompiler* cm, statfunc func );
	OvBool	match( CmCompiler* cm, statfunc func );
	void	rvalue( CmCompiler* cm );
	void	free_expr( CmCompiler* cm );

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
		void	compile( CmCompiler* cm );
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