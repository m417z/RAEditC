#pragma once

#include <windows.h>
#include "Data.h"

REG_T InsertNewLine(EDIT *pMem, DWORD nLine, REG_T nSize);
REG_T AddNewLine(EDIT *pMem, REG_T lpLine, REG_T nSize);
REG_T ExpandCharLine(EDIT *pMem);
REG_T DeleteLine(EDIT *pMem, DWORD nLine);
REG_T InsertChar(EDIT *pMem, DWORD cp, DWORD nChr);
REG_T DeleteChar(EDIT *pMem, DWORD cp);
REG_T DeleteSelection(EDIT *pMem, DWORD cpMin, DWORD cpMax);
REG_T DeleteSelectionBlock(EDIT *pMem, DWORD lnMin, DWORD clMin, DWORD lnMax, DWORD clMax);
REG_T EditInsert(EDIT *pMem, DWORD cp, REG_T lpBuff);
