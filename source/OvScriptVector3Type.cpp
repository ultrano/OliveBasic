#include "OvScriptDataType.h"
#include "OvScriptState.h"
#include "OvActObject.h"
#include "OvComponent.h"

OvObjectSPtr OvScriptVector3Type::NewInstance() { return OvNew OvVector3Val; };

void OvScriptVector3Type::Method( OvScriptState& env, const OvString& method ) 
{
	if ( method == "_construct" )
	{
		OvVector3ValSPtr arg1 = OvCastTo<OvVector3Val>(env.PopStack());
		OvStringValSPtr arg2 = OvCastTo<OvStringVal>(env.PopStack());
		if ( arg1 && arg2 )
		{
			arg1->val = OvFromString<OvVector3>( arg2->val );
		}
	}
	else if ( method == "_plus" )
	{
		OvVector3ValSPtr arg1 = OvCastTo<OvVector3Val>(env.PopStack());
		OvVector3ValSPtr arg2 = OvCastTo<OvVector3Val>(env.PopStack());
		OvVector3ValSPtr ret  = NewInstance();
		env.PushStack( ret );
		if ( arg1 && arg2 )
		{
			ret->val = arg1->val + arg2->val;
		}
	}
	else if ( method == "_minus" )
	{
		OvVector3ValSPtr arg1 = OvCastTo<OvVector3Val>(env.PopStack());
		OvVector3ValSPtr arg2 = OvCastTo<OvVector3Val>(env.PopStack());
		OvVector3ValSPtr ret  = NewInstance();
		env.PushStack( ret );
		if ( arg1 && arg2 )
		{
			ret->val = arg1->val - arg2->val;
		}
	}
	else if ( method == "_div" )
	{
		OvVector3ValSPtr arg1 = OvCastTo<OvVector3Val>(env.PopStack());
		OvFloatValSPtr arg2 = OvCastTo<OvFloatVal>(env.PopStack());
		OvVector3ValSPtr ret  = NewInstance();
		env.PushStack( ret );
		if ( arg1 && arg2 )
		{
			ret->val = arg1->val / arg2->val;
		}
	}
	else if ( method == "_mul" )
	{
		OvVector3ValSPtr arg1 = OvCastTo<OvVector3Val>(env.PopStack());
		OvFloatValSPtr arg2 = OvCastTo<OvFloatVal>(env.PopStack());
		OvVector3ValSPtr ret  = NewInstance();
		env.PushStack( ret );
		if ( arg1 && arg2 )
		{
			ret->val = arg1->val * arg2->val;
		}
	}
	else if ( method == "_set" )
	{
		OvVector3ValSPtr arg1 = OvCastTo<OvVector3Val>(env.PopStack());
		OvVector3ValSPtr arg2 = OvCastTo<OvVector3Val>(env.PopStack());
		env.PushStack( arg1 );
		if ( arg1 && arg2 )
		{
			arg1->val = arg2->val;
		}
	}

}
