
template<typename Type_0>
inline OvSmartPointer<Type_0>::OvSmartPointer():m_ref(NULL)
{};

template<typename Type_0>
template<typename Type_1>
inline OvSmartPointer<Type_0>::OvSmartPointer(const OvSmartPointer<Type_1>& _pointer)
:m_ref(reinterpret_cast<Type_0*>(_pointer.GetRear()))
{
	if (m_ref)
	{
		m_ref->IncRefCount();
	}
}
template<typename Type_0>
inline OvSmartPointer<Type_0>::OvSmartPointer( OvSmartPointer<Type_0>& _pointer)
{
	m_ref = _pointer.m_ref;
	if (m_ref)
	{
		m_ref->IncRefCount();
	}
};
template<typename Type_0>
inline OvSmartPointer<Type_0>::OvSmartPointer(const OvSmartPointer<Type_0>& _pointer)
{
	m_ref = _pointer.m_ref;
	if (m_ref)
	{
		m_ref->IncRefCount();
	}
};

template<typename Type_0>
inline OvSmartPointer<Type_0>::OvSmartPointer(Type_0* _pt)
{
	m_ref = _pt;
	if (m_ref)
	{
		m_ref->IncRefCount();
	}
}

template<typename Type_0>
inline OvSmartPointer<Type_0>::~OvSmartPointer()
{
	if(m_ref)
	m_ref->DecRefCount();
}
template<typename Type_0>
inline Type_0* OvSmartPointer<Type_0>::GetRear()const
{
	return (Type_0*)m_ref;
}
template<typename Type_0>
OvSmartPointer<Type_0>::operator OvInt()const
{
	return OvInt(m_ref);
}
template<typename Type_0>
inline Type_0*					OvSmartPointer<Type_0>::operator ->()const
{
	if(!m_ref)
		OvError("NULL에대한 접근 경고:OvSmartPointer이 NULL을 읽으려 합니다.");

	return m_ref;
}
template<typename Type_0>
inline OvSmartPointer<Type_0>&	OvSmartPointer<Type_0>::operator = ( Type_0* _pt)
{
	if (m_ref)
	{
		m_ref->DecRefCount();
	}

	m_ref = _pt;

	if (m_ref)
	{
		m_ref->IncRefCount();
	}

	return *this;
}

template<typename Type_0>
Type_0&							OvSmartPointer<Type_0>::operator [](OvInt _ArrayCount)
{
	return m_ref[_ArrayCount];
};
template<typename Type_0>
inline OvSmartPointer<Type_0>&	OvSmartPointer<Type_0>::operator = ( OvSmartPointer<Type_0>& _copy)
{
	if (m_ref)
	{
		m_ref->DecRefCount();
	}
	m_ref = _copy.m_ref;
	if (m_ref)
	{
		m_ref->IncRefCount();
	}
	return *this;
}
template<typename Type_0>
inline OvSmartPointer<Type_0>&	OvSmartPointer<Type_0>::operator = (const OvSmartPointer<Type_0>& _copy)
{
	if (m_ref)
	{
		m_ref->DecRefCount();
	}
	m_ref = _copy.m_ref;
	if (m_ref)
	{
		m_ref->IncRefCount();
	}
	return *this;
}
template<typename Type_0>
template<typename Type_1>
OvSmartPointer<Type_0>&		OvSmartPointer<Type_0>::operator = (const OvSmartPointer<Type_1>& _copy)
{
	if (m_ref)
	{
		m_ref->DecRefCount();
	}
	m_ref = reinterpret_cast<Type_0*>(_copy.GetRear());
	if (m_ref)
	{
		m_ref->IncRefCount();
	}
	return *this;
}

template<typename Type_0,typename Type_1>
OvBool	operator == (const OvSmartPointer<Type_0>& _copy0,const OvSmartPointer<Type_1>& _copy1)
{
	return ((void*)_copy0.GetRear() == (void*)_copy1.GetRear());
}
template<typename Type_0,typename Type_1>
OvBool	operator != (const OvSmartPointer<Type_0>& _copy0,const OvSmartPointer<Type_1>& _copy1)
{
	return ((void*)_copy0.GetRear() != (void*)_copy1.GetRear());
}

template<typename Type_0,typename Type_1>
OvBool	operator == (const OvSmartPointer<Type_0>& _copy0,const Type_1* _copy1)
{
	return ((void*)_copy0.GetRear() == (void*)_copy1);
}
template<typename Type_0,typename Type_1>
OvBool	operator != (const OvSmartPointer<Type_0>& _copy0,const Type_1* _copy1)
{
	return ((void*)_copy0.GetRear() != (void*)_copy1);
}

template<typename Type_0,typename Type_1>
OvBool	operator == (const Type_0* _copy0,const OvSmartPointer<Type_1>& _copy1)
{
	return ((void*)_copy0 == (void*)_copy1.GetRear());
}
template<typename Type_0,typename Type_1>
OvBool	operator != (const Type_0* _copy0,const OvSmartPointer<Type_1>& _copy1)
{
	return ((void*)_copy0 != (void*)_copy1.GetRear());
}


template<typename Type_0,typename Type_1>
OvBool	operator == (const OvSmartPointer<Type_0>& _copy0, Type_1* _copy1)
{
	return ((void*)_copy0.GetRear() == (void*)_copy1);
}
template<typename Type_0,typename Type_1>
OvBool	operator != (const OvSmartPointer<Type_0>& _copy0, Type_1* _copy1)
{
	return ((void*)_copy0.GetRear() != (void*)_copy1);
}

template<typename Type_0,typename Type_1>
OvBool	operator == ( Type_0* _copy0,const OvSmartPointer<Type_1>& _copy1)
{
	return ((void*)_copy0 == (void*)_copy1.GetRear());
}
template<typename Type_0,typename Type_1>
OvBool	operator != ( Type_0* _copy0,const OvSmartPointer<Type_1>& _copy1)
{
	return ((void*)_copy0 != (void*)_copy1.GetRear());
}