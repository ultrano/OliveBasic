#pragma once
#include <windows.h>

namespace OU
{
	namespace thread
	{
		class OvAutoSection
		{
		private:
			OvAutoSection();
		public:
			OvAutoSection( OvCriticalSection& critical_section )
				: m_critical_section( critical_section.m_section )
			{
				::EnterCriticalSection( &m_critical_section);
			}
			OvAutoSection( CRITICAL_SECTION& critical_section )
				: m_critical_section( critical_section )
			{
				::EnterCriticalSection( &m_critical_section);
			}
			~OvAutoSection()
			{
				::LeaveCriticalSection( &m_critical_section);
			}
		private:
			CRITICAL_SECTION&	m_critical_section;
		};
	}
};