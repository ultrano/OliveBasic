#include "OvScriptCommand.h"
#include "OvScriptDataType.h"
#include "OvScriptState.h"
#include "OvUtility.h"

void OvNewCommand::CmpProc( OvScriptState& env, const OvCmdArgs& args ) 
{

	if ( !args.empty() )
	{
		OvString init;
		if ( args.size() >= 2 ) init = args.at(1);

		OvObjectSPtr obj = NULL;
		if ( OvScriptDataType* datatype = env.FindDataType( args.at(0) ) )
		{
			obj = datatype->NewInstance();
			env.PushStack( OvNew OvStringVal( init ) );
			env.PushStack( obj );
			datatype->Method( env, "_construct" );
			env.PushStack( obj );
		}
	}
}

void OvSetCommand::CmpProc( OvScriptState& env, const OvCmdArgs& args ) 
{
	if ( !args.empty() )
	{
		env.SetVariable( args.at(0), env.PopStack() );
	}
}

void OvRemoveCommand::CmpProc( OvScriptState& env, const OvCmdArgs& args ) 
{
	if ( !args.empty() )
	{
		env.RemoveVariable( args.at(0) );
	}
}

void OvStakupCommand::CmpProc( OvScriptState& env, const OvCmdArgs& args )
{
	if ( !args.empty() )
	{
		env.PushStack( env.GetVariable( args.at(0) ) );
	}
}

void OvMethodCommand::CmpProc( OvScriptState& env, const OvCmdArgs& args ) 
{
	if ( args.size() >= 2 )
	{
		if ( OvScriptDataType* datatype = env.FindDataType( args.at(0) ) )
		{
			datatype->Method( env, args.at(1) );
		}
	}
}

void OvStateCommand::CmpProc( OvScriptState& env, const OvCmdArgs& args ) 
{
	if ( !args.empty() )
	{
		const OvString& func = args.at(0);

		if ( func == "clear" )
		{
			env.Clear();
		}
	}
}
