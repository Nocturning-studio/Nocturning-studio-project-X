#ifndef AFX_STDAFX_H__
#define AFX_STDAFX_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers

#include "PSAPI.h"
#include "WarningsOff.h"
#include "WarningsOn.h"
#include <stdlib.h>
#include <tchar.h>
#include <windows.h>

namespace BlackBox
{

bool isspace(int ch);

bool isdigit(int ch);

long atol(const char *nptr);

}; // namespace BlackBox

#ifdef _EDITOR
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif // _EDITOR

#endif //
