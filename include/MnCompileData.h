#pragma once

enum eStoreType
{
	stConst,
	stStack,
	stGlobal,
	stTemp,
};

struct CmStoreInfo
{
	eStoreType	type;
	MnIndex		idx;
};

struct CmExpression
{
	CmStoreInfo store1;
	CmStoreInfo store2;
};

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


#define cm_tok				( ( cm->tokpos<cm->tokens.size() )? cm->tokens[cm->tokpos] : CmToken() )
#define cm_lookahead		( ((cm->tokpos+1)<(cm->tokens.size()))? cm->tokens[cm->tokpos+1] : CmToken() )
#define cm_savepos()		( cm->savepos.push_back( cm->tokpos ), (cm->savepos.size()-1) )
#define cm_loadpos(idx)		( if (idx<cm->savepos.size()) {cm->tokpos = cm->savepos.back();cm->savepos.resize(idx);} )

#define cm_toktest(t)		( (cm_tok.type) == (t) )
#define cm_tokmust(t)		( cm_toktest(t)? (cm_toknext(),true) : cm_errmessage(#t + " is missing\n") )
#define cm_tokoption(t)		( cm_toktest(t)? (cm_toknext(),true) : false )
#define cm_tokkeyword(kw)	( cm_toktest(tt_identifier) && (MnToString(cm_tok.val)->get_str() == kw) )
#define cm_toknext()		( ++cm->tokpos )

#define cm_statmust(stat)	(statement::option(cm,statement::stat::test)? true : cm_errmessage(#stat + " is missing\n") )
#define cm_statoption(stat)	(statement::option(cm,statement::stat::test))
#define cm_statmatch(stat)	(statement::match(cm,statement::stat::test))
#define cm_statparse(stat)	(statement::stat::parse(cm))

#define cm_errmessage(msg)	(false)

namespace statement
{
	typedef OvBool (*testfunc)(CmCompiler*);
	OvBool option( CmCompiler* cm, testfunc func )
	{
		OvUInt saveidx = cm_savepos();
		OvBool ret = func();
		if ( !ret ) cm_loadpos(saveidx);
		return ret;
	}
	OvBool	match( CmCompiler* cm, testfunc func )
	{
		OvUInt saveidx = cm_savepos();
		OvBool ret = func();
		cm_loadpos(saveidx);
		return ret;
	}
	namespace local
	{
		OvBool	test( CmCompiler* cm )
		{
			OvBool ret = cm_tokkeyword("local");cm_toknext();
			ret = ret && cm_toktest(tt_identifier);cm_toknext();
			if ( cm_tokoption('=') )
			{
				ret = ret && cm_statmust(expression);
			}
			ret = ret && cm_tokmust(';');
		}
		OvBool	parse( CmCompiler* cm )
		{
			if ( match(cm) ) next(cm);
		}
	};
	
	namespace expression
	{
		OvBool	check( CmCompiler* cm )
		{

		}
	}
}