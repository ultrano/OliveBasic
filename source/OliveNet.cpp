#include <WinSock2.h>
#include <MSWSock.h>
#include "OliveNet.h"

// 
// OvBool OvSocket::GetPeerAddr( Address& addr )
// {
// 	OvSectionLock lock;
// 	if ( m_socket != INVALID_SOCKET )
// 	{
// 		SOCKADDR_IN addr_in;
// 		int sz = sizeof( addr_in );
// 		ZeroMemory(&addr_in,sz);
// 		getpeername( m_socket, (SOCKADDR*)&addr_in, &sz );
// 		addr.ip = inet_ntoa( addr_in.sin_addr );
// 		addr.port = ntohs( addr_in.sin_port );
// 		return true;
// 	}
// 	return false;
// }
// 
// OvBool OvSocket::GetSockAddr( Address& addr )
// {
// 	OvSectionLock lock;
// 	if ( m_socket != INVALID_SOCKET )
// 	{
// 		SOCKADDR_IN addr_in;
// 		int sz = sizeof( addr_in );
// 		ZeroMemory(&addr_in,sz);
// 		getsockname( m_socket, (SOCKADDR*)&addr_in, &sz );
// 		addr.ip = inet_ntoa( addr_in.sin_addr );
// 		addr.port = ntohs( addr_in.sin_port );
// 		return true;
// 	}
// 	return false;
// }


OvBool OliveNet::Startup()
{
	WSADATA wsa;
	return ( WSAStartup( MAKEWORD(2,2), &wsa ) == 0 );
}

OvBool OliveNet::Cleanup()
{
	return ( WSACleanup() == 0 );
}

SOCKET OliveNet::Connect( const string& ip, OvShort port )
{
	SOCKET sock = socket( AF_INET, SOCK_STREAM, 0 );
	if ( INVALID_SOCKET != sock )
	{
		SOCKADDR_IN addr_in;
		ZeroMemory(&addr_in,sizeof(addr_in));
		addr_in.sin_family = AF_INET;
		addr_in.sin_port = htons( port );
		addr_in.sin_addr.s_addr = inet_addr( ip.c_str() );

		int err = 0;
		if ( SOCKET_ERROR == connect( sock, (SOCKADDR*)&addr_in, sizeof(addr_in) ) )
		{
			err = WSAGetLastError();
		}
	}
	return sock;
}

SOCKET OliveNet::Bind( const OvString& ip, OvShort port )
{
	SOCKET sock = socket( AF_INET, SOCK_STREAM, 0 );
	int opt = true;
	setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt) );

	OvUInt addr = ( ip == "*" )? htonl(INADDR_ANY) : inet_addr( ip.c_str() );
	SOCKADDR_IN addr_in;
	ZeroMemory(&addr_in,sizeof(addr_in));
	addr_in.sin_port = htons( port );
	addr_in.sin_addr.s_addr = addr;
	addr_in.sin_family = AF_INET;

	int ret = 0;
	if ( SOCKET_ERROR == bind( sock, (SOCKADDR*)&addr_in, sizeof(addr_in) ) )
	{
		ret = WSAGetLastError();
	}
	if ( SOCKET_ERROR == listen( sock, SOMAXCONN ) )
	{
		ret = WSAGetLastError();
	}

	return sock;
}

SOCKET OliveNet::Accept( SOCKET listen )
{
	SOCKADDR_IN addr_in;
	OvInt addr_size = sizeof(addr_in);
	ZeroMemory(&addr_in,addr_size);
	return accept( listen, (SOCKADDR*)&addr_in, &addr_size );
}

void OliveNet::CloseSock( SOCKET sock )
{
	closesocket( sock );
}

OliveNet::SSockAddress OliveNet::GetSockAddress( SOCKET sock )
{
	SSockAddress addr;
	if ( sock != INVALID_SOCKET )
	{
		SOCKADDR_IN addr_in;
		int sz = sizeof( addr_in );
		ZeroMemory(&addr_in,sz);
		getsockname( sock, (SOCKADDR*)&addr_in, &sz );
		addr.ip = inet_ntoa( addr_in.sin_addr );
		addr.port = ntohs( addr_in.sin_port );
	}
	return addr;
}

OliveNet::SSockAddress OliveNet::GetPeerAddress( SOCKET sock )
{
	SSockAddress addr;
	if ( sock != INVALID_SOCKET )
	{
		SOCKADDR_IN addr_in;
		int sz = sizeof( addr_in );
		ZeroMemory(&addr_in,sz);
		getpeername( sock, (SOCKADDR*)&addr_in, &sz );
		addr.ip = inet_ntoa( addr_in.sin_addr );
		addr.port = ntohs( addr_in.sin_port );
	}
	return addr;
}
