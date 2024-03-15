#ifndef	dx10ShaderResourceStateCache_included
#define	dx10ShaderResourceStateCache_included
#pragma once

class dx10ShaderResourceStateCache
{
public:
	dx10ShaderResourceStateCache();

	void	ResetDeviceState();

	void	Apply();

	void	SetPSResource( u32 uiSlot, ID3D11ShaderResourceView	*pRes );
	void	SetGSResource( u32 uiSlot, ID3D11ShaderResourceView	*pRes );
	void	SetVSResource( u32 uiSlot, ID3D11ShaderResourceView	*pRes );
	void	SetDSResource( u32 uiSlot, ID3D11ShaderResourceView	*pRes );
	void	SetHSResource( u32 uiSlot, ID3D11ShaderResourceView	*pRes );
	void	SetCSResource( u32 uiSlot, ID3D11ShaderResourceView	*pRes );

private:
	ID3D11ShaderResourceView* m_PSViews[CBackend::mtMaxPixelShaderTextures];
	ID3D11ShaderResourceView* m_GSViews[CBackend::mtMaxGeometryShaderTextures];
	ID3D11ShaderResourceView* m_VSViews[CBackend::mtMaxVertexShaderTextures];
	ID3D11ShaderResourceView* m_HSViews[CBackend::mtMaxHullShaderTextures];
	ID3D11ShaderResourceView* m_DSViews[CBackend::mtMaxDomainShaderTextures];
	ID3D11ShaderResourceView* m_CSViews[CBackend::mtMaxComputeShaderTextures];

	u32		m_uiMinPSView;
	u32		m_uiMaxPSView;

	u32		m_uiMinGSView;
	u32		m_uiMaxGSView;

	u32		m_uiMinVSView;
	u32		m_uiMaxVSView;

	u32		m_uiMinHSView;
	u32		m_uiMaxHSView;

	u32		m_uiMinDSView;
	u32		m_uiMaxDSView;

	u32		m_uiMinCSView;
	u32		m_uiMaxCSView;

	bool	m_bUpdatePSViews;
	bool	m_bUpdateGSViews;
	bool	m_bUpdateVSViews;
	bool	m_bUpdateHSViews;
	bool	m_bUpdateDSViews;
	bool	m_bUpdateCSViews;
};

extern	dx10ShaderResourceStateCache	SRVSManager;

#endif	//	dx10ShaderResourceStateCache_included