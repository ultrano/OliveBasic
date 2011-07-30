#include "OvScriptDataType.h"
#include "OvScriptState.h"
#include "OvActObject.h"
#include "OvComponent.h"

OvObjectSPtr OvScriptFloatType::NewInstance() { return OvNew OvFloatVal; };

void OvScriptFloatType::Method( OvScriptState& env, const OvString& method )
{
	if ( method == "_construct" )
	{
		OvFloatValSPtr arg1 = OvCastTo<OvFloatVal>(env.PopStack());
		OvStringValSPtr arg2 = OvCastTo<OvStringVal>(env.PopStack());
		env.PushStack( arg1 );
		if ( arg1 && arg2 )
		{
			arg1->val = OvFromString<OvFloat>( arg2->val );
		}
	}
	else if ( method == "_plus" )
	{
		OvFloatValSPtr arg1 = OvCastTo<OvFloatVal>(env.PopStack());
		OvFloatValSPtr arg2 = OvCastTo<OvFloatVal>(env.PopStack());
		OvFloatValSPtr ret  = NewInstance();
		env.PushStack( ret );
		if ( arg1 && arg2 )
		{
			ret->val = arg1->val + arg2->val;
		}
	}
	else if ( method == "_minus" )
	{
		OvFloatValSPtr arg1 = OvCastTo<OvFloatVal>(env.PopStack());
		OvFloatValSPtr arg2 = OvCastTo<OvFloatVal>(env.PopStack());
		OvFloatValSPtr ret  = NewInstance();
		env.PushStack( ret );
		if ( arg1 && arg2 )
		{
			ret->val = arg1->val - arg2->val;
		}
	}
}

OvObjectSPtr OvScriptVector2Type::NewInstance() { return OvNew OvVector2Val; };
OvObjectSPtr OvScriptVector3Type::NewInstance() { return OvNew OvVector3Val; };
OvObjectSPtr OvScriptStringType::NewInstance() { return OvNew OvStringVal; };
OvObjectSPtr OvScriptColorType::NewInstance() { return OvNew OvColorVal; };
OvObjectSPtr OvScriptActorType::NewInstance() 
{
	OvActObjectSPtr actor = OvNew OvActObject;
	for each ( const OvPropertyTable::value_type& val in properties )
	{
		actor->InsertProp( val.first, OvCopyObject( val.second ) );
	}
	for each ( const OvString& comp in components )
	{
		actor->InsertComponent( OvCreateObject( comp ) );
	}
	return actor;
};
