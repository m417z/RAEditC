#pragma once

#include <windows.h>
#include "Data.h"

REG_T FindTheText(EDIT *pMem, DWORD pFind, DWORD fMC, DWORD fWW, DWORD fWhiteSpace, DWORD cpMin, DWORD cpMax, DWORD fDir, DWORD *pnIgnore);
REG_T FindTextEx(EDIT *pMem, DWORD fFlag, DWORD lpFindTextEx);
REG_T IsLine(EDIT *pMem, DWORD nLine, DWORD lpszTest);
REG_T SetBookMark(EDIT *pMem, DWORD nLine, DWORD nType);
REG_T GetBookMark(EDIT *pMem, DWORD nLine);
REG_T ClearBookMarks(EDIT *pMem, DWORD nType);
REG_T NextBookMark(EDIT *pMem, DWORD nLine, DWORD nType);
REG_T NextBreakpoint(EDIT *pMem, DWORD nLine);
REG_T NextError(EDIT *pMem, DWORD nLine);
REG_T PreviousBookMark(EDIT *pMem, DWORD nLine, DWORD nType);
REG_T LockLine(EDIT *pMem, DWORD nLine, DWORD fLock);
REG_T IsLineLocked(EDIT *pMem, DWORD nLine);
REG_T HideLine(EDIT *pMem, DWORD nLine, DWORD fHide);
REG_T IsLineHidden(EDIT *pMem, DWORD nLine);
REG_T NoBlockLine(EDIT *pMem, DWORD nLine, DWORD fNoBlock);
REG_T IsLineNoBlock(EDIT *pMem, DWORD nLine);
REG_T AltHiliteLine(EDIT *pMem, DWORD nLine, DWORD fAltHilite);
REG_T IsLineAltHilite(EDIT *pMem, DWORD nLine);
REG_T SetBreakpoint(EDIT *pMem, DWORD nLine, DWORD fBreakpoint);
REG_T SetError(EDIT *pMem, DWORD nLine, DWORD nErrID);
REG_T GetError(EDIT *pMem, DWORD nLine);
REG_T SetRedText(EDIT *pMem, DWORD nLine, DWORD fRed);
REG_T GetLineState(EDIT *pMem, DWORD nLine);
REG_T IsSelectionLocked(EDIT *pMem, DWORD cpMin, DWORD cpMax);
REG_T TrimSpace(EDIT *pMem, DWORD nLine, DWORD fLeft);
REG_T SkipSpace(EDIT *pMem, DWORD cp, DWORD fLeft);
REG_T SkipWhiteSpace(EDIT *pMem, DWORD cp, DWORD fLeft);
REG_T GetWordStart(EDIT *pMem, DWORD cp, DWORD nType);
REG_T GetLineStart(EDIT *pMem, DWORD cp);
REG_T GetTabPos(EDIT *pMem, DWORD cp);
REG_T GetWordEnd(EDIT *pMem, DWORD cp, DWORD nType);
REG_T GetLineEnd(EDIT *pMem, DWORD cp);
REG_T StreamIn(EDIT *pMem, DWORD lParam);
REG_T StreamOut(EDIT *pMem, DWORD lParam);
REG_T HiliteLine(EDIT *pMem, DWORD nLine, DWORD nColor);
REG_T SelChange(EDIT *pMem, DWORD nType);
REG_T AutoIndent(EDIT *pMem);
REG_T IsCharPos(EDIT *pMem, DWORD cp);
REG_T BracketMatchRight(EDIT *pMem, DWORD nChr, DWORD nMatch, DWORD cp);
REG_T BracketMatchLeft(EDIT *pMem, DWORD nChr, DWORD nMatch, DWORD cp);
REG_T BracketMatch(EDIT *pMem, DWORD nChr, DWORD cp);
REG_T GetLineBegin(EDIT *pMem, DWORD nLine);
