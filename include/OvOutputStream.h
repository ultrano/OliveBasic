#pragma once
#include "OvTypeDef.h"
#include "OvUtility_RTTI.h"
#include "OvRefObject.h"

OvREF_POINTER( OvOutputStream );
interface_class OvOutputStream : public OvRefObject
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
		OvVector<OvByte> strbuf;
		OvSize size = data.size();
		strbuf.resize(sizeof(size) + size );
		memcpy( &strbuf[ 0 ], &size, sizeof(size));
		memcpy( &strbuf[sizeof(size)], &data[0], size);
		return ((sizeof(size) + size ) == WriteBytes( &strbuf[0], strbuf.size() ));
	};

};