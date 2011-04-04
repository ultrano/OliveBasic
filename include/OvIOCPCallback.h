#pragma once
#include <windows.h>
#include "OvTypeDef.h"
#include "OvMemObject.h"

class OvIOCPObject;
class OvIOCPCallback : public OvMemObject
{
public:
	virtual void OnConnected( OvIOCPObject * iobj ) = 0;
	virtual void OnDisconnected( OvIOCPObject * iobj ) = 0;
	virtual void OnRecved( OvIOCPObject * iobj, OvSize completed_byte ) = 0;
	virtual void OnSended( OvIOCPObject * iobj, OvSize completed_byte ) = 0;
	virtual void OnErrOccured( OvIOCPObject * iobj, OvInt err_code ) = 0;
};
