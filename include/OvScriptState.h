#pragma once
#include "OvRefObject.h"
#include "OvTypeDef.h"
#include "OvObject.h"

OvSmartPointer_Class(OvScriptCommand);
OvSmartPointer_Class(OvScriptDataType);
OvSmartPointer_Class(OvScriptState);
class OvScriptState : public OvRefObject
{
	typedef OvMap<OvString,OvObjectSPtr> local_variables;
	typedef OvMap<OvString,OvScriptDataTypeSPtr> script_data_type_table;
	typedef OvMap<OvString,OvScriptCommandSPtr> script_command_table;
public:

	OvScriptState();
	~OvScriptState();

	void				DoFile( const OvString& filepath );
	void				DoCommand( const OvString& cmdline );

	OvBool				RemoveVariable( const OvString& name );
	void				SetVariable( const OvString& name, OvObjectSPtr var );
	OvObjectSPtr		GetVariable( const OvString& name );

	void				RegisterDataType( const OvString& nameoftype, OvScriptDataTypeSPtr datatype );
	OvScriptDataType*	FindDataType( const OvString& nameoftype );
	OvObjectSPtr		NewDataTypeInstance( const OvString& nameoftype );

	void				RegisterCommand( const OvString& cmdname, OvScriptCommandSPtr cmd );
	OvScriptCommand*	FindCommand( const OvString& cmdname );

	void				PushStack( OvObjectSPtr obj );
	OvObjectSPtr		PopStack();
	OvSize				StackSize();

	void				Clear();
private:
	
	script_data_type_table	m_type_table;
	script_command_table	m_cmp_table;
	local_variables			m_variables;
	OvList<OvObjectSPtr>	m_stack;
};