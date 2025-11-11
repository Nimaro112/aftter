#pragma  once

#include "GameCommon.h"
#include "../Gameplay/SharedUsableItem.h"

class obj_Dropped : public SharedUsableItem
{
	DECLARE_CLASS(obj_Dropped, SharedUsableItem)
public:
	obj_Dropped();
	virtual ~obj_Dropped();

	virtual	BOOL		Load(const char *name);

	virtual	BOOL		OnCreate();
	virtual BOOL		OnDestroy();

	virtual BOOL		Update();

	virtual void		AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam ) OVERRIDE;

	uint32_t			m_ItemID;
	float				m_RotX;
	uint32_t			m_OwnerCustomerID; // to show it to devs
};
