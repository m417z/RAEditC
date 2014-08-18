#pragma once

#include <windows.h>
#include <commctrl.h>
#include "Data.h"

REG_T DrawLine(DWORD hMem, DWORD lpChars, DWORD nLine, DWORD cp, DWORD hDC, DWORD lpRect)  ;
REG_T SetBlockMarkers(DWORD hMem, DWORD nLine, DWORD nMax)                                 ;
REG_T DrawChangedState(DWORD hMem, HDC hDC, DWORD lpLine, DWORD x, DWORD y)                ;
REG_T RAEditPaint(HWND hWin)                                                               ;
REG_T RAEditPaintNoBuff(HWND hWin)                                                         ;
