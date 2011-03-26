#pragma once
#include "OvAutoPtr.h"
#include "OvSocket.h"
#include "OvByteInputStream.h"
#include "OvBufferOutputStream.h"

class OvIOCPCallback;
class OvIOCPObject;
struct SCompletePort;

class OvIOCP : OvRefObject
{
public:

	OvIOCP();
	~OvIOCP();

	OvBool	Startup( const OvString & ip, OvShort port, OvIOCPCallback * callback );
	void	Cleanup();

private:

	void _on_connect( OvSocketSPtr sock );
	void _on_disconnect( OvIOCPObject * obj );

	static void _accept_thread( void * );
	void _accept();

	static void _worker_thread( void * );
	void _worker();

	void	_set_shutdown( OvBool b );
	OvBool	_is_shutdown();

private:

	HANDLE					m_iocp;
	OvVector<HANDLE>		m_threads;
	OvSet<OvIOCPObject*>	m_iocp_objects;
	OvSocketSPtr			m_listener;
	OvIOCPCallback *		m_callback;
	OvCriticalSection		m_cs;

	OvCriticalSection		m_shutdown_cs;
	volatile OvBool			m_shutdown;

};