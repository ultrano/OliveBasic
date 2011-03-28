#pragma once
#include "OvAutoPtr.h"
#include "OliveNet.h"
#include "OvByteInputStream.h"
#include "OvBufferOutputStream.h"

class OvIOCPCallback;
class OvIOCPObject;
struct SCompletePort;

struct SOverlapped;
class OvIOCP : OvRefObject
{
public:

	OvIOCP();
	~OvIOCP();

	OvBool	Startup( const OvString & ip, OvShort port, OvIOCPCallback * callback );
	void	Cleanup();

	HANDLE  GetIOCPHandle();

private:

	static void _worker( void * p );

private:

	HANDLE m_iocp_handle;
	SOCKET m_listensock;
	OvSet<SOverlapped*> m_clients;

	// 상활별 대기 이벤트
	HANDLE m_startup_complete;
	HANDLE m_on_cleaningup;
	OvBool m_terminate;

	OvVector<HANDLE> m_threads;
	//

};