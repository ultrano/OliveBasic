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

class CmCompiler
{
public:

	MnState*			s;

	OvVector<CmToken>	tokens;
	OvUInt				tokpos;
	OvVector<OvUInt>	savepos;

	CmCompiler( MnState* _s ) : s(_s), tokpos(0) {};
};



namespace statement
{
	//! �׽�Ʈ �Լ��� ��ū�� ������ �ǵ����� �ʴ´�.
	typedef OvBool (*testfunc)(CmCompiler*);
	//! �ɼ� �Լ��� �׽�Ʈ�� �����ϸ� ��ū ����, �ƴϸ� �ǵ�����.
	OvBool option( CmCompiler* cm, testfunc func );
	//! ��ġ �Լ��� �׽�Ʈ�� �������ο� ���� ���� ��ū�� �ǵ�����.
	OvBool	match( CmCompiler* cm, testfunc func );

	namespace single_stat
	{
		OvBool	test( CmCompiler* cm );
		OvBool	parse( CmCompiler* cm );
	}
	namespace local
	{
		OvBool	test( CmCompiler* cm );
		OvBool	parse( CmCompiler* cm );
	};

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
}