#include "OvScriptDataType.h"
#include "OvScriptState.h"
#include "OvActObject.h"
#include "OvComponent.h"

OvObjectSPtr OvScriptColorType::NewInstance() { return OvNew OvColorVal; };

void OvScriptColorType::Method( OvScriptState& env, const OvString& method ) 
{
	if ( method == "_construct" )
	{
		OvColorValSPtr arg1 = OvCastTo<OvColorVal>(env.PopStack());
		OvStringValSPtr arg2 = OvCastTo<OvStringVal>(env.PopStack());
		env.PushStack( arg1 );
		if ( arg1 && arg2 )
		{
			arg1->val = OvFromString<OvColor>( arg2->val );
		}
	}
	else if ( method == "_set" )
	{
		OvColorValSPtr arg1 = OvCastTo<OvColorVal>(env.PopStack());
		OvColorValSPtr arg2 = OvCastTo<OvColorVal>(env.PopStack());
		env.PushStack( arg1 );
		if ( arg1 && arg2 )
		{
			arg1->val = arg2->val;
		}
	}

}
