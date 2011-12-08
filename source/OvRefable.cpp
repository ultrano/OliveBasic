#include "OvRefable.h"
#include "OvGlobalFunc.h"

OvRefable::OvRefable() : refcnt(OvNew OvRefCounter(this))
{
	refcnt->inc_weak();
}

OvRefable::~OvRefable()
{
	refcnt->dec_weak();
	refcnt->dumpsref();
}