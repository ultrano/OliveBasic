#include "OvRefable.h"
#include "OvGlobalFunc.h"

OvRTTI_IMPL_ROOT(OvRefable);

OvRefable::OvRefable() : refcnt(OvNew OvRefCounter(this))
{
	refcnt->inc_weak();
}

OvRefable::~OvRefable()
{
	refcnt->dec_weak();
	refcnt->dumping();
}