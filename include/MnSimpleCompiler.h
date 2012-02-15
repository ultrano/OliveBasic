#pragma once
#include "MnInternal.h"

struct MnCompileState : public OvMemObject
{
	MnState* state;
	OvInputStream* is;
	OvChar c;
	MnValue	errfunc;
};

OvInt cp_exec_func( MnState* s, MnMFunction* func )
{

	s->pc = func->codes.size()? &(func->codes[0]) : NULL;
	while ( s->pc )
	{
		MnOpCodeABC& i = (MnOpCodeABC&)*s->pc; ++s->pc;
		switch ( i.op )
		{
		case MOP_NEWTABLE:	mn_newtable( s ); break;
		case MOP_NEWARRAY:	mn_newarray( s ); break;
		case MOP_NEWCLOSURE:
			{
				MnClosure* cls = ut_newMclosure(s);
				MnClosure::MClosure* mcls = cls->u.m;
				mcls->func = func->consts[i.bx-1];
				ut_pushvalue(s,MnValue(MOT_CLOSURE,cls));
			}
			break;

		case MOP_SET_STACK:	mn_setstack( s, i.bx ); break;
		case MOP_GET_STACK:	mn_getstack( s, i.bx ); break;

		case MOP_REMOVE:	mn_remove( s, i.bx ); break;
		case MOP_INSERT:	mn_insert( s, i.bx ); break;
		case MOP_SWAP:		mn_swap( s, i.a, i.b ); break;
		case MOP_REPLACE:	mn_replace( s, i.a, i.b ); break;

		case MOP_SET_FIELD:	mn_setfield( s, i.bx); break;
		case MOP_GET_FIELD:	mn_getfield( s, i.bx); break;

		case MOP_SET_GLOBAL:	mn_setglobal( s ); break;
		case MOP_GET_GLOBAL:	mn_getglobal( s ); break;

		case MOP_SET_META:	mn_setmeta( s, i.bx ); break;
		case MOP_GET_META:	mn_getmeta( s, i.bx ); break;

		case MOP_SET_UPVAL:	mn_setupval( s, i.bx ); break;
		case MOP_GET_UPVAL:	mn_getupval( s, i.bx ); break;

		case MOP_PUSH:		ut_pushvalue( s, func->consts[i.bx-1] ); break;
		case MOP_POP:		mn_pop( s, i.bx ); break;

		case MOP_CALL:		mn_call( s, i.a, i.b ); break;

		case MOP_LOG:
			{
				MnValue log = func->consts[i.bx-1];
				printf( MnToString(log)->get_str().c_str() );
				printf("\n");
			}
			break;

		case MOP_JMP: s->pc += (i.bx - 1); break;
		case MOP_RET: return i.bx;

		case MOP_NOT:
			{
				OvBool b = mn_toboolean(s,-1);
				mn_pop(s,1);
				mn_pushboolean(s,!b);
			}
			break;

		case MOP_CMP:
			{
				OvBool b = mn_toboolean(s,-1);
				mn_pop(s,1);
				if ( !b ) ++s->pc;
			}
			break;

		case MOP_EQ:
		case MOP_LT:
		case MOP_GT:
			{
				MnValue ret = ut_method_logical( s, (MnOperate)i.op, ut_getstack( s, -2 ), ut_getstack( s, -1 ) );
				mn_pop(s,2);
				ut_pushvalue(s,ret);
			}
			break;

		case MOP_ADD:
		case MOP_SUB:
		case MOP_MUL:
		case MOP_DIV:
			{
				MnValue ret = ut_method_arith( s, (MnOperate)i.op, ut_getstack( s, -2 ), ut_getstack( s, -1 ) );
				mn_pop(s,2);
				ut_pushvalue(s,ret);
			}
			break;

		case MOP_NONEOP:
		default:
			return 0;
		}
	}
	return 0;
}

void cp_build_func( MnCompileState* cs, MnMFunction* func );

enum eErrCode
{
	eErrCode_Unknown,
};

