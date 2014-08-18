#pragma once

#include <windows.h>
#include "Data.h"

REG_T InsertNewLine(DWORD hMem, DWORD nLine, DWORD nSize)                                  ;
REG_T AddNewLine(DWORD hMem, DWORD lpLine, DWORD nSize)                                    ;
REG_T ExpandCharLine(DWORD hMem)                                                           ;
REG_T DeleteLine(DWORD hMem, DWORD nLine)                                                  ;
REG_T InsertChar(DWORD hMem, DWORD cp, DWORD nChr)                                         ;
REG_T DeleteChar(DWORD hMem, DWORD cp)                                                     ;
REG_T DeleteSelection(DWORD hMem, DWORD cpMin, DWORD cpMax)                                ;
REG_T DeleteSelectionBlock(DWORD hMem, DWORD lnMin, DWORD clMin, DWORD lnMax, DWORD clMax) ;
REG_T EditInsert(DWORD hMem, DWORD cp, DWORD lpBuff)                                       ;
