#pragma once

#include <windows.h>
#include "Data.h"

REG_T SetClipData(LPSTR lpData, DWORD dwSize);
REG_T EditCopy(EDIT *pMem, DWORD lpCMem);
REG_T EditCopyBlock(EDIT *pMem, DWORD lpCMem);
REG_T EditCopyNoLF(EDIT *pMem, DWORD lpCMem);
REG_T Copy(EDIT *pMem);
REG_T EditPaste(EDIT *pMem, DWORD hData);
REG_T EditPasteBlock(EDIT *pMem, DWORD hData);
REG_T Paste(EDIT *pMem, HWND hWin, DWORD hData);
REG_T Cut(EDIT *pMem, HWND hWin);
REG_T ConvertCase(EDIT *pMem, DWORD nFunction);
REG_T ConvertIndent(EDIT *pMem, DWORD nFunction);
