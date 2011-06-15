#pragma once
#include "OvRefObject.h"
#include "OvFileInputStream.h"
#include "OvFileOutputStream.h"

OvSmartPointer_Class(OvFile);
class OvFile : public OvRefObject
{
	OvRTTI_DECL(OvFile);
public:

	OvFile();
	OvFile( const OvString& file, const OvString& mode = "a+b" );
	~OvFile();

	OvBool Open( const OvString& file, const OvString& mode );
	OvBool Open();
	void   Close();

	OvBool IsExist();
	OvBool IsDirectory();
	OvBool IsFile();

	OvString GetPath();
	OvString GetBasePath();

	OvString GetExtention();
	OvString GetFileName();
	
	FILE* GetHandle();

private:

	FILE* m_file;
	OvString m_path;
	OvString m_mode;

};