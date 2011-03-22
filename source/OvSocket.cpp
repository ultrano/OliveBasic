#include "OvSocket.h"
OvRTTI_IMPL( OvSocket );

OvSocket::OvSocket()
: m_socket( NULL )
{

}

OvSocket::~OvSocket()
{
	Close();
}

OvSocketSPtr OvSocket::Connect( const string& ip, OvShort port )
{
	OvSocketSPtr ovsocket = NULL;
	SOCKET sock = socket( AF_INET, SOCK_STREAM, 0 );
	if ( INVALID_SOCKET != sock )
	{
		SOCKADDR_IN addr_in;
		ZeroMemory(&addr_in,sizeof(addr_in));
		addr_in.sin_family = AF_INET;
		addr_in.sin_port = htons( port );
		addr_in.sin_addr.s_addr = inet_addr( ip.c_str() );

		if ( SOCKET_ERROR != connect( sock, (SOCKADDR*)&addr_in, sizeof(addr_in) ) )
		{
			ovsocket = OvNew OvSocket;
			ovsocket->m_socket = sock;
		}
	}
	return ovsocket;
}

void OvSocket::Close()
{
	OvSectionLock lock;
	if ( m_socket )
	{
		closesocket( m_socket );
		m_socket = NULL;
	}
}

OvSocketSPtr OvSocket::Bind( const OvString& ip, OvShort port )
{
	SOCKET sock = socket( AF_INET, SOCK_STREAM, 0 );
	int opt = true;
	setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt) );

	OvUInt addr = ( ip == "*" )? htonl(INADDR_ANY) : inet_addr( ip.c_str() );
	SOCKADDR_IN addr_in;
	ZeroMemory(&addr_in,sizeof(addr_in));
	addr_in.sin_family = AF_INET;
	addr_in.sin_port = htons( port );
	addr_in.sin_addr.s_addr = addr;

	if ( SOCKET_ERROR == bind( sock, (SOCKADDR*)&addr_in, sizeof(addr_in) ) )
	{
		return NULL;
	}

	if ( SOCKET_ERROR == listen( sock, SOMAXCONN ) )
	{
		return NULL;
	}

	OvSocketSPtr ret = OvNew OvSocket;
	ret->m_socket = sock;
	return ret;
}

OvSocketSPtr OvSocket::Accept( OvSocketSPtr listen )
{
	SOCKADDR_IN addr_in;
	OvInt addr_size = sizeof(addr_in);
	ZeroMemory(&addr_in,addr_size);
	SOCKET client = accept( listen->GetSock(), (SOCKADDR*)&addr_in, &addr_size );
	if ( INVALID_SOCKET == client )
	{
		return NULL;
	}

	OvSocketSPtr sock = OvNew OvSocket;
	sock->m_socket = client;
	return sock;
}

OvBool OvSocket::Startup()
{
	WSADATA wsa;
	return ( WSAStartup( MAKEWORD(2,2), &wsa ) == 0 );

}

OvBool OvSocket::Cleanup()
{
	return ( WSACleanup() == 0 );
}

OvBool OvSocket::GetPeerAddr( Address& addr )
{
	OvSectionLock lock;
	if ( m_socket != INVALID_SOCKET )
	{
		SOCKADDR_IN addr_in;
		int sz = sizeof( addr_in );
		ZeroMemory(&addr_in,sz);
		getpeername( m_socket, (SOCKADDR*)&addr_in, &sz );
		addr.ip = inet_ntoa( addr_in.sin_addr );
		addr.port = ntohs( addr_in.sin_port );
		return true;
	}
	return false;
}

OvBool OvSocket::GetSockAddr( Address& addr )
{
	OvSectionLock lock;
	if ( m_socket != INVALID_SOCKET )
	{
		SOCKADDR_IN addr_in;
		int sz = sizeof( addr_in );
		ZeroMemory(&addr_in,sz);
		getsockname( m_socket, (SOCKADDR*)&addr_in, &sz );
		addr.ip = inet_ntoa( addr_in.sin_addr );
		addr.port = ntohs( addr_in.sin_port );
		return true;
	}
	return false;
}

SOCKET OvSocket::GetSock()
{
	OvSectionLock lock;
	return m_socket;
}