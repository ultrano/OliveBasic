#pragma once
#include "OvMemObject.h"
#include "OvTypeDef.h"
#include "OvRefCounter.h"
#include "OvGlobalFunc.h"

class OvRefable : public OvMemObject
{
public:
	OvRefable();;
	virtual ~OvRefable();;
	OvRefCounter  * refcnt;
};

//////////////////////////////////////////////////////////////////////////

template<typename T>
class OvWRef : public OvMemObject
{
public:

	typedef T reftype;

	OvWRef() : refcnt(NULL) {};
	OvWRef( const reftype * ref ) : refcnt( ref? ref->refcnt:NULL ) { if (refcnt) refcnt->inc_weak(); }
	~OvWRef() { if (refcnt) refcnt->dec_weak(); };

	reftype* get_real() const { return (reftype*) (refcnt? refcnt->getref():NULL); };

	operator OvBool() { return !!get_real(); };
	const OvWRef & operator = ( const T * ref );
	const OvWRef & operator = ( const OvWRef & ref );

	reftype *	operator ()() const 
	{
		OvAssert( (refcnt? refcnt->getref() : NULL) );
		return (reftype *) (refcnt? refcnt->getref() : NULL);
	};

private:
	OvRefCounter * refcnt;
};

//////////////////////////////////////////////////////////////////////////

template<typename T>
class OvSPtr : public OvMemObject
{
public:

	typedef T reftype;

	OvSPtr( const reftype * ref ) : refcnt( ref? ref->refcnt:NULL ) { if (refcnt) refcnt->inc(); }
	OvSPtr( const OvSPtr<reftype> & ref ) : refcnt( ref? ref->refcnt:NULL ) { if (refcnt) refcnt->inc(); }
	~OvSPtr() { if (refcnt) refcnt->dec(); }

	reftype* get_real() const { return (reftype*)refcnt->getref(); };

	operator OvBool() { return !!get_real(); };
	const OvSPtr & operator = ( const T * ref );
	const OvSPtr & operator = ( const OvSPtr & ref );

	reftype *	operator ->() const 
	{
		OvAssert( (refcnt? refcnt->getref() : NULL) );
		return (reftype *) (refcnt? refcnt->getref() : NULL);
	}

private:
	OvRefCounter * refcnt;
};

//////////////////////////////////////////////////////////////////////////


template<typename T>
const OvWRef<T> & OvWRef<T>::operator=( const T * ref )
{
	if (refcnt) refcnt->dec_weak();
	refcnt = ref? ref->refcnt:NULL;
	if (refcnt) refcnt->inc_weak();
	return *this;
}

template<typename T>
const OvWRef<T> & OvWRef<T>::operator=( const OvWRef<T> & ref )
{
	if (refcnt) refcnt->dec_weak();
	refcnt = ref? ref.refcnt:NULL;
	if (refcnt) refcnt->inc_weak();
	return *this;
}

//////////////////////////////////////////////////////////////////////////

template<typename T>
const OvSPtr<T> & OvSPtr<T>::operator=( const T * ref )
{
	if (refcnt) refcnt->dec();
	refcnt = ref? ref->refcnt:NULL;
	if (refcnt) refcnt->inc();
	return *this;
}

template<typename T>
const OvSPtr<T> & OvSPtr<T>::operator=( const OvSPtr<T> & ref )
{
	if (refcnt) refcnt->dec();
	refcnt = ref? ref.refcnt:NULL;
	if (refcnt) refcnt->inc();
	return *this;
}
