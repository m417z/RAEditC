#pragma once

#include <windows.h>
#include <commctrl.h>
#include "Data.h"

REG_T DrawLine(EDIT *pMem, REG_T lpChars, DWORD nLine, DWORD cp, REG_T hDC, REG_T lpRect);
REG_T SetBlockMarkers(EDIT *pMem, DWORD nLine, DWORD nMax);
REG_T DrawChangedState(EDIT *pMem, HDC hDC, REG_T lpLine, DWORD x, DWORD y);
REG_T RAEditPaint(HWND hWin);
REG_T RAEditPaintNoBuff(HWND hWin);
