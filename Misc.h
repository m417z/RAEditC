#pragma once

#include <windows.h>
#include "Data.h"

REG_T SetFont(EDIT *pMem, DWORD lpRafont);
REG_T SetColor(EDIT *pMem, DWORD lpRAColor);
REG_T DestroyBrushes(EDIT *pMem);
REG_T CreateBrushes(EDIT *pMem);
REG_T DwToAscii(DWORD dwVal, DWORD lpAscii);
REG_T GetChar(EDIT *pMem, DWORD cp);
REG_T IsChar(BYTE ch);
REG_T IsCharLeadByte(EDIT *pMem, DWORD cp);
REG_T GetTextWidth(EDIT *pMem, HDC hDC, DWORD lpText, DWORD nChars, DWORD lpRect);
REG_T GetBlockRange(DWORD lpSrc, DWORD lpDst);
REG_T GetBlockRects(EDIT *pMem, DWORD lpRects);
REG_T InvalidateBlock(EDIT *pMem, DWORD lpOldRects);
