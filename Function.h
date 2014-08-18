#pragma once

#include <windows.h>
#include "Data.h"

REG_T FindTheText(DWORD hMem, DWORD pFind, DWORD fMC, DWORD fWW, DWORD fWhiteSpace, DWORD cpMin, DWORD cpMax, DWORD fDir, DWORD *pnIgnore);
REG_T FindTextEx(DWORD hMem, DWORD fFlag, DWORD lpFindTextEx)                                                            ;
REG_T IsLine(DWORD hMem, DWORD nLine, DWORD lpszTest)                                                                    ;
REG_T SetBookMark(DWORD hMem, DWORD nLine, DWORD nType)                                                                  ;
REG_T GetBookMark(DWORD hMem, DWORD nLine)                                                                               ;
REG_T ClearBookMarks(DWORD hMem, DWORD nType)                                                                            ;
REG_T NextBookMark(DWORD hMem, DWORD nLine, DWORD nType)                                                                 ;
REG_T NextBreakpoint(DWORD hMem, DWORD nLine)                                                                            ;
REG_T NextError(DWORD hMem, DWORD nLine)                                                                                 ;
REG_T PreviousBookMark(DWORD hMem, DWORD nLine, DWORD nType)                                                             ;
REG_T LockLine(DWORD hMem, DWORD nLine, DWORD fLock)                                                                     ;
REG_T IsLineLocked(DWORD hMem, DWORD nLine)                                                                              ;
REG_T HideLine(DWORD hMem, DWORD nLine, DWORD fHide)                                                                     ;
REG_T IsLineHidden(DWORD hMem, DWORD nLine)                                                                              ;
REG_T NoBlockLine(DWORD hMem, DWORD nLine, DWORD fNoBlock)                                                               ;
REG_T IsLineNoBlock(DWORD hMem, DWORD nLine)                                                                             ;
REG_T AltHiliteLine(DWORD hMem, DWORD nLine, DWORD fAltHilite)                                                           ;
REG_T IsLineAltHilite(DWORD hMem, DWORD nLine)                                                                           ;
REG_T SetBreakpoint(DWORD hMem, DWORD nLine, DWORD fBreakpoint)                                                          ;
REG_T SetError(DWORD hMem, DWORD nLine, DWORD nErrID)                                                                    ;
REG_T GetError(DWORD hMem, DWORD nLine)                                                                                  ;
REG_T SetRedText(DWORD hMem, DWORD nLine, DWORD fRed)                                                                    ;
REG_T GetLineState(DWORD hMem, DWORD nLine)                                                                              ;
REG_T IsSelectionLocked(DWORD hMem, DWORD cpMin, DWORD cpMax)                                                            ;
REG_T TrimSpace(DWORD hMem, DWORD nLine, DWORD fLeft)                                                                    ;
REG_T SkipSpace(DWORD hMem, DWORD cp, DWORD fLeft)                                                                       ;
REG_T SkipWhiteSpace(DWORD hMem, DWORD cp, DWORD fLeft)                                                                  ;
REG_T GetWordStart(DWORD hMem, DWORD cp, DWORD nType)                                                                    ;
REG_T GetLineStart(DWORD hMem, DWORD cp)                                                                                 ;
REG_T GetTabPos(DWORD hMem, DWORD cp)                                                                                    ;
REG_T GetWordEnd(DWORD hMem, DWORD cp, DWORD nType)                                                                      ;
REG_T GetLineEnd(DWORD hMem, DWORD cp)                                                                                   ;
REG_T StreamIn(DWORD hMem, DWORD lParam)                                                                                 ;
REG_T StreamOut(DWORD hMem, DWORD lParam)                                                                                ;
REG_T HiliteLine(DWORD hMem, DWORD nLine, DWORD nColor)                                                                  ;
REG_T SelChange(DWORD hMem, DWORD nType)                                                                                 ;
REG_T AutoIndent(DWORD hMem)                                                                                             ;
REG_T IsCharPos(DWORD hMem, DWORD cp)                                                                                    ;
REG_T BracketMatchRight(DWORD hMem, DWORD nChr, DWORD nMatch, DWORD cp)                                                  ;
REG_T BracketMatchLeft(DWORD hMem, DWORD nChr, DWORD nMatch, DWORD cp)                                                   ;
REG_T BracketMatch(DWORD hMem, DWORD nChr, DWORD cp)                                                                     ;
REG_T GetLineBegin(DWORD hMem, DWORD nLine)                                                                              ;
