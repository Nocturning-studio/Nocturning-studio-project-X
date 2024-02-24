#include "stdafx.h"

#include "ShaderMacros.h"

void CShaderMacros::add(BOOL Enabled, string32 Name, string32 Definition)
{
	MacroImpl macro;
	macro.Name = Name;
	macro.Definition = Definition;
	if (Enabled)	macro.Status = Enable;
	else			macro.Status = Disable;
	if (!Name)		macro.Status = Last;
	macros_impl.push_back(macro);
}

void CShaderMacros::add(string32 Name, string32 Definition)
{
	add(TRUE, Name, Definition);
}

void CShaderMacros::add(CShaderMacros Macros)
{
	for (u32 i = 0; i < Macros.macros_impl.size(); i++)
	{
		MacroImpl macro = Macros.macros_impl[i];
		macros_impl.push_back(macro);
	}
}

void CShaderMacros::undef(string32 Name)
{
	for (u32 i = 0; i < macros_impl.size(); i++)
	{
		if (xr_strcmp(Name, macros_impl[i].Name) == 0)
		{
			macros_impl[i].Status = Undef;
			break;
		}
	}
}

void CShaderMacros::clear()
{
	macros_impl.clear();
}

xr_vector<CShaderMacros::MacroImpl> CShaderMacros::get_impl()
{
	return macros_impl;
}

std::string CShaderMacros::get_name()
{
	name.clear();

	for (u32 i = 0; i < macros_impl.size(); i++)
	{
		if (macros_impl[i].Status == Enable)
		{
			name += macros_impl[i].Definition;
		}
		else
		{
			for (u32 j = 0; j < strnlen_s(macros_impl[i].Definition, 128); j++)
			{
				name += macros_impl[i].Status;
			}
		}
	}

	return name;
}

xr_vector<D3D_SHADER_MACRO> CShaderMacros::get_macros()
{
	macros.clear();

	for (u32 i = 0; i < macros_impl.size(); i++)
	{
		if (macros_impl[i].Status == Enable || macros_impl[i].Status == Last)
		{
			D3D_SHADER_MACRO macro = {macros_impl[i].Name, macros_impl[i].Definition};
			macros.push_back(macro);
		}
	}

	return macros;
}