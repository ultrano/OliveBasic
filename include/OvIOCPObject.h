#pragma once
#include "OvMemObject.h"
#include "OvUtility.h"

struct SOverlapped;

class OvIOCPObject : public OvMemObject
{
	friend class OvIOCP;
	friend class OvIOCPOutputStream;
	friend class OvIOCPInputStream;
public:
	OvIOCPObject( OvIOCP * _iocp, SOCKET s );
	~OvIOCPObject();

	void		SetUserData( void * user );
	void *		GetUserData();
	SOCKET		GetSock();
private:

	OvCriticalSection	cs;
	SOverlapped	*		connected;
	SOverlapped	*		recved;
	SOverlapped	*		sended;
	SOverlapped	*		erroccured;

	OvIOCP *			iocp;
	SOCKET				sock;
	void*				user_data;
};