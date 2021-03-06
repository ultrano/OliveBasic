#pragma once
#include "OvTypeDef.h"

template<typename T>
template_class OvSingleton
{
public:
	static OvBool Startup()
	{
		if ( !GetInstance() )
		{
			T * instance = new T;
			return !!(instance);
		}
		return false;
	}
	static void Cleanup()
	{
		if ( T * instance = GetInstance() )
		{
			delete instance;
		}
	}
	static T* GetInstance()
	{
		return m_single_instance;
	}
protected:
	OvSingleton()
	{
		if ( m_single_instance == NULL )
		{
			void* ptr = (void*)this;
			memcpy( &m_single_instance, &ptr, sizeof(T*) );
		}
		else
		{
			// ���� ó��.
		}
	}
private:
	static T* m_single_instance;
};

template<typename T>
T* OvSingleton<T>::m_single_instance;

#define OvSINGLETON( classtype ) friend class OvSingleton<classtype>;
