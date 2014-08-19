#pragma once

#include <windows.h>
#include "Data.h"

REG_T GetTopFromYp(EDIT *pMem, HWND hWin, DWORD yp);
REG_T GetCharPtr(EDIT *pMem, DWORD cp, REG_T *pdwCharIndex, REG_T *pdwLineNumber);
REG_T GetCpFromLine(EDIT *pMem, DWORD nLine);
REG_T GetLineFromCp(EDIT *pMem, DWORD cp);
REG_T GetYpFromLine(EDIT *pMem, DWORD nLine);
REG_T GetLineFromYp(EDIT *pMem, DWORD y);
REG_T GetCpFromXp(EDIT *pMem, REG_T lpChars, DWORD x, DWORD fNoAdjust);
REG_T GetPosFromChar(EDIT *pMem, DWORD cp, REG_T lpPoint);
REG_T GetCharFromPos(EDIT *pMem, DWORD cpy, DWORD x, DWORD y);
REG_T GetCaretPoint(EDIT *pMem, DWORD cp, DWORD cpy, REG_T lpPoint);
REG_T SetCaret(EDIT *pMem, DWORD cpy);
REG_T ScrollEdit(EDIT *pMem, HWND hWin, DWORD x, DWORD y);
REG_T InvalidateEdit(EDIT *pMem, HWND hWin);
REG_T InvalidateLine(EDIT *pMem, HWND hWin, DWORD nLine);
REG_T InvalidateSelection(EDIT *pMem, HWND hWin, DWORD cpMin, DWORD cpMax);
REG_T SetCaretVisible(HWND hWin, DWORD cpy);
REG_T GetBlockCp(EDIT *pMem, DWORD nLine, DWORD nPos);
REG_T SetCpxMax(EDIT *pMem, HWND hWin);
REG_T SetBlockFromCp(EDIT *pMem, DWORD cp, DWORD fShift);
