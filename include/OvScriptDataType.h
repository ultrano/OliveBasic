#pragma once
#include "OvUtility.h"
#include "OvValue.h"
#include "OvObject.h"

class OvScriptState;
OvSmartPointer_Class(OvScriptDataType);
class OvScriptDataType : public OvRefObject
{
public:
	virtual void		 Method( OvScriptState& env, const OvString& method ){};
	virtual OvObjectSPtr NewInstance() = 0 ;
};

class OvScriptFloatType : public OvScriptDataType
{
	virtual void		 Method( OvScriptState& env, const OvString& method ) OVERRIDE;
	virtual OvObjectSPtr NewInstance() OVERRIDE;
};
class OvScriptVector2Type : public OvScriptDataType
{
	virtual void		 Method( OvScriptState& env, const OvString& method ) OVERRIDE;
	virtual OvObjectSPtr NewInstance() OVERRIDE;
};
class OvScriptVector3Type : public OvScriptDataType
{
	virtual void		 Method( OvScriptState& env, const OvString& method ) OVERRIDE;
	virtual OvObjectSPtr NewInstance() OVERRIDE;
};
class OvScriptStringType : public OvScriptDataType
{
	virtual void		 Method( OvScriptState& env, const OvString& method ) OVERRIDE;
	virtual OvObjectSPtr NewInstance() OVERRIDE;
};
class OvScriptColorType : public OvScriptDataType
{
	virtual void		 Method( OvScriptState& env, const OvString& method ) OVERRIDE;
	virtual OvObjectSPtr NewInstance() OVERRIDE;
};

OvSmartPointer_Class(OvScriptActorType);
class OvScriptActorType : public OvScriptDataType
{
public:
	OvPropertyTable properties;
	OvSet<OvString>	components;

	virtual void		 Method( OvScriptState& env, const OvString& method ) OVERRIDE;
	virtual OvObjectSPtr NewInstance() OVERRIDE;
};