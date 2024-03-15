#ifndef	dx10State_included
#define	dx10State_included
#pragma once

class SimulatorStates;

#pragma comment(Reminder("Fix me :("))
#include "SamplerStateCache.h"
#include "StateCache.h"
#include "StateCacheImpl.h"
#include "StateUtils.h"
#include "StateManager.h"


class dx10State
{
//	Public interface
public:
	dx10State();	//	These have to be private bu xr_new/xr_delete don't support this
	~dx10State();

	static dx10State* Create( SimulatorStates& state_code );

	//	DX9 unified interface
	HRESULT Apply();
	void	Release();

	//	DX10 specific
	void	UpdateStencilRef( UINT Ref ) { m_uiStencilRef = Ref; }
	void	UpdateAlphaRef( UINT Ref ) { m_uiAlphaRef = Ref; }

//	User restricted interface
private:
	typedef	dx10SamplerStateCache::HArray tSamplerHArray;

private:
	static void InitSamplers( tSamplerHArray& SamplerArray, SimulatorStates& state_code, int iBaseSamplerIndex);

private:
	//	All states are supposed to live along all application lifetime
	ID3D11RasterizerState*		m_pRasterizerState;		//	Weak link
	ID3D11DepthStencilState*	m_pDepthStencilState;	//	Weak link
	ID3D11BlendState*			m_pBlendState;			//	Weak link

	tSamplerHArray				m_VSSamplers;
	tSamplerHArray				m_PSSamplers;
	tSamplerHArray				m_GSSamplers;
	tSamplerHArray				m_CSSamplers;
	tSamplerHArray				m_HSSamplers;
	tSamplerHArray				m_DSSamplers;

	UINT						m_uiStencilRef;
	UINT						m_uiAlphaRef;

//	Private data
private:
};

#endif	//	dx10State_included