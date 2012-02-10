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

OvInt __call( MnState* s )
{
	return 1;
}

void test_do_asm_do_asm_func()
{
	MnState* s = mn_openstate();
	mn_lib_default(s);

	mn_pushstring(s,"call");
	OvInt* p = (OvInt*)mn_newminidata(s,400); *p = 12;
	mn_newtable(s);
	mn_pushstring(s,"__call");
	mn_pushfunction(s,__call);
	mn_setfield(s,-3);
	mn_setmeta(s,-2);
	mn_setglobal(s);

	mn_pushfunction(s,native1);
	mn_do_asm( s, "../OliveBasic/include/test/do_asm.txt", mn_gettop(s) );

	mn_closestate(s);
}
