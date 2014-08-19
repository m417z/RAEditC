#pragma once

#include <windows.h>
#include <commctrl.h>
#include "Data.h"

REG_T DrawLine(EDIT *pMem, DWORD lpChars, DWORD nLine, DWORD cp, DWORD hDC, DWORD lpRect);
REG_T SetBlockMarkers(EDIT *pMem, DWORD nLine, DWORD nMax);
REG_T DrawChangedState(EDIT *pMem, HDC hDC, DWORD lpLine, DWORD x, DWORD y);
REG_T RAEditPaint(HWND hWin);
REG_T RAEditPaintNoBuff(HWND hWin);
