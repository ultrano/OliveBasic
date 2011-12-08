#pragma once
#include "OvMemObject.h"
#include "OvTypeDef.h"
#include "OvRefCounter.h"

class OvRefable : public OvMemObject
{
public:
	OvRefable();;
	virtual ~OvRefable();;
	OvRefCounter  * refcnt;
};

template<typename T>
class OvWRef : public OvMemObject
{
public:

	typedef T reftype;

	OvWRef() : refcnt(NULL) {};
	OvWRef( const reftype * ref ) : refcnt( ref? ref->refcnt:NULL )
	{
		if (refcnt) refcnt->inc_weak();
	}
	~OvWRef()
	{
		if (refcnt) refcnt->dec_weak();
	};

	reftype* get_real() const { return (reftype*) (refcnt? refcnt->getref():NULL); };

	operator OvBool() { return (OvBool)get_real(); };
	reftype *	operator ()() const { return (reftype *)refcnt; };
	const OvWRef<reftype> & operator = ( const reftype * ref )
	{
		if (refcnt) refcnt->dec_weak();
		refcnt = ref? ref->refcnt:NULL;
		if (refcnt) refcnt->inc_weak();
		return *this;
	}

private:
	OvRefCounter * refcnt;
};

template<typename T>
class OvSPtr : public OvMemObject
{
public:

	typedef T reftype;

	OvSPtr( const reftype * ref ) : refcnt( ref? ref->refcnt:NULL )
	{
		if (refcnt) refcnt->inc();
	}
	OvSPtr( const OvSPtr<reftype> & ref ) : refcnt( ref? ref->refcnt:NULL )
	{
		if (refcnt) refcnt->inc();
	}

	~OvSPtr()
	{
		if (refcnt) refcnt->dec();
	}

	reftype* get_real() const { return (reftype*)refcnt->getref(); };

	operator OvBool() { return (OvBool)get_real(); };

	reftype *	operator ->() const
	{
		return (reftype *) (refcnt? refcnt->getref() : NULL);
	}

private:
	OvRefCounter * refcnt;
};