#include "OvObject.h"

OvREF_POINTER(OvNexObject);
OvREF_POINTER(OvComponent);
class OvComponent : public OvObject
{
	OvRTTI_DECL(OvComponent);
public:

	void			SetTarget( OvNexObjectSPtr target );
	OvNexObjectSPtr GetTarget();

private:

	OvNexObjectSPtr m_target;

};