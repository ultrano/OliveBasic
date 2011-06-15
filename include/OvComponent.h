#include "OvObject.h"

OvSmartPointer_Class(OvXObject);
OvSmartPointer_Class(OvComponent);
class OvComponent : public OvObject
{
	OvRTTI_DECL(OvComponent);
public:

	virtual void	Update( OvTimeTick elapsed ) = 0;

	void			SetTarget( OvXObjectSPtr target );
	OvXObjectSPtr	GetTarget();

private:

	OvXObjectSPtr m_target;

};