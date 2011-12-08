#pragma once
#include "OvUtility.h"
#include "OvMemObject.h"
#include "OvSmartPointer.h"

// 클레스 이름 : OvRefObject
// 설명 : 

class  OvRefObject : public OvMemObject
{
	OvRTTI_DECL_ROOT(OvRefObject);
public:
	// 일단 생성되면서 카운티을 0으로 맞춘다.
	OvRefObject();

	// 현재, 사용중인 포인터들의 갯수를 얻어온다.
	OvInt		GetRefCount();

	// 아래의 함수들은 OvSmartPointer에서 사용하는 함수이지만
	// 사용자가 원한다면 참조를 올리거나 내릴수 있도록 public으로 선언했다.
	// (사용자가 직접 사용하길 권장하지 않는다.)
	OvInt		IncRefCount();
	OvInt		DecRefCount();

	// 왠만하면 사용하지 말자.
	void	DeleteThis(){OvDelete this;};
protected:

	// 소멸될때 아직 누군가 사용중이라 카운트가 0이 아닌 상태라면
	// 잘못된 삭제요청으로 간주하고 에러메세지를 뛰우고 
	// 강제 종료한다. (멀리봤을때 처음 잘못된 삭제가 요청됬을때 잡아주는게 좋다고 생각해서다.)
	virtual ~OvRefObject();
private:
	OvInt	m_ref_count;
	OvCriticalSection m_cs;
};