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

struct CmToken
{
	OvUInt row, col;
	OvInt	type;
	MnValue val;
};

struct CmCompiler
{

};