#pragma once
#include <windows.h>

namespace OU
{
	namespace thread
	{
		template<typename T>
		class	OvThreadSyncer
		{
		public:
			struct OvSectionLock 
			{
				OvSectionLock() { OvThreadSyncer<T>::Enter(); }
				~OvSectionLock() { OvThreadSyncer<T>::Leave(); }
			};
			static void	Enter() { __msh_sesstion_key.Lock(); };
			static void	Leave() { __msh_sesstion_key.Unlock(); };
		private:
			static OvCriticalSection	__msh_sesstion_key;
		};

		template<typename T>
		OvCriticalSection OvThreadSyncer<T>::__msh_sesstion_key;
	}
}