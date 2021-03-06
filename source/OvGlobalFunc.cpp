#include "OvGlobalFunc.h"
#include "OvMemObject.h"
#include "OvUtility.h"
using namespace std;

void	OvGetLastError()
{
	LPVOID lpMsgBuf;

	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
		);
	MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );

	LocalFree( lpMsgBuf );
};

void	OvMessageBox(const OvChar* _msg,const OvChar* _caption)
{
	MessageBox(NULL,_msg,_caption,MB_OK);
}

void	OvErrorMsgBox(const OvChar* _file,const OvChar* _block, const OvUInt _line, const OvChar* _msg )
{
	OvString errmsg = OU::string::format(	"File: %s\n"
					"Block: %s\n"
					"Line: %d\n"
					"Assert: %s"
					,_file
					,_block
					,_line
					,_msg);

	MessageBox(NULL, errmsg.c_str() ,"[Run time error]",MB_OK);

}

OvBool		OvStringAllocator(LPTSTR* _lpp_dest,LPCTSTR lp_src)
{
	if (!lp_src)
	{
		return false;
	}
	size_t	k_len = strlen(lp_src) + 1;
	*_lpp_dest = (OvChar*)OvMemAlloc( k_len );
	memset((void*)(*_lpp_dest),0,k_len*sizeof(OvChar));
	if (!(*_lpp_dest))
	{
		return false;
	}
	memcpy((void*)(*_lpp_dest),(void*)lp_src,k_len*sizeof(OvChar));
	return true;
};
OvString	OvGetDirectoryInFullFilePath(const OvString& strFileFullPath)
{
	OvString kReturnString= "";
	OvChar separator[] = {'\\','/'};
	for ( OvUShort i = 0 ; i < 2 ; ++i )
	{
		size_t stTok = strFileFullPath.rfind( separator[i] );
		if (stTok != OvString::npos)
		{
			kReturnString = strFileFullPath;
			kReturnString.resize( stTok );
		}
	}
	return kReturnString;
}
OvString	OvGetFileNameInFullFilePath(const OvString& strFileFullPath)
{
	OvString kReturnString= "";
	OvChar separator[] = {'\\','/'};
	for ( OvUShort i = 0 ; i < 2 ; ++i )
	{
		size_t stTok = strFileFullPath.rfind( separator[i] );
		if (stTok != OvString::npos)
		{
			kReturnString = &strFileFullPath[stTok+1];
			stTok = kReturnString.rfind('.');
			kReturnString.resize(stTok);
		}
	}
	return kReturnString;
}
OvString	OvGetExtentionInFullFilePath(const OvString& strFileFullPath)
{
	OvString kReturnString= "";
	size_t stTok = strFileFullPath.rfind('.');
	if (stTok != OvString::npos)
	{
		kReturnString = &strFileFullPath[stTok];
	}
	return kReturnString;
}

OvUInt OvMath::Factorial( OvUInt total )
{
	OvUInt result = 1;
	while ( total )
	{
		result *= total;
		--total;
	}
	return result;
}

OvUInt OvMath::Factorial( OvUInt total, OvUInt select )
{
	return ( OvMath::Factorial( total ) / OvMath::Factorial( select ) );
}

OvUInt OvMath::Combination( OvUInt total, OvUInt select )
{
	return ( OvMath::Factorial( total ) / ( OvMath::Factorial( select ) * OvMath::Factorial( total - select ) ) );
}

OvUInt OvMath::Sigma( OvUInt total )
{
	return ((total * ( total + 1 )) / 2);
}