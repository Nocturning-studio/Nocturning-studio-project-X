/*----------------------------------------------------------------------
"Debugging Applications" (Microsoft Press)
Copyright (c) 1997-2000 John Robbins -- All rights reserved.
----------------------------------------------------------------------*/

#include "stdafx_.h"
#include "BugslayerUtil.h"
//#include "Internal.h"

/*//////////////////////////////////////////////////////////////////////
                           File Scope Globals
//////////////////////////////////////////////////////////////////////*/
// Indicates that the version information is valid.
static bool g_bHasVersion = false;
// Indicates NT or 95/98.
static bool g_bIsNT = true;

bool __stdcall IsNT ( void )
{
    if (g_bHasVersion )
		return g_bIsNT;
    

    OSVERSIONINFO stOSVI ;

    FillMemory ( &stOSVI , sizeof ( OSVERSIONINFO ), NULL ) ;
    stOSVI.dwOSVersionInfoSize = sizeof ( OSVERSIONINFO ) ;

    bool bRet = GetVersionEx ( &stOSVI );

    ASSERT (bRet) ;

    if (!bRet )
    {
        TRACE0 ( "GetVersionEx failed!\n" ) ;
        return ( false ) ;
    }

    // Check the version and call the appropriate thing.
    if ( VER_PLATFORM_WIN32_NT == stOSVI.dwPlatformId )
		g_bIsNT = true;
	else
		g_bIsNT = false;
    
    g_bHasVersion = true;

    return g_bIsNT;
}
