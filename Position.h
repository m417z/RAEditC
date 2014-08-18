#pragma once

#include <windows.h>
#include "Data.h"

REG_T GetTopFromYp(DWORD hMem, DWORD hWin, DWORD yp);
REG_T GetCharPtr(DWORD hMem, DWORD cp, DWORD *pdwCharIndex, DWORD *pdwLineNumber);
REG_T GetCpFromLine(DWORD hMem, DWORD nLine);
REG_T GetLineFromCp(DWORD hMem, DWORD cp);
REG_T GetYpFromLine(DWORD hMem, DWORD nLine);
REG_T GetLineFromYp(DWORD hMem, DWORD y);
REG_T GetCpFromXp(DWORD hMem, DWORD lpChars, DWORD x, DWORD fNoAdjust);
REG_T GetPosFromChar(DWORD hMem, DWORD cp, DWORD lpPoint);
REG_T GetCharFromPos(DWORD hMem, DWORD cpy, DWORD x, DWORD y);
REG_T GetCaretPoint(DWORD hMem, DWORD cp, DWORD cpy, DWORD lpPoint);
REG_T SetCaret(DWORD hMem, DWORD cpy);
REG_T ScrollEdit(DWORD hMem, DWORD hWin, DWORD x, DWORD y);
REG_T InvalidateEdit(DWORD hMem, DWORD hWin);
REG_T InvalidateLine(DWORD hMem, DWORD hWin, DWORD nLine);
REG_T InvalidateSelection(DWORD hMem, DWORD hWin, DWORD cpMin, DWORD cpMax);
REG_T SetCaretVisible(DWORD hWin, DWORD cpy);
REG_T GetBlockCp(DWORD hMem, DWORD nLine, DWORD nPos);
REG_T SetCpxMax(DWORD hMem, DWORD hWin);
REG_T SetBlockFromCp(DWORD hMem, DWORD cp, DWORD fShift);
