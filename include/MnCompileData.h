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
	OvInt	type;
	OvUInt	level;
	MnIndex	index;
};

class CmCompiler
{
public:

	MnState*			s;

	OvVector<CmToken>	tokens;
	OvUInt				tokpos;
	OvVector<OvUInt>	savepos;

	OvUInt				level;
	OvVector<CmSyminfo>	symbols;

	CmCompiler( MnState* _s ) : s(_s), tokpos(0) {};
};

namespace statement
{
	//! 테스트 함수는 토큰의 진행을 되돌리진 않는다.
	typedef OvBool (*testfunc)(CmCompiler*);
	//! 옵션 함수는 테스트가 성공하면 토큰 진행, 아니면 되돌린다.
	OvBool option( CmCompiler* cm, testfunc func );
	//! 매치 함수는 테스트의 성공여부와 관계 없이 토큰을 되돌린다.
	OvBool	match( CmCompiler* cm, testfunc func );

	namespace multi_stat
	{
		OvBool	test( CmCompiler* cm );
		OvBool	parse( CmCompiler* cm );
	}

	namespace single_stat
	{
		OvBool	test( CmCompiler* cm );
		OvBool	parse( CmCompiler* cm );
	}

	namespace local
	{
		OvBool	test( CmCompiler* cm );
		OvBool	parse( CmCompiler* cm );
	}

	namespace block
	{
		OvBool	test( CmCompiler* cm );
		OvBool	parse( CmCompiler* cm );
	}

	namespace expression
	{
		OvBool	test( CmCompiler* cm );
		OvBool	parse( CmCompiler* cm );
	}

	namespace expr10
	{
		OvBool	test( CmCompiler* cm );
		OvBool	parse( CmCompiler* cm );
	}

	namespace expr9
	{
		OvBool	test( CmCompiler* cm );
		OvBool	parse( CmCompiler* cm );
	}

	namespace expr8
	{
		OvBool	test( CmCompiler* cm );
		OvBool	parse( CmCompiler* cm );
	}

	namespace expr7
	{
		OvBool	test( CmCompiler* cm );
		OvBool	parse( CmCompiler* cm );
	}

	namespace expr6
	{
		OvBool	test( CmCompiler* cm );
		OvBool	parse( CmCompiler* cm );
	}

	namespace expr5
	{
		OvBool	test( CmCompiler* cm );
		OvBool	parse( CmCompiler* cm );
	}

	namespace expr4
	{
		OvBool	test( CmCompiler* cm );
		OvBool	parse( CmCompiler* cm );
	}

	namespace expr3
	{
		OvBool	test( CmCompiler* cm );
		OvBool	parse( CmCompiler* cm );
	}

	namespace expr2
	{
		OvBool	test( CmCompiler* cm );
		OvBool	parse( CmCompiler* cm );
	}

	namespace expr1
	{
		OvBool	test( CmCompiler* cm );
		OvBool	parse( CmCompiler* cm );
	}

	namespace term
	{
		OvBool	test( CmCompiler* cm );
		OvBool	parse( CmCompiler* cm );
	}

	namespace postexpr
	{
		OvBool	test( CmCompiler* cm );
		OvBool	parse( CmCompiler* cm );
	}

	namespace primary
	{
		OvBool	test( CmCompiler* cm );
		OvBool	parse( CmCompiler* cm );
	}

	namespace funcdesc
	{
		OvBool	test( CmCompiler* cm );
		OvBool	parse( CmCompiler* cm );
	}

	namespace funcargs
	{
		OvBool	test( CmCompiler* cm );
		OvBool	parse( CmCompiler* cm );
	}

	namespace callargs
	{
		OvBool	test( CmCompiler* cm );
		OvBool	parse( CmCompiler* cm );
	}

	namespace returnstat
	{
		OvBool	test( CmCompiler* cm );
		OvBool	parse( CmCompiler* cm );
	}

	namespace ifstat
	{
		OvBool	test( CmCompiler* cm );
		OvBool	parse( CmCompiler* cm );
	}

	namespace whilestat
	{
		OvBool	test( CmCompiler* cm );
		OvBool	parse( CmCompiler* cm );
	}
}