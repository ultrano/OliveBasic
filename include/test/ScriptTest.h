#pragma once

#include "MnScript.h"
#include "OvObject.h"

//////////////////////////////////////////////////////////////////////////

void bit_test()
{
	OvInt i = 0;
	OvInt op,a,b,c;
	i = cs_code(1,2,3,4);
	op = cs_getop(i);
	a = cs_geta(i);
	b = cs_getb(i);
	c = cs_getc(i);
}