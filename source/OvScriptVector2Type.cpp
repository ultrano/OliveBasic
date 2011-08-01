#include "OvScriptDataType.h"
#include "OvScriptState.h"
#include "OvActObject.h"
#include "OvComponent.h"

OvObjectSPtr OvScriptVector2Type::NewInstance() { return OvNew OvVector2Val; };

void OvScriptVector2Type::Method( OvScriptState& env, const OvString& method ) 
{
	if ( method == "_construct" )
	{
		OvVector2ValSPtr arg1 = OvCastTo<OvVector2Val>(env.PopStack());
		OvStringValSPtr arg2 = OvCastTo<OvStringVal>(env.PopStack());
		if ( arg1 && arg2 )
		{
			arg1->val = OvFromString<OvVector2>( arg2->val );
		}
	}
	else if ( method == "_plus" )
	{
		OvVector2ValSPtr arg1 = OvCastTo<OvVector2Val>(env.PopStack());
		OvVector2ValSPtr arg2 = OvCastTo<OvVector2Val>(env.PopStack());
		OvVector2ValSPtr ret  = NewInstance();
		env.PushStack( ret );
		if ( arg1 && arg2 )
		{
			ret->val = arg1->val + arg2->val;
		}
	}
	else if ( method == "_minus" )
	{
		OvVector2ValSPtr arg1 = OvCastTo<OvVector2Val>(env.PopStack());
		OvVector2ValSPtr arg2 = OvCastTo<OvVector2Val>(env.PopStack());
		OvVector2ValSPtr ret  = NewInstance();
		env.PushStack( ret );
		if ( arg1 && arg2 )
		{
			ret->val = arg1->val - arg2->val;
		}
	}
	else if ( method == "_div" )
	{
		OvVector2ValSPtr arg1 = OvCastTo<OvVector2Val>(env.PopStack());
		OvFloatValSPtr arg2 = OvCastTo<OvFloatVal>(env.PopStack());
		OvVector2ValSPtr ret  = NewInstance();
		env.PushStack( ret );
		if ( arg1 && arg2 )
		{
			ret->val = arg1->val / arg2->val;
		}
	}
	else if ( method == "_mul" )
	{
		OvVector2ValSPtr arg1 = OvCastTo<OvVector2Val>(env.PopStack());
		OvFloatValSPtr arg2 = OvCastTo<OvFloatVal>(env.PopStack());
		OvVector2ValSPtr ret  = NewInstance();
		env.PushStack( ret );
		if ( arg1 && arg2 )
		{
			ret->val = arg1->val * arg2->val;
		}
	}
	else if ( method == "_set" )
	{
		OvVector2ValSPtr arg1 = OvCastTo<OvVector2Val>(env.PopStack());
		OvVector2ValSPtr arg2 = OvCastTo<OvVector2Val>(env.PopStack());
		env.PushStack( arg1 );
		if ( arg1 && arg2 )
		{
			arg1->val = arg2->val;
		}
	}

}
