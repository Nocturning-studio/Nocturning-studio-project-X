#ifndef	dx10BufferUtils_included
#define	dx10BufferUtils_included
#pragma once

namespace dx10BufferUtils
{
HRESULT CreateVertexBuffer(ID3D11Buffer** ppBuffer, const void* pData, UINT DataSize, bool bImmutable = true);
HRESULT CreateIndexBuffer(ID3D11Buffer** ppBuffer, const void* pData, UINT DataSize, bool bImmutable = true);
HRESULT	CreateConstantBuffer( ID3D11Buffer** ppBuffer, UINT DataSize);
void	ConvertVertexDeclaration( const xr_vector<D3DVERTEXELEMENT9> &declIn, xr_vector<D3D11_INPUT_ELEMENT_DESC> &declOut);
};

#endif	//	dx10BufferUtils_included