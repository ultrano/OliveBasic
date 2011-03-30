// 2008 12 11
// maker : Han,sang woon
#define OvExportDll
#include "OvMemObject.h"
#include "OvMemoryMgr.h"
#include "OvUtility.h"

void*	OvMemObject::operator new(size_t, void* location)
{
	static OvCriticalSection cs;
	OvAutoSection lock(cs);
	return location;
}


#ifdef _DEBUG
void*	OvMemObject::operator new(std::size_t _size, char* _pBlock,int _iLine)
{
	static OvCriticalSection cs;
	OvAutoSection lock(cs);
	return OvMemAlloc_Debug(_pBlock,_iLine,_size);
}
void*	OvMemObject::operator new[](std::size_t _size, char* _pBlock,int _iLine)
{
	static OvCriticalSection cs;
	OvAutoSection lock(cs);
	return OvMemAlloc_Debug(_pBlock,_iLine,_size);
}
#endif

void*	OvMemObject::operator new(std::size_t _size)
{
	static OvCriticalSection cs;
	OvAutoSection lock(cs);
	return OvMemAlloc(_size);
}
void	OvMemObject::operator delete(void* _memory)
{
	static OvCriticalSection cs;
	OvAutoSection lock(cs);
	OvMemFree(_memory);
}
void*	OvMemObject::operator new[](std::size_t _size)
{
	static OvCriticalSection cs;
	OvAutoSection lock(cs);
	return OvMemAlloc(_size);
}
void	OvMemObject::operator delete[](void* _memory)
{
	static OvCriticalSection cs;
	OvAutoSection lock(cs);
	OvMemFree(_memory);
}


#ifdef _DEBUG

void*	OvMemAlloc_Debug(char* _pBlock,int _iLine,const size_t _szMemSize)
{
	static OvCriticalSection cs;
	OvAutoSection lock(cs);
	return OvMemoryMgr::GetInstance()->Alloc_Debug(_pBlock, _iLine,_szMemSize);
	//return malloc(_szMemSize);
}
void	OvMemFree_Debug(void* _pMemory)
{
	static OvCriticalSection cs;
	OvAutoSection lock(cs);
	OvMemoryMgr::GetInstance()->Free_Debug(_pMemory);
	//free(_pMemory);
}

#endif

void*	OvMemAlloc_Release(const size_t _szMemSize)
{
	static OvCriticalSection cs;
	OvAutoSection lock(cs);
	return OvMemoryMgr::GetInstance()->Alloc(_szMemSize);
}
void	OvMemFree_Release(void* _pMemory)
{
	static OvCriticalSection cs;
	OvAutoSection lock(cs);
	OvMemoryMgr::GetInstance()->Free(_pMemory);
}


