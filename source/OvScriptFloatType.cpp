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
	else if ( method == "_div" )
	{
		OvFloatValSPtr arg1 = OvCastTo<OvFloatVal>(env.PopStack());
		OvFloatValSPtr arg2 = OvCastTo<OvFloatVal>(env.PopStack());
		OvFloatValSPtr ret  = NewInstance();
		env.PushStack( ret );
		if ( arg1 && arg2 )
		{
			ret->val = arg1->val / arg2->val;
		}
	}
	else if ( method == "_mul" )
	{
		OvFloatValSPtr arg1 = OvCastTo<OvFloatVal>(env.PopStack());
		OvFloatValSPtr arg2 = OvCastTo<OvFloatVal>(env.PopStack());
		OvFloatValSPtr ret  = NewInstance();
		env.PushStack( ret );
		if ( arg1 && arg2 )
		{
			ret->val = arg1->val * arg2->val;
		}
	}
	else if ( method == "_set" )
	{
		OvFloatValSPtr arg1 = OvCastTo<OvFloatVal>(env.PopStack());
		OvFloatValSPtr arg2 = OvCastTo<OvFloatVal>(env.PopStack());
		env.PushStack( arg1 );
		if ( arg1 && arg2 )
		{
			arg1->val = arg2->val;
		}
	}
}
