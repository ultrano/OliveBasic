#pragma once
#include "OvUtility.h"
#include "OvMemObject.h"
#include "OvTypeDef.h"
#include "OvRefCounter.h"

#define OvDescWRef(type,name) type name; typedef OvWRef<name> name##WRef;
#define OvDescSPtr(type,name) type name; typedef OvSPtr<name> name##SPtr;OvDescWRef(type,name);

class OvRefable : public OvMemObject
{
public:
	OvRTTI_DECL_ROOT(OvRefable);
	OvRefable();;
	virtual ~OvRefable();;
	OvRefCounter  * refcnt;
};

//////////////////////////////////////////////////////////////////////////

template<typename T> class OvWRef;
template<typename T> class OvSPtr;

template<typename T>
class OvWRef : public OvMemObject
{
public:
	typedef T reftype;
	OvRefCounter * refcnt;

	OvWRef() : refcnt(NULL) {};
	OvWRef( const T * ref ) : refcnt( ref? ref->refcnt:NULL ) { if (refcnt) refcnt->inc_weak(); }
	OvWRef( const OvWRef& ref ) : refcnt( ref? ref.refcnt:NULL ) { if (refcnt) refcnt->inc_weak(); }
	template<typename ST> OvWRef( const OvWRef<ST> &ref );
	template<typename ST> OvWRef( const OvSPtr<ST> &ref );

	~OvWRef() { if (refcnt) refcnt->dec_weak(); refcnt = NULL; };

	const OvWRef & operator = ( const T * ref );
	const OvWRef & operator = ( const OvWRef& ref );
	template<typename ST> const OvWRef & operator = ( const OvWRef<ST> & ref );
	template<typename ST> const OvWRef & operator = ( const OvSPtr<ST> & ref );

	reftype *	operator ->() const;

	reftype* get_real() const { return (reftype*) (refcnt? refcnt->getref():NULL); };
	operator OvBool() const { return !!get_real(); };
};

//////////////////////////////////////////////////////////////////////////

template<typename T>
class OvSPtr : public OvMemObject
{
public:
	typedef T reftype;
	OvRefCounter * refcnt;

	OvSPtr() : refcnt(NULL) {};
	OvSPtr( const T * ref ) : refcnt( ref? ref->refcnt:NULL ) { if (refcnt) refcnt->inc(); }
	OvSPtr( const OvSPtr& ref ) : refcnt( ref? ref.refcnt:NULL ) { if (refcnt) refcnt->inc(); }
	template<typename ST> OvSPtr( const OvWRef<ST> & ref );
	template<typename ST> OvSPtr( const OvSPtr<ST> & ref );

	~OvSPtr() { if (refcnt) refcnt->dec(); refcnt = NULL; }

	const OvSPtr & operator = ( const T * ref );
	const OvSPtr & operator = ( const OvSPtr& ref );
	template<typename ST> const OvSPtr & operator = ( const OvWRef<ST> & ref );
	template<typename ST> const OvSPtr & operator = ( const OvSPtr<ST> & ref );

	T*	operator ->() const;

	reftype* get_real() const { return (reftype*)refcnt->getref(); };
	operator OvBool() const { return !!get_real(); };

};

//////////////////////////////////////////////////////////////////////////

template<typename T>
template<typename ST>
OvWRef<T>::OvWRef( const OvWRef<ST> &ref ) : refcnt( ref? ref.refcnt:NULL )
{
	if (refcnt) refcnt->inc_weak();
}

template<typename T>
template<typename ST>
OvWRef<T>::OvWRef( const OvSPtr<ST> &ref ) : refcnt( ref? ref.refcnt:NULL )
{
	if (refcnt) refcnt->inc_weak();
}

template<typename T>
const OvWRef<T> & OvWRef<T>::operator=( const T * ref )
{
	if (refcnt) refcnt->dec_weak();
	refcnt = ref? ref->refcnt:NULL;
	if (refcnt) refcnt->inc_weak();
	return *this;
}

