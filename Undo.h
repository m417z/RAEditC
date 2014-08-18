#pragma once

#include <windows.h>
#include "Data.h"

REG_T DoUndo(DWORD hMem)                                              ;
REG_T DoRedo(DWORD hMem)                                              ;
REG_T SaveUndo(DWORD hMem, DWORD nFun, DWORD cp, DWORD lp, DWORD cb)  ;
REG_T Undo(RAEDT *raedt, DWORD hMem, DWORD hWin)                      ;
REG_T Redo(RAEDT *raedt, DWORD hMem, DWORD hWin)                      ;
REG_T GetUndo(DWORD hMem, DWORD nCount, DWORD lpMem)                  ;
REG_T SetUndo(DWORD hMem, DWORD nSize, DWORD lpMem)                   ;
