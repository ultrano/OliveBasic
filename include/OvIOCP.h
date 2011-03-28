#pragma once
#include "OvAutoPtr.h"
#include "OvMTSet.h"
#include "OliveNet.h"
#include "OvByteInputStream.h"
#include "OvBufferOutputStream.h"

struct SOverlapped;
struct OvIOCPObject;
namespace OU
{
	namespace iocp
	{
		SOCKET	GetSocket( OvIOCPObject * iobj );
		void*	GetUserData( OvIOCPObject * iobj );
		void	SendData( OvIOCPObject * iobj, OvByte * buf, OvSize len );
	}
}
class OvIOCPCallback : public OvMemObject
{
public:
	virtual void OnConnected( void** user, OvByte** recv_buf, OvSize & buf_size ) = 0;
	virtual void OnDisconnected( void* user ) = 0;
	virtual void OnRecved( OvIOCPObject * iobj, OvSize completed_byte ) = 0;
	virtual void OnSended( OvIOCPObject * iobj, OvSize completed_byte ) = 0;
};
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
	void 		_on_connected( OvIOCPObject * iobj );
	void 		_on_disconnected( OvIOCPObject * iobj );
	void 		_on_sended( OvIOCPObject * iobj, OvSize completed_byte );
	void 		_on_recved( OvIOCPObject * iobj, OvSize completed_byte );
private:

	HANDLE				m_iocp_handle;
	SOCKET				m_listensock;
	OvMTSet<OvIOCPObject*> m_overlaps;

	// 상활별 대기 이벤트
	HANDLE				m_startup_complete;
	HANDLE				m_on_cleaningup;
	OvBool				m_terminate;

	OvVector<HANDLE>	m_threads;

	OvCriticalSection	m_callback_cs;
	OvIOCPCallback*		m_callback;
	//

};