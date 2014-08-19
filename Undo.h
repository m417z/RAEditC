#pragma once

#include <windows.h>
#include "Data.h"

REG_T DoUndo(EDIT *pMem);
REG_T DoRedo(EDIT *pMem);
REG_T SaveUndo(EDIT *pMem, DWORD nFun, DWORD cp, DWORD lp, DWORD cb);
REG_T Undo(RAEDT *raedt, EDIT *pMem, HWND hWin);
REG_T Redo(RAEDT *raedt, EDIT *pMem, HWND hWin);
REG_T GetUndo(EDIT *pMem, DWORD nCount, DWORD lpMem);
REG_T SetUndo(EDIT *pMem, DWORD nSize, DWORD lpMem);
