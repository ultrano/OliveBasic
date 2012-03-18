#pragma once

struct CmStatement
{
	struct Function;
	struct While;
};

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
	CmCompiler( MnState* _s ) : s(_s), tokpos(0) {};
	MnState*			s;

	OvVector<CmToken>	tokens;
	OvUInt				tokpos;
	OvVector<OvUInt>	savepos;
};

#define cm_tok			( ( cm->tokpos<cm->tokens.size() )? cm->tokens[cm->tokpos] : CmToken() )
#define cm_lookahead	( ((cm->tokpos+1)<(cm->tokens.size()))? cm->tokens[cm->tokpos+1] : CmToken() )
#define cm_next()		( ++cm->tokpos )
#define cm_prev()		( --cm->tokpos )

#define cm_typematch(t) ( (cm_tok.type) == (t) )

#define cm_savepos()	( cm->savepos.push_back( cm->tokpos ) )
#define cm_loadpos()	(if (cm->savepos.size()) {cm->tokpos = cm->savepos.back();cm->savepos.pop_back();})