#pragma once
#include "OvInputStream.h"

OvSmartPointer_Class(OvFile);
OvSmartPointer_Class(OvFileOutputStream);
class OvFileInputStream : public OvInputStream
{
private:
	OvFileInputStream();
public:

	OvFileInputStream( const OvString& file, const OvString& mode = "rb" );

	virtual OvSize ReadBytes( OvByte * dest, OvSize dest_size ) OVERRIDE;

	OvSize Skip( OvSize offset ) ;

	OvBool Open( const OvString& file, const OvString& mode = "r+b" );
	void Close();
	OvFileSPtr GetFile();

private:

	OvFileSPtr m_file;

};