void cp_call_errfunc( MnCompileState* cs, OvInt errcode, const OvString& msg )
{
	ut_pushvalue( cs->state, cs->errfunc );
	mn_pushnumber( cs->state, errcode );
	mn_pushstring( cs->state, msg );
	mn_call( cs->state, 2, 0 );
}

enum
{
	eTNumber = 256,
	eTString,
	eTIdentifier,
	eTEndOfStream,
	eTUnknown,
};

OvInt cp_token( MnCompileState* cs, OvInt& num, OvString& str )
{
	OvChar& c = cs->c;
	num = 0;
	str.clear();
#define cp_read() (cs->is->Read(c))
	while ( true )
	{
		if ( c == EOF  )
		{
			return eTEndOfStream;
		}
		else if ( isdigit(c) )
		{
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
			return eTNumber;
		}
		else if ( c == '"' )
		{
			cp_read();
			do
			{
				str.push_back(c);
			} while ( cp_read() && c != '"' );
			cp_read();
			return eTString;
		}
		else if ( isalpha(c) || c == '_' )
		{
			do
			{
				str.push_back(c);
			} while ( cp_read() && (isalnum(c) || c == '_') );
			return eTIdentifier;
		}
		else
		{
			OvChar ret = c;
			cp_read();
			return ret;
		}
	}
	return eTUnknown;
#undef cp_read
}

MnOperate cp_str2op( OvString str ) 
{
	if ( str == "push" ) return MOP_PUSH;
	else if ( str == "pop" ) return MOP_POP;
	if ( str == "log" ) return MOP_LOG;

	else if ( str == "newarray" ) return MOP_NEWARRAY;
	else if ( str == "newtable" ) return MOP_NEWTABLE;
	else if ( str == "newclosure" ) return MOP_NEWCLOSURE;

	else if ( str == "setstack" ) return MOP_SET_STACK;
	else if ( str == "getstack" ) return MOP_GET_STACK;

	else if ( str == "remove" ) return MOP_REMOVE;
	else if ( str == "insert" ) return MOP_INSERT;
	else if ( str == "swap" ) return MOP_SWAP;
	else if ( str == "replace" ) return MOP_REPLACE;

	else if ( str == "setfield" ) return MOP_SET_FIELD;
	else if ( str == "getfield" ) return MOP_GET_FIELD;

	else if ( str == "setglobal" ) return MOP_SET_GLOBAL;
	else if ( str == "getglobal" ) return MOP_GET_GLOBAL;

	else if ( str == "setmeta" ) return MOP_SET_META;
	else if ( str == "getmeta" ) return MOP_GET_META;

	else if ( str == "setupval" ) return MOP_SET_UPVAL;
	else if ( str == "getupval" ) return MOP_GET_UPVAL;

	else if ( str == "jmp" )   return MOP_JMP;
	else if ( str == "cmp" )   return MOP_CMP;

	else if ( str == "add" )   return MOP_ADD;
	else if ( str == "sub" )   return MOP_SUB;
	else if ( str == "mul" )   return MOP_MUL;
	else if ( str == "div" )   return MOP_DIV;

	else if ( str == "not" )   return MOP_NOT;

	else if ( str == "eq" )   return MOP_EQ;
	else if ( str == "lt" )   return MOP_LT;
	else if ( str == "gt" )   return MOP_GT;

	else if ( str == "call" )   return MOP_CALL;
	else if ( str == "ret" )   return MOP_RET;

	else if ( str == "linkstack" )   return MOP_LINK_STACK;
	else if ( str == "linkupval" )   return MOP_LINK_UPVAL;

	return MOP_NONEOP;
}

OvBool cp_operate( MnCompileState* cs, MnOperate& op )
{
	OvString str;
	OvInt num;
	OvInt tok;
	do tok = cp_token(cs,num,str); while ( isspace((OvChar)tok) );

	op = MOP_NONEOP;
	if ( tok == eTIdentifier )
	{
		op = cp_str2op(str);
		if ( op == MOP_NONEOP ) cp_call_errfunc( cs, 0, (str + " - unknown operate\n"));
		return true;
	}
	else if ( (OvChar)tok == ':' )
	{
		do tok = cp_token(cs,num,str); while ( isspace((OvChar)tok) );
		if ( str == "end" )
		{
			return false;
		}
	}
	return true;
}

