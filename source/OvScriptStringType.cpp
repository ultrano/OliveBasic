#include "OvScriptDataType.h"
#include "OvScriptState.h"
#include "OvActObject.h"
#include "OvComponent.h"

OvObjectSPtr OvScriptStringType::NewInstance() { return OvNew OvStringVal; };

void OvScriptStringType::Method( OvScriptState& env, const OvString& method ) 
{
	if ( method == "_construct" )
	{
		OvStringValSPtr arg1 = OvCastTo<OvStringVal>(env.PopStack());
		OvStringValSPtr arg2 = OvCastTo<OvStringVal>(env.PopStack());
		env.PushStack( arg1 );
		if ( arg1 && arg2 )
		{
			arg1->val = OvFromString<OvString>( arg2->val );
		}
	}
	else if ( method == "_plus" )
	{
		OvStringValSPtr arg1 = OvCastTo<OvStringVal>(env.PopStack());
		OvStringValSPtr arg2 = OvCastTo<OvStringVal>(env.PopStack());
		OvStringValSPtr ret  = NewInstance();
		env.PushStack( ret );
		if ( arg1 && arg2 )
		{
			ret->val = arg1->val + arg2->val;
		}
	}
	else if ( method == "_set" )
	{
		OvStringValSPtr arg1 = OvCastTo<OvStringVal>(env.PopStack());
		OvStringValSPtr arg2 = OvCastTo<OvStringVal>(env.PopStack());
		env.PushStack( arg1 );
		if ( arg1 && arg2 )
		{
			arg1->val = arg2->val;
		}
	}

}
