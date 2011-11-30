#include "OvUtility_dataconvert.h"
#include "OvUtility_string.h"
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/// Ovint
template<> 
OvInt			OvFromString( const OvString& str )
{
	OvInt ret;
	sscanf_s( str.c_str(), "%d", &ret );
	return ret;
}

template<> 
OvString		OvToString( const OvInt& val )
{
	return OvString( OU::string::format( "%d", val ) );
}

/// OvUInt
template<> 
OvUInt			OvFromString( const OvString& str )
{
	OvUInt ret;
	sscanf_s( str.c_str(), "%d", &ret );
	return ret;
}

template<> 
OvString		OvToString( const OvUInt& val )
{
	return OvString( OU::string::format( "%d", val ) );
}

/// OvBool
template<>
OvBool			OvFromString( const OvString& str )
{
	if ( _stricmp( "0",str.c_str() ) == 0 || _stricmp( "false",str.c_str() ) == 0 )
	{
		return false;
	}
	else if ( _stricmp( "1",str.c_str() ) == 0 || _stricmp( "true",str.c_str() ) == 0 )
	{
		return true;
	}
	return false;
}

template<>
OvString		OvToString( const OvBool& val )
{
	return OvString( OU::string::format( "%d", val ) );
}

/// OvFloat
//////////////////////////////////////////////////////////////////////////
template<>
OvFloat			OvFromString( const OvString& str )
{
	OvFloat ret;
	sscanf_s( str.c_str(), "%f", &ret );
	return ret;
}

template<>
OvString			OvToString( const OvFloat& val )
{
	return OvString( OU::string::format( "%f", val ) );
}

/// OvReal
//////////////////////////////////////////////////////////////////////////
template<>
OvReal			OvFromString( const OvString& str )
{
	OvFloat ret;
	sscanf_s( str.c_str(), "%f", &ret );
	return ret;
}

template<>
OvString			OvToString( const OvReal& val )
{
	return OvString( OU::string::format( "%f", val ) );
}

/// OvVector2
//////////////////////////////////////////////////////////////////////////
template<>
OvVector2			OvFromString( const OvString& str )
{
	OvVector2 ret;
	sscanf_s( str.c_str(), "%f,%f", &ret.x, &ret.y );
	return ret;
}

template<>
OvString			OvToString( const OvVector2& val )
{
	return OvString( OU::string::format( "%f,%f", val.x, val.y ) );
}

/// OvVector3
//////////////////////////////////////////////////////////////////////////

template<>
OvVector3			OvFromString( const OvString& str )
{
	OvVector3 ret;
	sscanf_s( str.c_str(), "%f,%f,%f", &ret.x, &ret.y, &ret.z );
	return ret;
}

template<>
OvString			OvToString( const OvVector3& val )
{
	return OvString( OU::string::format( "%f,%f,%f", val.x, val.y, val.z ) );
}

/// OvQuaternion
//////////////////////////////////////////////////////////////////////////

template<>
OvQuaternion		OvFromString( const OvString& str )
{
	OvQuaternion ret;
	sscanf_s( str.c_str(), "%f,%f,%f,%f", &ret.x, &ret.y, &ret.z, &ret.w );
	return ret;
}

template<>
OvString			OvToString( const OvQuaternion& val )
{
	return OvString( OU::string::format( "%f,%f,%f,%f", val.x, val.y, val.z, val.w ) );
}

///OvObjectID
//////////////////////////////////////////////////////////////////////////

template<>
OvObjectID				OvFromString( const OvString& str )
{
	OvObjectID::id_core_type core_id;
	sscanf_s( str.c_str(), "%d", &core_id );
	return OvObjectID( core_id );
}

template<>
OvString			OvToString( const OvObjectID& val )
{
	return OvString( OU::string::format( "%d", val ) );
}

/// OvColor
//////////////////////////////////////////////////////////////////////////

template<>
OvColor				OvFromString( const OvString& expData )
{
	//!< 읽어오는 형식은 %d에 의해 숫자형으로 써져있기 때문에 숫자형인 OvInt나 OvUInt 로 받아야한다.
	//!< OvColor::a r g b 멤버로 바로 받으면 가장 마지막인 b를 받는순간 arg는 b의 4바이트읽기에 의해 덮여 쓰여지게 된다.
	//!< 그렇다고 char a,r,g,b; 로 받으면 스택 오버플로우로 함수가 무너진게 된다.
	OvColor ret;
	OvUInt a,r,g,b;
	sscanf_s( expData.c_str(), "a%dr%dg%db%d", &a, &r, &g, &b );

	return OvColor(a,r,g,b);
}

template<>
OvString			OvToString( const OvColor& val )
{
	return OvString(OU::string::format( "a%dr%dg%db%d", val.a, val.r, val.g, val.b ));
}

/// OvString
//////////////////////////////////////////////////////////////////////////

template<>
OvString				OvFromString( const OvString& expData )
{
	return OU::string::trim( expData, "\"" );
}

template<>
OvString			OvToString( const OvString& val )
{
	return "\"" + val + "\"";
}