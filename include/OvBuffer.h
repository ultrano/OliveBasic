#pragma once
#include "OvRefObject.h"
#include "OvBufferInputStream.h"
#include "OvBufferOutputStream.h"

OvSmartPointer_Class(OvBuffer);
class OvBuffer : public OvRefObject
{
	OvRTTI_DECL(OvBuffer);
public:

	enum { 
		DEFAULT_BUFFER_CAPACITY = 0,
		DEFAULT_BUFFER_INCREMENT = 128 
	};

	static OvBufferSPtr CreateBuffer( OvSize capacity = DEFAULT_BUFFER_CAPACITY
									, OvSize increment = DEFAULT_BUFFER_INCREMENT );

public:

	OvByte *	Pointer();
	OvSize		Size();
	OvByte *	GetBuffer( OvSize& size );
	OvSize		Increment();

	void		Establish( OvUInt times = 1 );

private:
	OvBuffer();
	~OvBuffer();
private:
	OvByte*		m_buf;
	OvSize		m_size;
	OvSize		m_increment;
};