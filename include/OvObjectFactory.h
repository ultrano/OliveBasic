#pragma once
#include "OvMemObject.h"
#include "OvRefObject.h"
#include "OvObjectID.h"
using namespace std;
OvSmartPointer_Class(OvObject);


OvBool			OvIsObjectCreatable( const OvString& type_name );
OvObject*		OvCreateObject_Ptr( const OvString& type_name );
OvObjectSPtr	OvCreateObject( const OvString& type_name );
OvObjectSPtr	OvCopyObject( OvObjectSPtr origin );

struct factory{};
typedef OvObject* (*construct_function)(void);
void			OvRegisterConstructFunc( const OvChar* type_name, construct_function func );
struct OvFactoryMemberDeclarer
{
	OvFactoryMemberDeclarer( const OvChar* type_name, construct_function func );
};

#define OvFACTORY_OBJECT_DECL( type_name ) \
	friend OvObject* _construct_function_##type_name();\
private: static OvObject* _construct_function_(){ return OvNew type_name(factory()); };\
	private: static OvFactoryMemberDeclarer _static_factory_declarer;

#define OvFACTORY_OBJECT_IMPL( type_name ) \
	OvObject* _construct_function_##type_name(){ return type_name::_construct_function_(); }\
	OvFactoryMemberDeclarer type_name::_static_factory_declarer( #type_name, _construct_function_##type_name);
