#pragma once

#include "MnScript.h"
#include "OvObject.h"

//////////////////////////////////////////////////////////////////////////

OvInt	native1(MnState* s)
{
	OvString a1 = mn_tostring(s,1);
	OvReal a2 = mn_tonumber(s,2);

	mn_getupval(s,1);
	OvString u1 = mn_tostring(s,-1);

	mn_pushstring(s,"return1");
	return 1;
}

OvInt __add(MnState*s)
{

	return 1;
}

void test_do_asm_userdata()
{
	MnState* s = mn_openstate();
	mn_lib_default(s);

	mn_pushstring(s,"user");
	mn_pushuserdata( s, NULL );
	mn_newtable(s);
	mn_pushstring(s,"__add");
	mn_pushfunction( s, __add );
	mn_setfield(s,-3);
	mn_setmeta(s,-2);
	mn_setglobal(s);

	mn_pushfunction(s,native1);
	mn_do_asm( s, "../OliveBasic/include/test/userdata.txt", mn_gettop(s) );

	mn_closestate(s);
};

//////////////////////////////////////////////////////////////////////////

void test_do_asm_dumpstack()
{
	MnState* s = mn_openstate();
	mn_lib_default(s);

	mn_pushfunction(s,native1);
	mn_do_asm( s, "../OliveBasic/include/test/dumpstack.txt", mn_gettop(s) );

	mn_closestate(s);
}

//////////////////////////////////////////////////////////////////////////

#include "OvVector2.h"
typedef OvFloat OvVector2::* TT;


OvInt ex_vector2_constructor( MnState* s )
{
	mn_newminidata( s, sizeof(OvVector2) );
	mn_pushstring(s,"vector2");
	mn_getglobal(s);
	mn_setmeta(s,-2);
	return 1;
}

OvInt us_vector2_newindex( MnState* s )
{
	mn_getmeta(s,1);
	mn_getstack(s,2);
	mn_getfield(s,-2);
	OvVector2* v = (OvVector2*) mn_tominidata(s,1);
	TT* m = (TT*)mn_tominidata(s,-1);
	v->**m = mn_tonumber(s,3);
	mn_pop(s,2);
	return 0;
}

OvInt us_vector2_index( MnState* s )
{
	mn_getmeta(s,1);
	mn_getstack(s,2);
	mn_getfield(s,-2);
	OvVector2* v = (OvVector2*) mn_tominidata(s,1);
	TT* m = (TT*)mn_tominidata(s,-1);
	mn_pop(s,2);
	mn_pushnumber(s,v->**m);
	return 1;
}

void us_push_vector2meta( MnState* s )
{
	mn_newtable(s);

	mn_pushstring(s,"__newindex");
	mn_pushfunction(s,us_vector2_newindex);
	mn_setfield(s,-3);

	mn_pushstring(s,"__index");
	mn_pushfunction(s,us_vector2_index);
	mn_setfield(s,-3);

	mn_pushstring(s,"x");
	new(mn_newminidata(s,sizeof(TT))) TT( &OvVector2::x );
	mn_setfield(s,-3);

	mn_pushstring(s,"y");
	new(mn_newminidata(s,sizeof(TT))) TT( &OvVector2::y );
	mn_setfield(s,-3);
}


void test_do_asm_do_asm_func()
{
	MnState* s = mn_openstate();
	mn_lib_default(s);

	mn_pushstring(s,"vector2");

	us_push_vector2meta(s);
	mn_newtable(s);
	mn_pushstring(s,"__call");
	mn_pushfunction(s,ex_vector2_constructor);
	mn_setfield(s,-3);
	mn_setmeta(s,-2);

	mn_setglobal(s);

	mn_do_asm(s,"../OliveBasic/include/test/do_asm.txt",0);

	mn_pushstring(s,"vec");
	mn_getglobal(s);
	OvVector2* vec = (OvVector2*)mn_tominidata(s,-1);

	mn_closestate(s);
}
