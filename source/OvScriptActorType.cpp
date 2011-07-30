#include "OvScriptDataType.h"
#include "OvScriptState.h"
#include "OvActObject.h"
#include "OvComponent.h"

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

void OvScriptActorType::Method( OvScriptState& env, const OvString& method ) 
{
	if ( method == "_construct" )
	{
		OvColorValSPtr arg1 = OvCastTo<OvColorVal>(env.PopStack());
		OvStringValSPtr arg2 = OvCastTo<OvStringVal>(env.PopStack());
		env.PushStack( arg1 );
	}
	else if ( method == "findprop" )
	{
		OvActObjectSPtr arg1 = OvCastTo<OvActObject>(env.PopStack());
		OvStringValSPtr arg2 = OvCastTo<OvStringVal>(env.PopStack());
		OvObjectSPtr prop = NULL;
		if ( arg1 && arg2 )
		{
			prop = arg1->FindProp( arg2->val );
		}
		env.PushStack( prop );
	}

}
