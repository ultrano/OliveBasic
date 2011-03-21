#pragma once
#pragma comment(lib,"WS2_32")

#include "OvRefObject.h"
#include "OvUtility.h"
#include "OvSocketInputStream.h"
#include "OvSocketOutputStream.h"

OvREF_POINTER(OvSocket);
class OvSocket : public OvRefObject, public OvThreadSyncer< OvSocket >
{
	OvRTTI_DECL( OvSocket );
	struct Address
	{
		OvString ip;
		OvShort	 port;
	};
private:
	OvSocket();
	~OvSocket();
public:

	static OvBool Startup();
	static OvBool Cleanup();

	static OvSocketSPtr Connect( const string& ip, OvShort port );
	static OvSocketSPtr Bind( const OvString& ip, OvShort port );
	static OvSocketSPtr Accept( OvSocketSPtr listen );

public:

	OvBool GetPeerAddr( Address& addr );
	OvBool GetSockAddr( Address& addr );

	SOCKET GetSock();

	void	Close();

private:
	SOCKET m_socket;

};