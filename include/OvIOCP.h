#pragma once
#include "OvAutoPtr.h"
#include "OvSocket.h"
#include "OvByteInputStream.h"
#include "OvByteOutputStream.h"

class OvIOCPCallback : OvRefObject
{
public:
	struct CallbackObject : OvMemObject
	{
		OvSocketSPtr		sock;
		OvByteInputStream	input;
		OvByteOutputStream	output;
		void *				user_data;
	};
	virtual void OnConnect( CallbackObject * obj ) = 0;
	virtual void OnDisconnect( CallbackObject * obj ) = 0;

	virtual void OnRecv( CallbackObject * obj ) = 0;
	virtual void OnSend( CallbackObject * obj ) = 0;
};

class OvIOCP : OvRefObject
{
public:

	OvIOCP();
	~OvIOCP();
	struct IOCPObject;

	void Startup( const OvString & ip, OvShort port, OvIOCPCallback * callback );
	void Cleanup();

private:

	void OnConnect( IOCPObject * obj );
	void OnDisconnect( IOCPObject * obj );

	void OnRecv( IOCPObject * obj );
	void OnSend( IOCPObject * obj );


	static void _accept_thread( void * );
	void _accept();

	static void _worker_thread( void * );
	void _worker();

private:

	HANDLE m_iocp;
	OvMap<OvSocket*,OvAutoPtr<IOCPObject>> m_iocp_table;

	OvSocketSPtr m_listener;

	OvIOCPCallback * m_callback;

};