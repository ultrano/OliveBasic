#pragma once
#include "OvTypeDef.h"
#include "OvMemObject.h"

template<typename T>
template_class OvMTSet : public OvMemObject
{
public:
	typedef T			value_type;
	typedef OvList<T>	value_list;
public:

	void		insert( const value_type& val )
	{
		OvAutoSection lock(m_cs);
		m_set.insert( val );
	}

	void		erase( const value_type& val )
	{
		OvAutoSection lock(m_cs);
		m_set.erase( val );
	}

	OvBool		is_contained( const value_type& val )
	{
		OvAutoSection lock(m_cs);
		OvBool ret = (m_set.end() != m_set.find( val ));
		return ret;
	}

	void		copy_value_list( value_list& itor )
	{
		OvAutoSection lock(m_cs);
		itor.clear();
		for each ( const value_type& val in m_set )
		{
			itor.push_back( val );
		}
	}

private:

	OvSet<T>	m_set;
	OvCriticalSection m_cs;

};