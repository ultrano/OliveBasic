#pragma once
#include "OvOutputStream.h"

OvSmartPointer_Class(OvFile);
OvSmartPointer_Class(OvFileOutputStream);
class OvFileOutputStream : public OvOutputStream
{
private:
	OvFileOutputStream();
public:

	OvFileOutputStream( OvFileSPtr file );
	OvFileOutputStream( const OvString& file, const OvString& mode = "w+b" );

	virtual OvSize WriteBytes( OvByte * write_buf, OvSize write_size ) OVERRIDE;

	OvSize Skip( OvSize offset ) ;

	OvBool Open( const OvString& file, const OvString& mode = "w+b" );
	void Close();
	OvFileSPtr GetFile();

private:

	OvFileSPtr m_file;

};