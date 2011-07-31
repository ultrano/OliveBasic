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

class OvNewCommand : public OvScriptCommand
{
	virtual void	CmpProc( OvScriptState& env, const OvCmdArgs& args ) OVERRIDE;
};

class OvSetCommand : public OvScriptCommand
{
	virtual void	CmpProc( OvScriptState& env, const OvCmdArgs& args ) OVERRIDE;
};

class OvRemoveCommand : public OvScriptCommand
{
	virtual void	CmpProc( OvScriptState& env, const OvCmdArgs& args ) OVERRIDE;
};

class OvStakupCommand : public OvScriptCommand
{
	virtual void	CmpProc( OvScriptState& env, const OvCmdArgs& args ) OVERRIDE;
};

class OvMethodCommand : public OvScriptCommand
{
	virtual void	CmpProc( OvScriptState& env, const OvCmdArgs& args ) OVERRIDE;
};

class OvStateCommand : public OvScriptCommand
{
	virtual void	CmpProc( OvScriptState& env, const OvCmdArgs& args ) OVERRIDE;
};