#pragma once
#include <windows.h>

namespace OU
{
	namespace thread
	{
		class OvCriticalSection
		{
		public:
			OvCriticalSection()
			{
				InitializeCriticalSection( &m_section );
			}
			~OvCriticalSection()
			{
				DeleteCriticalSection( &m_section );
			}
			void Lock(){ ::EnterCriticalSection( &m_section); };
			void Unlock(){ ::LeaveCriticalSection( &m_section); };
			CRITICAL_SECTION m_section;
		};
	}
};