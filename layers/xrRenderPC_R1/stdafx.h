// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

#pragma once

#pragma warning(disable:4995)
#include "..\xrEngine\stdafx.h"
#pragma warning(disable:4995)
#include <d3dx9.h>
#pragma warning(default:4995)
#pragma warning(disable:4714)
#pragma warning( 4 : 4018 )
#pragma warning( 4 : 4244 )

#define		R_R1	1
#define		R_R2	2
#define		RENDER	R_R1

#include "../../xrCore/xrCore.h"

#include "..\xrEngine\resourcemanager.h"
#include "..\xrEngine\vis_common.h"
#include "..\xrEngine\render.h"
#include "..\xrEngine\igame_level.h"
#include "..\xrEngine\blenders\blender.h"
#include "..\xrEngine\blenders\blender_clsid.h"
#include "..\xrEngine\psystem.h"
#include "..\xrRender\xrRender_console.h"
#include "FStaticRender.h"

#include <d3d11.h>
#include <d3dx11.h>
#include "d3d11shader.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")

#define		TEX_POINT_ATT	"internal\\internal_light_attpoint"
#define		TEX_SPOT_ATT	"internal\\internal_light_attclip"
