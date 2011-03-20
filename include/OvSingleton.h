#pragma once
#include "OvTypeDef.h"

template<typename T>
template_class OvSingleton
{
public:
	static T* GetInstance()
	{
		static T instance;
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
			// 에러 처리.
		}
	}
private:
	static T* m_single_instance;
};

template<typename T>
T* OvSingleton<T>::m_single_instance;

#define OvSINGLETON( classtype ) friend class OvSingleton<classtype>;
