#pragma once

#include <windows.h>
#include "Data.h"

REG_T xGlobalAlloc(DWORD t, REG_T s);
REG_T xHeapAlloc(REG_T h, DWORD t, REG_T s);
REG_T ExpandLineMem(EDIT *pMem);
REG_T GarbageCollection(REG_T lpEdit, REG_T lpLine, REG_T lpSrc, REG_T lpDst);
REG_T ExpandCharMem(EDIT *pMem, REG_T nLen);
REG_T ExpandUndoMem(EDIT *pMem, REG_T cb);
REG_T ExpandWordMem(void);
