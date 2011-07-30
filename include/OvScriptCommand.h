#pragma once
#include "OvRefObject.h"
#include "OvTypeDef.h"

class OvScriptState;

OvSmartPointer_Class(OvScriptCommand);
class OvScriptCommand : public OvRefObject
{
public:
	virtual void	CmpProc( OvScriptState& env, const OvString& parameters ) = 0;
};

class OvNewCommand : public OvScriptCommand
{
	virtual void	CmpProc( OvScriptState& env, const OvString& parameters ) OVERRIDE;
};

class OvSetCommand : public OvScriptCommand
{
	virtual void	CmpProc( OvScriptState& env, const OvString& parameters ) OVERRIDE;
};

class OvRemoveCommand : public OvScriptCommand
{
	virtual void	CmpProc( OvScriptState& env, const OvString& parameters ) OVERRIDE;
};

class OvStakupCommand : public OvScriptCommand
{
	virtual void	CmpProc( OvScriptState& env, const OvString& parameters ) OVERRIDE;
};

class OvMethodCommand : public OvScriptCommand
{
	virtual void	CmpProc( OvScriptState& env, const OvString& parameters ) OVERRIDE;
};