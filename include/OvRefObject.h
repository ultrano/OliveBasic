#pragma once
#include "OvUtility.h"
#include "OvMemObject.h"
#include "OvSmartPointer.h"

// Ŭ���� �̸� : OvRefObject
// ���� : 

class  OvRefObject : public OvMemObject
{
	OvRTTI_DECL_ROOT(OvRefObject);
public:
	// �ϴ� �����Ǹ鼭 ī��Ƽ�� 0���� �����.
	OvRefObject();

	// ����, ������� �����͵��� ������ ���´�.
	OvInt		GetRefCount();

	// �Ʒ��� �Լ����� OvSmartPointer���� ����ϴ� �Լ�������
	// ����ڰ� ���Ѵٸ� ������ �ø��ų� ������ �ֵ��� public���� �����ߴ�.
	// (����ڰ� ���� ����ϱ� �������� �ʴ´�.)
	OvInt		IncRefCount();
	OvInt		DecRefCount();

	// �ظ��ϸ� ������� ����.
	void	DeleteThis(){OvDelete this;};
protected:

	// �Ҹ�ɶ� ���� ������ ������̶� ī��Ʈ�� 0�� �ƴ� ���¶��
	// �߸��� ������û���� �����ϰ� �����޼����� �ٿ�� 
	// ���� �����Ѵ�. (�ָ������� ó�� �߸��� ������ ��û������ ����ִ°� ���ٰ� �����ؼ���.)
	virtual ~OvRefObject();
private:
	OvInt	m_ref_count;
	OvCriticalSection m_cs;
};