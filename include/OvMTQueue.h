#pragma once
#include "OvTypeDef.h"
#include "OvCriticalSection.h"
#include "OvAutoSection.h"

template<typename T>
template_class OvMTQueue : public OvMemObject
{
	typedef OvSize									size_type;
	typedef typename OvQueue<T>::value_type			value_type;
	typedef typename OvQueue<T>::reference			reference;
	typedef typename OvQueue<T>::const_reference	const_reference;
public:

	bool empty() const { OvAutoSection lock(m_cs); return m_queue.empty(); };

	size_type size() const{ OvAutoSection lock(m_cs); return m_queue.size(); };

	reference front(){ OvAutoSection lock(m_cs); return m_queue.front(); };

	const_reference front() const{ OvAutoSection lock(m_cs); return m_queue.front(); };

	reference back(){ OvAutoSection lock(m_cs); return m_queue.back(); };

	const_reference back() const{ OvAutoSection lock(m_cs); return m_queue.back(); };

	void push(const value_type& _Val){ OvAutoSection lock(m_cs);  m_queue.push( _Val ); };

	void pop(){ OvAutoSection lock(m_cs);  m_queue.pop(); };

private:

	OvQueue<T>			m_queue;
	OvCriticalSection	m_cs;

};