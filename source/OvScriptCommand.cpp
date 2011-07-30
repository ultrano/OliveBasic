#include "OvScriptCommand.h"
#include "OvScriptDataType.h"
#include "OvScriptState.h"
#include "OvUtility.h"

void OvNewCommand::CmpProc( OvScriptState& env, const OvString& parameters ) 
{
	OvString type;
	OvString init;
	OvString dummy;
	OU::string::split( parameters, type, init );
	OU::string::split( init, init, dummy );

	if ( !type.empty() )
	{
		OvObjectSPtr obj = NULL;
		if ( OvScriptDataType* datatype = env.FindDataType( type ) )
		{
			obj = datatype->NewInstance();
			env.PushStack( OvNew OvStringVal( init ) );
			env.PushStack( obj );
			datatype->Method( env, "_construct" );
		}
	}
}

void OvSetCommand::CmpProc( OvScriptState& env, const OvString& parameters ) 
{
	OvString name;
	OvString dummy;
	OU::string::split( parameters, name, dummy );

	if ( !name.empty() )
	{
		env.SetVariable( name, env.PopStack() );
	}
}

void OvRemoveCommand::CmpProc( OvScriptState& env, const OvString& parameters ) 
{
	OvString name;
	OvString dummy;
	OU::string::split( parameters, name, dummy );
	if ( !name.empty() )
	{
		env.RemoveVariable( name );
	}
}

void OvStakupCommand::CmpProc( OvScriptState& env, const OvString& parameters )
{
	OvString name;
	OvString dummy;
	OU::string::split( parameters, name, dummy );
	if ( !name.empty() )
	{
		env.PushStack( env.GetVariable( name ) );
	}
}

void OvMethodCommand::CmpProc( OvScriptState& env, const OvString& parameters ) 
{
	OvString type;
	OvString method;
	OvString dummy;
	OU::string::split( parameters, type, method );
	OU::string::split( method, method, dummy );

	if ( !type.empty() && !method.empty() )
	{
		if ( OvScriptDataType* datatype = env.FindDataType( type ) )
		{
			datatype->Method( env, method );
		}
	}
}
