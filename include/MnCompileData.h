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

struct CmLabelInfo
{
	OvHash32 hash;
	OvUInt	 pos;
	CmLabelInfo( OvHash32 h, OvUInt p ) : hash(h), pos(p) {};
	CmLabelInfo() {};
};

struct CmGotoInfo
{
	OvUInt		pos;
	OvHash32	label;
};

class CmFuncinfo
{
public:
	CmFuncinfo*  last;
	MnMFunction* func;

	OvVector<OvHash32>		locals;
	OvVector<CmLabelInfo>	labels;
	OvVector<CmGotoInfo>	gotos;
	OvVector<OvInt>			breaks;
	MnCodeWriter		codewriter;

	CmFuncinfo() : last(NULL), func(NULL), codewriter(NULL) {};
};

enum CmExprType
{
	et_none,
	et_nil,
	et_closure,
	et_const,
	et_boolean,
	et_number,
	et_rvalue,
	et_global,
	et_local,
	et_upval,
	et_field,
	et_call,
	et_methodcall,
};

class CmExprInfo
{
public:
	CmExprType	  type;
	
	union
	{
		OvByte		  idx;
		MnNumber	  num;
		OvBool		  blr;
		struct { OvByte func; OvByte nupvals; };
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

	OvBool	option( CmCompiler* cm, statfunc func );
	OvBool	match( CmCompiler* cm, statfunc func );
	void	rvalue( CmCompiler* cm );
	void	free_expr( CmCompiler* cm );
	void	resolve_goto( CmCompiler* cm, CmFuncinfo* fi );
	OvByte	addconst( CmCompiler* cm, const MnValue& val );
	OvInt	jumping( CmCompiler* cm, OvByte op );

	namespace multi_stat
	{
		void	compile( CmCompiler* cm );
	}

	namespace single_stat
	{
		void	compile( CmCompiler* cm );
	}

	namespace label_stat
	{
		void	compile( CmCompiler* cm );
	}

	namespace goto_stat
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

	namespace funcbody
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

	namespace if_stat
	{
		void	compile( CmCompiler* cm );
	}

	namespace whilestat
	{
		void	compile( CmCompiler* cm );
	}

	namespace breakstat
	{
		void	compile( CmCompiler* cm );
	}
}