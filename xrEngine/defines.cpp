#include "stdafx.h"

#ifdef DEBUG
ECORE_API BOOL bDebug = FALSE;
#endif

// Video
//. u32			psCurrentMode		= 1024;
u32			psCurrentVidMode[2] = { 1024,768 };
u32			psCurrentBPP = 32;
// release version always has "mt_*" enabled
Flags32		psDeviceFlags = { rsFullscreen | rsDetails | mtPhysics | mtSound | mtNetwork | rsDrawStatic | rsDrawDynamic };

u32	psWindowMode = 3;
xr_token WindowMode_token[] = 
{
	{ "window",				1 },
	{ "borderless",			2 },
	{ "fullscreen",			3 },
	{ 0,					0 }
};

// textures
int			psTextureLOD = 0;