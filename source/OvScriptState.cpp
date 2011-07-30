#include "OvScriptState.h"
#include "OvUtility.h"
#include "OvScriptDataType.h"
#include "OvScriptCommand.h"
#include <iostream>
#include <fstream>

OvScriptState::OvScriptState()
{
	m_version = "v1.0";

	RegisterDataType( "float", OvNew OvScriptFloatType );
	RegisterDataType( "vector2", OvNew OvScriptVector2Type );
	RegisterDataType( "vector3", OvNew OvScriptVector3Type );
	RegisterDataType( "string", OvNew OvScriptStringType );
	RegisterDataType( "color", OvNew OvScriptColorType );
	RegisterDataType( "actor", OvNew OvScriptActorType );

	RegisterCommand( "@new", OvNew OvNewCommand);
	RegisterCommand( "@set", OvNew OvSetCommand );
	RegisterCommand( "@remove", OvNew OvRemoveCommand );
	RegisterCommand( "@stackup", OvNew OvStakupCommand );
	RegisterCommand( "@method", OvNew OvMethodCommand );
}

OvScriptState::~OvScriptState()
{

}

void OvScriptState::DoFile( const OvString& filepath )
{
	FILE* file = fopen( filepath.c_str(), "r" );
	if ( !file ) return ;
	const int bufsize = 256;
	char linebuf[bufsize];

	OvString cmd_line;
	while ( feof( file ) == 0 )
	{
		fgets( linebuf, bufsize, file );
		cmd_line = OU::string::trim( linebuf );
		if ( cmd_line.empty() ) continue;

		DoCommand( cmd_line );
	}

	fclose( file );
}

void OvScriptState::DoCommand( const OvString& cmdline )
{
	OvString cmdname;
	OvString parameters;
	OU::string::split( cmdline, cmdname, parameters );

	if ( OvScriptCommand* cmd = FindCommand( cmdname ) )
	{
		cmd->CmpProc( *this, parameters );
	}
}

OvBool OvScriptState::RemoveVariable( const OvString& name )
{
	if ( m_variables.find(name) != m_variables.end() )
	{
		m_variables.erase(name);
		return true;
	}
	return false;
}

void OvScriptState::SetVariable( const OvString& name, OvObjectSPtr var )
{
	m_variables[ name ] = var;
}

OvObjectSPtr OvScriptState::GetVariable( const OvString& name )
{
	local_variables::iterator itor = m_variables.find( name );
	if ( itor != m_variables.end() )
	{
		return itor->second;
	}
	return NULL;
}

void	OvScriptState::RegisterDataType( const OvString& nameoftype, OvScriptDataTypeSPtr datatype )
{
	m_type_table[nameoftype] = datatype;
}
OvObjectSPtr OvScriptState::NewDataTypeInstance( const OvString& nameoftype )
{
	OvScriptDataTypeSPtr datatype = FindDataType(nameoftype);
	if ( datatype )
	{
		return datatype->NewInstance();
	}
	return NULL;
}

OvScriptDataType* OvScriptState::FindDataType( const OvString& nameoftype )
{
	script_data_type_table::iterator itor = m_type_table.find(nameoftype);
	if ( m_type_table.end() != itor )
	{
		OvScriptDataTypeSPtr datatype = itor->second;
		if ( datatype )
		{
			return datatype.GetRear();
		}
	}
	return NULL;
}

void OvScriptState::PushStack( OvObjectSPtr obj )
{
	m_stack.push_back(obj);
}

OvObjectSPtr OvScriptState::PopStack()
{
	if ( m_stack.size() )
	{
		OvObjectSPtr obj;
		obj = m_stack.back();
		m_stack.pop_back();
		return obj;
	}
	return NULL;
}

OvSize OvScriptState::StackSize()
{
	return m_stack.size();
}

void OvScriptState::RegisterCommand( const OvString& cmdname, OvScriptCommandSPtr cmd )
{
	m_cmp_table[cmdname] = cmd;
}

OvScriptCommand* OvScriptState::FindCommand( const OvString& cmdname )
{
	script_command_table::iterator itor = m_cmp_table.find( cmdname );
	if ( itor != m_cmp_table.end() )
	{
		OvScriptCommandSPtr cmd = itor->second;
		return cmd.GetRear();
	}
	return NULL;
}
