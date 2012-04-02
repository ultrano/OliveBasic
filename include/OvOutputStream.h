#pragma once
#include "OvTypeDef.h"
#include "OvUtility_RTTI.h"
#include "OvRefable.h"

OvDescSPtr( class, OvOutputStream );
interface_class OvOutputStream : public OvRefable
{
	OvRTTI_DECL_ROOT(OvOutputStream);

	//! override method
	virtual OvSize WriteBytes( OvByte * write_buf, OvSize write_size ) = 0;
	//

	template<typename T>
	OvBool	Write( const T& data )
	{
		return (sizeof(T) == WriteBytes( (OvByte*)&data, sizeof(T) ));
	};
	OvBool Write( const OvChar* data){ return Write( OvString(data) ); };
	OvBool Write( const OvString& data )
	{
		OvSize size = data.size();
		Write( size );
		return ((sizeof(size) + size ) == WriteBytes( (OvByte*)&data[0], size ));
	};
	template<typename T>
	OvOutputStream& operator << ( const T& data ) { Write( data ); return *this; };
};