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
	explain: 데이터를 생성하고 스택에 쌓는다.
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
	explain: 스택 맨 위의 값을 꺼네서 [name] 의 변수에 넣는다.
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
	explain: 해당 이름의 변수를 Script State에서 지움.
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
	explain: 해당 이름의 변수를 스택 위에 쌓는다.
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
	explain: [type]의 [method]를 호출함.
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
	explain: Script State를 조절하는 커맨드
	cmd: @state [control cmd]
	example: 
			@state clear
*/
//////////////////////////////////////////////////////////////////////////
class OvStateCommand : public OvScriptCommand
{
	virtual void	CmpProc( OvScriptState& env, const OvCmdArgs& args ) OVERRIDE;
};