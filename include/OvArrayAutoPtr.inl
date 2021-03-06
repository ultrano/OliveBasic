#include "OvAutoPtr.h"


template<typename Type_0>
OvArrayAutoPtr<Type_0>::OvArrayAutoPtr():m_pReferenceTarget(0)
{	
}

template<typename Type_0>
OvArrayAutoPtr<Type_0>::OvArrayAutoPtr(OvArrayAutoPtr& _sp)
{
	m_pReferenceTarget = _sp.m_pReferenceTarget;
	_sp.m_pReferenceTarget = NULL;
}
template<typename Type_0>
OvArrayAutoPtr<Type_0>::OvArrayAutoPtr(Type_0* _pt)
{
	m_pReferenceTarget = _pt;
}
template<typename Type_0>
OvArrayAutoPtr<Type_0>::~OvArrayAutoPtr()
{
	if (m_pReferenceTarget)
	{
		delete []m_pReferenceTarget;
	}
}

template<typename Type_0>
OvArrayAutoPtr<Type_0>::operator	OvBool()const
{
	return (m_pReferenceTarget != NULL);
}
template<typename Type_0>
Type_0*				OvArrayAutoPtr<Type_0>::GetRear()const
{
	return (Type_0*)m_pReferenceTarget;
}

template<typename Type_0>
Type_0&				OvArrayAutoPtr<Type_0>::operator *()
{
	if(!m_pReferenceTarget)
		OvError("NULL에대한 접근 경고:OvArrayAutoPtr이 NULL을 읽으려 합니다.");

	return *m_pReferenceTarget;
}
template<typename Type_0>
Type_0*				OvArrayAutoPtr<Type_0>::operator ->()
{
	if(!m_pReferenceTarget)
		OvError("NULL에대한 접근 경고:OvArrayAutoPtr이 NULL을 읽으려 합니다.");
	return m_pReferenceTarget;
}
template<typename Type_0>
OvArrayAutoPtr<Type_0>&		OvArrayAutoPtr<Type_0>::operator = (const OvArrayAutoPtr& _copy)
{
	if(m_pReferenceTarget)
		delete m_pReferenceTarget;

	m_pReferenceTarget = _copy.m_pReferenceTarget;
	((OvArrayAutoPtr&)_copy).m_pReferenceTarget = NULL;
	return *this;
}
template<typename Type_0>
OvArrayAutoPtr<Type_0>&			OvArrayAutoPtr<Type_0>::operator = ( Type_0* _pt)
{
	if(m_pReferenceTarget)
		delete m_pReferenceTarget;

	m_pReferenceTarget = _pt;
	return *this;
}
template<typename Type_0>
template<typename Tyep_Count>
Type_0&				OvArrayAutoPtr<Type_0>::operator [](Tyep_Count _ArrayCount)
{
	return m_pReferenceTarget[_ArrayCount];
};



template<typename Type_0,typename Type_1>
OvBool	operator == (const OvArrayAutoPtr<Type_0>& _copy0,const OvArrayAutoPtr<Type_1>& _copy1)
{
	return ((void*)_copy0.GetRear() == (void*)_copy1.GetRear());
}
template<typename Type_0,typename Type_1>
OvBool	operator != (const OvArrayAutoPtr<Type_0>& _copy0,const OvArrayAutoPtr<Type_1>& _copy1)
{
	return ((void*)_copy0.GetRear() != (void*)_copy1.GetRear());
}

template<typename Type_0,typename Type_1>
OvBool	operator == (const OvArrayAutoPtr<Type_0>& _copy0,const Type_1* _copy1)
{
	return ((void*)_copy0.GetRear() == (void*)_copy1);
}
template<typename Type_0,typename Type_1>
OvBool	operator != (const OvArrayAutoPtr<Type_0>& _copy0,const Type_1* _copy1)
{
	return ((void*)_copy0.GetRear() != (void*)_copy1);
}


template<typename Type_0,typename Type_1>
OvBool	operator == (const Type_0* _copy0,const OvArrayAutoPtr<Type_1>& _copy1)
{
	return ((void*)_copy0 == (void*)_copy1.GetRear());
}
template<typename Type_0,typename Type_1>
OvBool	operator != (const Type_0* _copy0,const OvArrayAutoPtr<Type_1>& _copy1)
{
	return ((void*)_copy0 != (void*)_copy1.GetRear());
}
