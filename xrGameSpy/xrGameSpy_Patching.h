#pragma once
#include "GameSpy/Patching/pt.h"
#include "xrGameSpy_MainDefs.h"

extern "C"
{
    EXPORT_FN_DECL(bool, ptCheckForPatch,
                   (
                       //		int productID,  const gsi_char * versionUniqueID,  int distributionID,
                       ptPatchCallback callback, PTBool blocking, void *instance));
}
