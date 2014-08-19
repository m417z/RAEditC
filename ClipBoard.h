#pragma once

#include <windows.h>
#include "Data.h"

REG_T SetClipData(LPSTR lpData, DWORD dwSize);
REG_T EditCopy(DWORD hMem, DWORD lpCMem);
REG_T EditCopyBlock(DWORD hMem, DWORD lpCMem);
REG_T EditCopyNoLF(DWORD hMem, DWORD lpCMem);
REG_T Copy(DWORD hMem);
REG_T EditPaste(DWORD hMem, DWORD hData);
REG_T EditPasteBlock(DWORD hMem, DWORD hData);
REG_T Paste(DWORD hMem, HWND hWin, DWORD hData);
REG_T Cut(DWORD hMem, HWND hWin);
REG_T ConvertCase(DWORD hMem, DWORD nFunction);
REG_T ConvertIndent(DWORD hMem, DWORD nFunction);
