#ifndef sh_atomicH
#define sh_atomicH
#pragma once
#include "xr_resource.h"
#include "tss_def.h"

#include "dx11/State.h"

#pragma pack(push, 4)

//////////////////////////////////////////////////////////////////////////
// Atomic resources
//////////////////////////////////////////////////////////////////////////
struct ENGINE_API SInputSignature : public xr_resource_flagged
{
	ID3DBlob* signature;
	SInputSignature(ID3DBlob* pBlob);
	~SInputSignature();
};
typedef resptr_core<SInputSignature, resptr_base<SInputSignature>> ref_input_sign;
//////////////////////////////////////////////////////////////////////////
struct ENGINE_API SVS : public xr_resource_named
{
	ID3D11VertexShader* sh;
	R_constant_table constants;
	ref_input_sign signature;
	SVS();
	~SVS();
};
typedef resptr_core<SVS, resptr_base<SVS>> ref_vs;

//////////////////////////////////////////////////////////////////////////
struct ENGINE_API SPS : public xr_resource_named
{
	ID3D11PixelShader* sh;
	R_constant_table constants;
	~SPS();
};
typedef resptr_core<SPS, resptr_base<SPS>> ref_ps;

//////////////////////////////////////////////////////////////////////////
struct ENGINE_API SGS : public xr_resource_named
{
	ID3D11GeometryShader* sh;
	R_constant_table constants;
	~SGS();
};
typedef resptr_core<SGS, resptr_base<SGS>> ref_gs;


struct ENGINE_API SHS : public xr_resource_named
{
	ID3D11HullShader* sh;
	R_constant_table constants;
	~SHS();
};
typedef resptr_core<SHS, resptr_base<SHS>> ref_hs;

struct ENGINE_API SDS : public xr_resource_named
{
	ID3D11DomainShader* sh;
	R_constant_table constants;
	~SDS();
};
typedef resptr_core<SDS, resptr_base<SDS>> ref_ds;

struct ENGINE_API SCS : public xr_resource_named
{
	ID3D11ComputeShader* sh;
	R_constant_table constants;
	~SCS();
};
typedef resptr_core<SCS, resptr_base<SCS>> ref_cs;

//////////////////////////////////////////////////////////////////////////
struct ENGINE_API SState : public xr_resource_flagged
{
	dx10State* state;
	SimulatorStates state_code;
	~SState();
};
typedef resptr_core<SState, resptr_base<SState>> ref_state;

//////////////////////////////////////////////////////////////////////////
struct ENGINE_API SDeclaration : public xr_resource_flagged
{
	//	Maps input signature to input layout
	xr_map<ID3DBlob*, ID3D11InputLayout*> vs_to_layout;
	xr_vector<D3D11_INPUT_ELEMENT_DESC> dx10_dcl_code;

	//	Use this for DirectX10 to cache DX9 declaration for comparison purpose only
	xr_vector<D3DVERTEXELEMENT9> dcl_code;
	~SDeclaration();
};
typedef resptr_core<SDeclaration, resptr_base<SDeclaration>> ref_declaration;

#pragma pack(pop)
#endif // sh_atomicH
