#pragma once
#include "OvAutoPtr.h"
#include "OvMTSet.h"
#include "OliveNet.h"
#include "OvByteInputStream.h"
#include "OvBufferOutputStream.h"

class OvIOCPCallback;
struct OvIOCPObject;
class OvIOCP : OvRefObject
{
public:

	OvIOCP();
	~OvIOCP();

	OvBool	Startup( const OvString & ip, OvShort port, OvIOCPCallback * callback );
	void	Cleanup();

	HANDLE  GetIOCPHandle();

private:

	static void _accepter( void * p );
	static void _worker( void * p );

private:

	HANDLE m_iocp_handle;
	SOCKET m_listensock;
	OvMTSet<OvIOCPObject*> m_overlaps;

	// 상활별 대기 이벤트
	HANDLE m_startup_complete;
	HANDLE m_on_cleaningup;
	OvBool m_terminate;

	OvVector<HANDLE> m_threads;
	//

};