#pragma once

#include <windows.h>
#include "Data.h"

REG_T xGlobalAlloc(DWORD t, DWORD s);
REG_T xHeapAlloc(DWORD h, DWORD t, DWORD s);
REG_T ExpandLineMem(EDIT *pMem);
REG_T GarbageCollection(DWORD lpEdit, DWORD lpLine, DWORD lpSrc, DWORD lpDst);
REG_T ExpandCharMem(EDIT *pMem, DWORD nLen);
REG_T ExpandUndoMem(EDIT *pMem, DWORD cb);
REG_T ExpandWordMem(void);
