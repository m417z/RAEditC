#include <windows.h>
#include <commctrl.h>
#include "Data.h"

REG_T TimerProc(HWND hWin, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
REG_T SetHiliteWords(DWORD nColor, REG_T lpWords);
REG_T GetCharTabPtr(void);
REG_T GetCharTabVal(DWORD nChar);
REG_T SetCharTabVal(DWORD nChar, DWORD nValue);
REG_T SetBlockDef(REG_T lpRABLOCKDEF);
REG_T SplittBtnProc(HWND hWin, UINT uMsg, WPARAM wParam, LPARAM lParam);
REG_T StateProc(HWND hWin, UINT uMsg, WPARAM wParam, LPARAM lParam);
REG_T EditFunc(HWND hWin, UINT uMsg, DWORD fAlt, DWORD fShift, DWORD fControl);
REG_T RAEditProc(HWND hWin, UINT uMsg, WPARAM wParam, LPARAM lParam);
REG_T GetText(EDIT *pMem, DWORD cpMin, DWORD cpMax, REG_T lpText, DWORD fLf);
REG_T FakeToolTipProc(HWND hWin, UINT uMsg, WPARAM wParam, LPARAM lParam);
REG_T ConvTwipsToPixels(HDC hDC, DWORD fHorz, DWORD lSize);
REG_T RAWndProc(HWND hWin, UINT uMsg, WPARAM wParam, LPARAM lParam);
