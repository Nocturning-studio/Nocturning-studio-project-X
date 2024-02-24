#pragma once
#include "d3dcompiler.h"

class ENGINE_API CShaderMacros
{
public:
	enum Status
	{ 
		Enable	= 'e', // Define: +, Name: definition
		Disable = 'd', // Define: -, Name: d
		Undef	= 'u', // Define: -, Name: u
		Last	= 'l', // Define: -, Name: -
	};

	struct MacroImpl 
	{
		LPCSTR Name;
		LPCSTR Definition;
		Status Status; 
	};

private:
	xr_vector<MacroImpl> macros_impl; // advanced macros
	std::string name; // cache
	xr_vector<D3D_SHADER_MACRO> macros; // cache

public:
	void add(BOOL Enabled, string32 Name, string32 Definition);
	void add(string32 Name, string32 Definition);
	void add(CShaderMacros Macros);
	void undef(string32 Name);
	void clear();

	xr_vector<MacroImpl> get_impl();
	std::string get_name();
	xr_vector<D3D_SHADER_MACRO> get_macros();
};