template<typename T>
const OvWRef<T> & OvWRef<T>::operator=( const OvWRef& ref )
{
	if (refcnt) refcnt->dec_weak();
	refcnt = ref? ref.refcnt:NULL;
	if (refcnt) refcnt->inc_weak();
	return *this;
}

template<typename T>
template<typename ST>
const OvWRef<T> & OvWRef<T>::operator=( const OvWRef<ST> & ref )
{
	if (refcnt) refcnt->dec_weak();
	refcnt = ref? ref.refcnt:NULL;
	if (refcnt) refcnt->inc_weak();
	return *this;
}

template<typename T>
template<typename ST>
const OvWRef<T> & OvWRef<T>::operator=( const OvSPtr<ST> & ref )
{
	if (refcnt) refcnt->dec_weak();
	refcnt = ref? ref.refcnt:NULL;
	if (refcnt) refcnt->inc_weak();
	return *this;
}

template<typename T>
T* OvWRef<T>::operator->() const
{
	OvAssert( (refcnt? refcnt->getref() : NULL) );
	return (reftype *) (refcnt? refcnt->getref() : NULL);
}

template<typename T, typename ST>
OvBool operator == ( const T *t1, const OvWRef<ST> &t2 )
{
	return t1 == t2.get_real();
}

template<typename T, typename ST>
OvBool operator == ( const OvWRef<ST> &t1, const T *t2 )
{
	return t2.get_real() == t1;
}

template<typename T, typename ST>
OvBool operator != ( const T *t1, const OvWRef<ST> &t2 )
{
	return t1 != t2.get_real();
}

template<typename T, typename ST>
OvBool operator != ( const OvWRef<ST> &t1, const T *t2 )
{
	return t2.get_real() != t1;
}

//////////////////////////////////////////////////////////////////////////

template<typename T>
template<typename ST>
OvSPtr<T>::OvSPtr( const OvWRef<ST> & ref ) : refcnt( ref? ref.refcnt:NULL )
{
	if (refcnt) refcnt->inc();
}

template<typename T>
template<typename ST>
OvSPtr<T>::OvSPtr( const OvSPtr<ST> & ref ) : refcnt( ref? ref.refcnt:NULL )
{
	if (refcnt) refcnt->inc();
}

template<typename T>
const OvSPtr<T> & OvSPtr<T>::operator=( const T * ref )
{
	if (refcnt) refcnt->dec();
	refcnt = ref? ref->refcnt:NULL;
	if (refcnt) refcnt->inc();
	return *this;
}

template<typename T>
const OvSPtr<T> & OvSPtr<T>::operator=( const OvSPtr& ref )
{
	if (refcnt) refcnt->dec();
	refcnt = ref? ref.refcnt:NULL;
	if (refcnt) refcnt->inc();
	return *this;
}

template<typename T>
template<typename ST>
const OvSPtr<T> & OvSPtr<T>::operator=( const OvWRef<ST> & ref )
{
	if (refcnt) refcnt->dec();
	refcnt = ref? ref.refcnt:NULL;
	if (refcnt) refcnt->inc();
	return *this;
}

template<typename T>
template<typename ST>
const OvSPtr<T> & OvSPtr<T>::operator=( const OvSPtr<ST> & ref )
{
	if (refcnt) refcnt->dec();
	refcnt = ref? ref.refcnt:NULL;
	if (refcnt) refcnt->inc();
	return *this;
}

template<typename T>
T*	OvSPtr<T>::operator->() const
{
	OvAssert( (refcnt? refcnt->getref() : NULL) );
	return (reftype *) (refcnt? refcnt->getref() : NULL);
}


template<typename T, typename ST>
OvBool operator == ( const T *t1, const OvSPtr<ST> &t2 )
{
	return t1 == t2.get_real();
}

template<typename T, typename ST>
OvBool operator == ( const OvSPtr<ST> &t1, const T *t2 )
{
	return t2.get_real() == t1;
}

template<typename T, typename ST>
OvBool operator != ( const T *t1, const OvSPtr<ST> &t2 )
{
	return t1 != t2.get_real();
}

template<typename T, typename ST>
OvBool operator != ( const OvSPtr<ST> &t1, const T *t2 )
{
	return t2.get_real() != t1;
}