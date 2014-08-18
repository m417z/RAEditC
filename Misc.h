#pragma once

#include <windows.h>
#include "Data.h"

REG_T SetFont(DWORD hMem, DWORD lpRafont)                                            ;
REG_T SetColor(DWORD hMem, DWORD lpRAColor)                                          ;
REG_T DestroyBrushes(DWORD hMem)                                                     ;
REG_T CreateBrushes(DWORD hMem)                                                      ;
REG_T DwToAscii(DWORD dwVal, DWORD lpAscii)                                          ;
REG_T GetChar(DWORD hMem, DWORD cp)                                                  ;
REG_T IsChar(void)                                                                   ;
REG_T IsCharLeadByte(DWORD hMem, DWORD cp)                                           ;
REG_T GetTextWidth(DWORD hMem, HDC hDC, DWORD lpText, DWORD nChars, DWORD lpRect)    ;
REG_T GetBlockRange(DWORD lpSrc, DWORD lpDst)                                        ;
REG_T GetBlockRects(DWORD hMem, DWORD lpRects)                                       ;
REG_T InvalidateBlock(DWORD hMem, DWORD lpOldRects)                                  ;
