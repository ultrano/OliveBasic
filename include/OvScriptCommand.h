#pragma once
#include "OvRefObject.h"
#include "OvTypeDef.h"

typedef OvVector<OvString> OvCmdArgs;
class OvScriptState;
OvSmartPointer_Class(OvScriptCommand);
class OvScriptCommand : public OvRefObject
{
public:
	virtual void	CmpProc( OvScriptState& env, const OvCmdArgs& args ) = 0;
};
//////////////////////////////////////////////////////////////////////////
/*
	explain: �����͸� �����ϰ� ���ÿ� �״´�.
	cmd: @new [type] [init]
	example: 
			@new float 1.11

	example: 
			@new string "1.11"
*/
//////////////////////////////////////////////////////////////////////////
class OvNewCommand : public OvScriptCommand
{
	virtual void	CmpProc( OvScriptState& env, const OvCmdArgs& args ) OVERRIDE;
};

//////////////////////////////////////////////////////////////////////////
/*	
	explain: ���� �� ���� ���� ���׼� [name] �� ������ �ִ´�.
	cmd: @set [name]
	example: 
			@new string "stupid"
			@set myname
*/
//////////////////////////////////////////////////////////////////////////
class OvSetCommand : public OvScriptCommand
{
	virtual void	CmpProc( OvScriptState& env, const OvCmdArgs& args ) OVERRIDE;
};

//////////////////////////////////////////////////////////////////////////
/*	
	explain: �ش� �̸��� ������ Script State���� ����.
	cmd: @remove [name]
	example: 
			@remove myname
*/
//////////////////////////////////////////////////////////////////////////
class OvRemoveCommand : public OvScriptCommand
{
	virtual void	CmpProc( OvScriptState& env, const OvCmdArgs& args ) OVERRIDE;
};

//////////////////////////////////////////////////////////////////////////
/*	
	explain: �ش� �̸��� ������ ���� ���� �״´�.
	cmd: @stackup [name]
	example: 
			@stackup myname
*/
//////////////////////////////////////////////////////////////////////////
class OvStakupCommand : public OvScriptCommand
{
	virtual void	CmpProc( OvScriptState& env, const OvCmdArgs& args ) OVERRIDE;
};

//////////////////////////////////////////////////////////////////////////
/*	
	explain: [type]�� [method]�� ȣ����.
	cmd: @method [type] [method]
	example: 
			@new float 1.11
			@new float 2.22
			@method float _plus
			@set result
*/
//////////////////////////////////////////////////////////////////////////
class OvMethodCommand : public OvScriptCommand
{
	virtual void	CmpProc( OvScriptState& env, const OvCmdArgs& args ) OVERRIDE;
};

//////////////////////////////////////////////////////////////////////////
/*	
	explain: Script State�� �����ϴ� Ŀ�ǵ�
	cmd: @state [control cmd]
	example: 
			@state clear
*/
//////////////////////////////////////////////////////////////////////////
class OvStateCommand : public OvScriptCommand
{
	virtual void	CmpProc( OvScriptState& env, const OvCmdArgs& args ) OVERRIDE;
};