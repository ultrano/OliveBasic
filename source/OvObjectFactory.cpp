#include "OvObjectFactory.h"
#include "OvObject.h"
#include "OvBuffer.h"
#include <map>
using namespace std;

struct OvObjectFactory
{
	map<OvString, construct_function > factory_table;
};

OvObjectFactory& GetFactory()
{
	static OvObjectFactory static_factory;
	return static_factory;
}

OvBool OvIsObjectCreatable( const OvString& type_name )
{
	map<OvString, construct_function >::iterator itor = GetFactory().factory_table.find( type_name );
	return ( itor != GetFactory().factory_table.end() );
}

OvObject* OvCreateObject_Ptr( const OvString& type_name )
{
	construct_function func = GetFactory().factory_table[ type_name ];
	return ( func )? ( func() ) : ( 0 );
}

OvObjectSPtr OvCreateObject( const OvString& type_name )
{
	construct_function func = GetFactory().factory_table[ type_name ];
	return ( func )? ( func() ) : ( 0 );
}

OvObjectSPtr OvCopyObject( OvObjectSPtr origin )
{
	if ( origin )
	{
		OvBufferSPtr origin_buf = OvBuffer::CreateBuffer();

		OvBufferOutputStream bos( origin_buf );
		OvObjectOutputStream oos( &bos );
		oos.Serialize( origin );

		OvBufferInputStream bis( origin_buf );
		OvObjectInputStream ois( &bis );
		return ois.Deserialize();
	}
	return NULL;
}

void OvRegisterConstructFunc( const OvChar* type_name, construct_function func )
{
	GetFactory().factory_table[ OvString( type_name ) ] = func;
}
OvFactoryMemberDeclarer::OvFactoryMemberDeclarer( const OvChar* type_name, construct_function func )
{
	OvRegisterConstructFunc( type_name, func );
}