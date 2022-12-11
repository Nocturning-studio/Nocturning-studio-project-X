#pragma once

#include "xrCore.h"

#ifndef SAFE_ARRAY_DELETE
#define SAFE_ARRAY_DELETE(p)  { if(p) { delete [] (p);  p=NULL; } }
#endif

#ifndef ASSERT_IF_FAILED
#define ASSERT_IF_FAILED( hres )	\
{									\
	if( FAILED(hres) )				\
	   assert( false );				\
}
#endif

#pragma comment(lib,"xrShared.lib")
#pragma comment(lib, "nvtt.lib")
#pragma comment(lib,"FreeImage.lib")
