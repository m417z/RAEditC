#pragma once

#include <windows.h>
#include "Data.h"

REG_T SetFont(EDIT *pMem, REG_T lpRafont);
REG_T SetColor(EDIT *pMem, REG_T lpRAColor);
REG_T DestroyBrushes(EDIT *pMem);
REG_T CreateBrushes(EDIT *pMem);
REG_T DwToAscii(DWORD dwVal, REG_T lpAscii);
REG_T GetChar(EDIT *pMem, DWORD cp);
REG_T IsChar(BYTE ch);
REG_T IsCharLeadByte(EDIT *pMem, DWORD cp);
REG_T GetTextWidth(EDIT *pMem, HDC hDC, REG_T lpText, DWORD nChars, REG_T lpRect);
REG_T GetBlockRange(REG_T lpSrc, REG_T lpDst);
REG_T GetBlockRects(EDIT *pMem, REG_T lpRects);
REG_T InvalidateBlock(EDIT *pMem, REG_T lpOldRects);
