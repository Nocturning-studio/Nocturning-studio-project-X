#ifndef	dx10StateCache_included
#define	dx10StateCache_included
#pragma once

template <class IDeviceState, class StateDecs> 
class ENGINE_API dx10StateCache
{
	//	Public interface
public:
	dx10StateCache();
	~dx10StateCache();

	void			ClearStateArray();

	IDeviceState*	GetState( SimulatorStates& state_code );
	IDeviceState*	GetState( StateDecs& desc );
	//	Can be called on device destruction only!
	//	dx10State holds weak links on manager's states and 
	//	won't understand that state was destroyed
	//void	FlushStates();
	//	Private functionality

	//	Private declarations
private:
	struct StateRecord 
	{
		u32						m_crc;
		IDeviceState*			m_pState;
	};

private:
	void			CreateState( StateDecs desc, IDeviceState** ppIState );
	IDeviceState*	FindState( const StateDecs& desc, u32 StateCRC );

	//	Private data
private:
	//	This must be cleared on device destroy
	xr_vector<StateRecord>	m_StateArray;
};

extern dx10StateCache<ID3D11RasterizerState, D3D11_RASTERIZER_DESC> RSManager;
extern dx10StateCache<ID3D11DepthStencilState, D3D11_DEPTH_STENCIL_DESC> DSSManager;
extern dx10StateCache<ID3D11BlendState, D3D11_BLEND_DESC> BSManager;

#include "StateCacheImpl.h"

#endif	//	dx10StateCache_included