#pragma once
#include "OvTypeDef.h"
#include "OvUtility_RTTI.h"
#include "OvRefObject.h"

OvSmartPointer_Class( OvOutputStream );
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
		OvSize size = data.size();
		Write( size );
		return ((sizeof(size) + size ) == WriteBytes( (OvByte*)&data[0], size ));
	};

};