OvInt	  cp_operand( MnCompileState* cs )
{
	OvString str;
	OvInt num;
	OvInt tok;
	do { tok = cp_token(cs,num,str); } while ( isspace((OvChar)tok) );

	bool minus = ( tok == '-' );
	if ( minus )
	{
		do { tok = cp_token(cs,num,str); } while ( isspace((OvChar)tok) );
	}

	if ( tok == eTNumber )
	{
		return num * (minus? -1:1);
	}
	return 0;
}

MnOperand cp_func_const( MnCompileState* cs, MnMFunction* func )
{
	OvString str;
	OvInt num;
	OvInt tok;
	do 
	{
		tok = cp_token(cs,num,str);
	} while ( isspace((OvChar)tok) );

	if ( tok == eTIdentifier )
	{
		if ( str == "false" || str == "true" )
		{
			for ( MnIndex i = 0 ; i < func->consts.size() ; ++i )
			{
				MnValue& val = func->consts[i];
				if ( MnIsBoolean(val) && MnToBoolean(val) == (str=="true") ) return (i+1);
			}
			func->consts.push_back( MnValue( (OvBool)(str=="true") ) );
		}
		else if ( str == "nil" )
		{
			for ( MnIndex i = 0 ; i < func->consts.size() ; ++i )
			{
				MnValue& val = func->consts[i];
				if ( MnIsNil(val) ) return (i+1);
			}
			func->consts.push_back( MnValue() );
		}
		return func->consts.size();
	}
	else if ( tok == eTString )
	{
		for ( MnIndex i = 0 ; i < func->consts.size() ; ++i )
		{
			MnValue& val = func->consts[i];
			if ( MnIsString(val) && MnToString(val)->get_str() == str ) return (i+1);
		}
		func->consts.push_back( MnValue( MOT_STRING, ut_newstring(cs->state,str) ) );
		return func->consts.size();
	}
	else if ( tok == eTNumber )
	{
		for ( MnIndex i = 0 ; i < func->consts.size() ; ++i )
		{
			MnValue& val = func->consts[i];
			if ( MnIsNumber(val) && MnToNumber(val) == num ) return (i+1);
		}
		func->consts.push_back( MnValue( (MnNumber)num ) );
		return func->consts.size();
	}
	else if ( (OvChar)tok == ':' )
	{
		do tok = cp_token(cs,num,str); while ( isspace((OvChar)tok) );
		if ( str == "func" )
		{
			MnMFunction* proto = ut_newfunction(cs->state);
			cp_build_func(cs,proto);
			func->consts.push_back( MnValue( MOT_FUNCPROTO, proto ) );
			return func->consts.size();
		}
	}
	return 0;
}

void cp_build_func( MnCompileState* cs, MnMFunction* func )
{
	MnInstruction i;
	MnOpCodeABC* code = (MnOpCodeABC*)&i;
	while ( cp_operate( cs, (MnOperate&)code->op ) )
	{
		switch ( code->op )
		{
		case MOP_SET_STACK:
		case MOP_GET_STACK:
		case MOP_REMOVE:
		case MOP_INSERT:
		case MOP_SET_FIELD:
		case MOP_GET_FIELD:
		case MOP_SET_META:
		case MOP_GET_META:
		case MOP_SET_UPVAL:
		case MOP_GET_UPVAL:
		case MOP_POP:
		case MOP_JMP:
		case MOP_RET:
			code->bx = cp_operand(cs);
			break;
		case MOP_PUSH:
		case MOP_LOG:
		case MOP_NEWCLOSURE:
			code->bx= cp_func_const( cs, func );
			break;
		case MOP_SWAP:
		case MOP_REPLACE:
		case MOP_CALL:
			code->a = cp_operand(cs);
			code->b = cp_operand(cs);
			break;
		case MOP_NONEOP:
			func->codes.push_back( (MnInstruction)i );
			return;
		}
		func->codes.push_back( (MnInstruction)i );
	}
	func->codes.push_back( (MnInstruction)i );
}
