#include "OvRefable.h"
#include "OvGlobalFunc.h"
#include "OvRefCounter.h"

#define REF_LIFE_CHECK() if ( ref && (scnt == 0)) { OvRefable * temp(ref); ref = NULL; OvDelete temp; }
#define CNT_LIFE_CHECK() if ( wcnt == 0 && scnt == 0 ) OvDelete this;

OvRefCounter::OvRefCounter( OvRefable *o )
: scnt(0), wcnt(0), ref(o)
{

}

OvRefCounter::~OvRefCounter()
{

}

void OvRefCounter::inc_strong()
{
	OvAssert( scnt >= 0 && ref );
	if ( scnt >= 0 && ref ) ++scnt;
}

void OvRefCounter::dec_strong()
{
	OvAssert( scnt > 0 );
	if ( scnt > 0 ) --scnt;

	REF_LIFE_CHECK();
	CNT_LIFE_CHECK();
}

void OvRefCounter::inc_weak()
{
	++wcnt;
}

void OvRefCounter::dec_weak()
{
	OvAssert( wcnt > 0 );
	if ( wcnt > 0 ) --wcnt;
	CNT_LIFE_CHECK();
}

void OvRefCounter::inc()
{
	inc_weak();
	inc_strong();
}

void OvRefCounter::dec()
{
	dec_strong();
	dec_weak();
}

void OvRefCounter::dumping()
{
	scnt = 0;
	ref	 = NULL;
	CNT_LIFE_CHECK();
}
