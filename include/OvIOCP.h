#pragma once
#include "OvAutoPtr.h"
#include "OvSocket.h"

class OvIOCPCallback : OvRefObject
{
public:
	struct CallbackObject : OvMemObject
	{
		OvSocketSPtr sock;
		void *		 user_data;
	};
	virtual void OnConnect( CallbackObject * obj );
	virtual void OnDisconnect( CallbackObject * obj );

	virtual void OnRecv( CallbackObject * obj );
	virtual void OnSend( CallbackObject * obj );
};

class OvIOCP : OvRefObject
{
public:

	OvIOCP();
	~OvIOCP();
	struct IOCPObject;

	void Begin();
	void End();

	void OnConnect( IOCPObject * obj );
	void OnDisconnect( IOCPObject * obj );

	void OnRecv( IOCPObject * obj );
	void OnSend( IOCPObject * obj );

private:

	static void _accept( void * );
	static void _worker( void * );

private:

	HANDLE m_iocp;
	OvMap<OvSocket*,OvAutoPtr<IOCPObject>> m_iocp_table;

	OvSocketSPtr m_listener;

	OvIOCPCallback * m_callback;

};