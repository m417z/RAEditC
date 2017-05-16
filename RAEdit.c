#include "RAEdit.h"

#include "ClipBoard.h"
#include "Edit.h"
#include "Block.h"
#include "DragDrop.h"
#include "Function.h"
#include "Memory.h"
#include "Misc.h"
#include "Paint.h"
#include "Position.h"
#include "Undo.h"

#define EM_SETIMESTATUS 0x00d8
#define EM_GETIMESTATUS 0x00d9

REG_T TimerProc(HWND hWin, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	REG_T eax = 0;

	if(tmr1.hwnd)
	{
		eax = PostMessage(tmr1.hwnd, tmr1.umsg, tmr1.wparam, tmr1.lparam);
		tmr1.hwnd = 0;
	} // endif
	if(tmr2.hwnd)
	{
		eax = PostMessage(tmr2.hwnd, tmr2.umsg, tmr2.wparam, tmr2.lparam);
		tmr2.hwnd = 0;
	} // endif
	eax = KillTimer(NULL, TimerID);
	return eax;

} // TimerProc

// Create a windowclass for the user control
void WINAPI InstallRAEdit(HINSTANCE hInst, BOOL fGlobal)
{
	REG_T eax = 0, ebx;
	REG_T temp1;
	WNDCLASSEX wc;
	REG_T hBmp;

	temp1 = hInst;
	hInstance = temp1;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	if(fGlobal)
	{
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_GLOBALCLASS;
	} // endif
	wc.lpfnWndProc = RAWndProc;
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = sizeof(ULONG_PTR); // Holds memory handle
	temp1 = hInst;
	wc.hInstance = temp1;
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = szRAEditClass;
	eax = NULL;
	wc.hIcon = eax;
	wc.hIconSm = eax;
	eax = LoadCursor(NULL, IDC_ARROW);
	wc.hCursor = eax;
	eax = RegisterClassEx(&wc);

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = RAEditProc;
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = sizeof(ULONG_PTR); // Holds memory handle
	temp1 = hInst;
	wc.hInstance = temp1;
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = szEditClassName;
	eax = NULL;
	wc.hIcon = eax;
	wc.hIconSm = eax;
	eax = LoadCursor(NULL, IDC_IBEAM);
	wc.hCursor = eax;
	eax = RegisterClassEx(&wc);
	eax = LoadCursor(hInst, IDC_HSPLITTCUR);
	hHSCur = eax;
	eax = LoadCursor(hInstance, IDC_SELECTCUR);
	hSelCur = eax;
	eax = LoadBitmap(hInstance, IDB_BOOKMARK);
	hBmp = eax;
	eax = ImageList_Create(11, 11, ILC_COLOR4 | ILC_MASK, 7, 7);
	hIml = eax;
	eax = ImageList_AddMasked(hIml, hBmp, 0x0C0C0C0);
	eax = DeleteObject(hBmp);
	eax = LoadBitmap(hInstance, IDB_LINENUMBER);
	hBmpLnr = eax;
	eax = LoadBitmap(hInstance, IDB_EXPAND);
	hBmpExp = eax;
	eax = LoadBitmap(hInstance, IDB_COLLAPSE);
	hBmpCol = eax;
	eax = LoadBitmap(hInstance, IDB_LOCK);
	hBmpLck = eax;
	eax = GetSysColor(COLOR_INFOBK);
	eax = CreateSolidBrush(eax);
	hBrTlt = eax;
	// Allocate memory for the word list
	eax = GetProcessHeap();
	eax = xHeapAlloc(eax, HEAP_ZERO_MEMORY, MAXWORDMEM);
	hWrdMem = eax;
	cbWrdMem = MAXWORDMEM;
	rpWrdFree = 256*4;
	// Setup upper CharTab
	ebx = 128;
	while(ebx<256)
	{
		eax = IsCharAlphaNumeric(ebx);
		CharTab[ebx] = RBYTE_LOW(eax);
		ebx++;
	} // endw
	// Setup whole CaseTab
	ebx = 0;
	while(ebx<256)
	{
		eax = IsCharAlpha(ebx);
		if(eax)
		{
			eax = CharUpper(ebx);
			if(eax==ebx)
			{
				eax = CharLower(ebx);
			} // endif
			CaseTab[ebx] = RBYTE_LOW(eax);
		}
		else
		{
			CaseTab[ebx] = RBYTE_LOW(ebx);
		} // endif
		ebx++;
	} // endw
	return;

} // InstallRAEdit

void WINAPI UnInstallRAEdit(void)
{
	REG_T eax = 0;

	DestroyCursor(hHSCur);
	DestroyCursor(hSelCur);
	ImageList_Destroy(hIml);
	DeleteObject(hBmpLnr);
	DeleteObject(hBmpCol);
	DeleteObject(hBmpExp);
	DeleteObject(hBmpLck);
	DeleteObject(hBrTlt);
	eax = GetProcessHeap();
	HeapFree(eax, 0, hWrdMem);
	return;

} // UnInstallRAEdit

// Prefix ^		Word is case sensitive
// Prefix ~		Word is case converted
// Suffix +		Hilites rest of line with comment color
// Suffix -		Hilites rest of line with text color
//
// nColor			gggg0sff cccccccc cccccccc cccccccc
// g=Word group, s=Case sensitive, f=Font style, c=color
REG_T SetHiliteWords(DWORD nColor, REG_T lpWords)
{
	REG_T eax = 0, ecx, edx, esi, edi;
	REG_T temp1;
	DWORD fEnd;
	DWORD fEnd2;
	DWORD len;

	auto void GetLen(void);
	auto void TestWord(void);

	esi = lpWords;
	if(!esi)
	{
		edi = hWrdMem;
		ecx = cbWrdMem;
		ecx /= 4;
		eax = 0;
		while(ecx > 0)
		{
			*(DWORD *)edi = eax;
			edi += 4;
			ecx--;
		}
		rpWrdFree = 256*4;
	}
	else
	{
NxtWrd:
		fEnd = 0;
		if(((BYTE *)(&nColor))[3] & 4)
		{
			// group is case sensitive. Toggles meaning of '^'
			fEnd |= 3;
		} // endif
		eax = ExpandWordMem();
		edi = hWrdMem;
NxtWrd1:
		eax = *(BYTE *)esi;
		if(eax)
		{
			if(eax==' ' || eax==VK_TAB || eax==VK_RETURN || eax==0x0A)
			{
				esi++;
				goto NxtWrd;
			} // endif
			if(RBYTE_LOW(eax)>='a' && RBYTE_LOW(eax)<='z')
			{
				RBYTE_LOW(eax) &= 0x5F;
			}
			else if(RBYTE_LOW(eax)=='^')
			{
				fEnd ^= 3;
				esi++;
				goto NxtWrd1;
			}
			else if(RBYTE_LOW(eax)=='~')
			{
				fEnd = 4;
				esi++;
				goto NxtWrd1;
			} // endif
			temp1 = eax;
			TestWord();
			ecx = eax;
			eax = temp1;
			if(ecx!=0)
			{
				goto NxtWrd;
			} // endif
			// pointer to previous
			edx = *(DWORD *)(edi+eax*4);
			// pointer to free
			ecx = rpWrdFree;
			*(DWORD *)(edi+eax*4) = ecx;
NxtChar:
			RBYTE_LOW(eax) = *(BYTE *)esi;
			esi++;
			if(RBYTE_LOW(eax)==' ' || RBYTE_LOW(eax)==VK_TAB || RBYTE_LOW(eax)==VK_RETURN || RBYTE_LOW(eax)==0x0A || !RBYTE_LOW(eax))
			{
				if(!RBYTE_LOW(eax))
				{
					esi--;
				} // endif
				*(BYTE *)(edi+ecx+sizeof(WORDINFO)) = 0;
				eax = ecx;
				ecx = rpWrdFree;
				eax -= ecx;
				((WORDINFO *)(edi+ecx))->len = eax;
				eax += sizeof(WORDINFO)+1;
				rpWrdFree += eax;
				((WORDINFO *)(edi+ecx))->rpprev = edx;
				eax = nColor;
				((WORDINFO *)(edi+ecx))->color = eax;
				eax = fEnd;
				((WORDINFO *)(edi+ecx))->fend = eax;
				goto NxtWrd;
			} // endif
			if(RBYTE_LOW(eax)=='-')
			{
				fEnd |= 1;
			}
			else if(RBYTE_LOW(eax)=='+')
			{
				fEnd |= 2;
			}
			else
			{
				*(BYTE *)(edi+ecx+sizeof(WORDINFO)) = RBYTE_LOW(eax);
				ecx++;
			} // endif
			goto NxtChar;
		} // endif
	} // endif
	return eax;

	void GetLen(void)
	{
		ecx = 0;
anon_1:
		RBYTE_LOW(eax) = *(BYTE *)(esi+ecx);
		if(RBYTE_LOW(eax)>' ')
		{
			ecx++;
			goto anon_1;
		} // endif
		len = ecx;
		return;

	}

	void TestWord(void)
	{
		REG_T temp1, temp2;
		temp1 = edi;
		GetLen();
		eax = *(BYTE *)esi;
		if(eax>='a' && eax<='z' && fEnd!=3)
		{
			eax &= 0x5F;
		} // endif
		edi = hWrdMem;
		edx = *(DWORD *)(edi+eax*4);
		eax = 0;
		if(edx==0)
		{
			goto Ex;
		} // endif
anon_2:
		eax = ((WORDINFO *)(edi+edx))->fend;
		fEnd2 = eax;
		eax = ((WORDINFO *)(edi+edx))->color;
		eax >>= 28;
		ecx = nColor;
		ecx >>= 28;
		if(eax!=ecx)
		{
			edx = ((WORDINFO *)(edi+edx))->rpprev;
			if(edx)
			{
				goto anon_2;
			}
			else
			{
				eax = 0;
				goto Ex;
			} // endif
		} // endif
		eax = ((WORDINFO *)(edi+edx))->len;
		ecx = len;
		if(eax!=ecx)
		{
			edx = ((WORDINFO *)(edi+edx))->rpprev;
			if(edx)
			{
				goto anon_2;
			}
			else
			{
				eax = 0;
				goto Ex;
			} // endif
		} // endif
		temp2 = edx;
		ecx = len;
		edx = edi+edx+sizeof(WORDINFO);
		eax = 0;
		while(ecx && !eax)
		{
			ecx--;
			RBYTE_LOW(eax) = *(BYTE *)(esi+ecx);
			RBYTE_HIGH(eax) = *(BYTE *)(edx+ecx);
			if(fEnd!=3)
			{
				if(RBYTE_LOW(eax)>='a' && RBYTE_LOW(eax)<='z')
				{
					RBYTE_LOW(eax) &= 0x5f;
				} // endif
			} // endif
			if(fEnd2!=3)
			{
				if(RBYTE_HIGH(eax)>='a' && RBYTE_HIGH(eax)<='z')
				{
					RBYTE_HIGH(eax) &= 0x5f;
				} // endif
			} // endif
			RBYTE_LOW(eax) -= RBYTE_HIGH(eax);
			RBYTE_HIGH(eax) = 0;
		} // endw
		edx = temp2;
		if(!eax)
		{
			eax++;
			esi += len;
			goto Ex;
		} // endif
		edx = ((WORDINFO *)(edi+edx))->rpprev;
		if(edx)
		{
			goto anon_2;
		}
		else
		{
			eax = 0;
			goto Ex;
		} // endif
Ex:
		edi = temp1;
		return;

	}

} // SetHiliteWords

REG_T GetCharTabPtr(void)
{
	REG_T eax = 0;

	eax = CharTab;
	return eax;

} // GetCharTabPtr

REG_T GetCharTabVal(DWORD nChar)
{
	REG_T eax = 0, edx;

	edx = nChar;
	edx &= 0x0FF;
	eax = *(BYTE *)(edx+CharTab);
	return eax;

} // GetCharTabVal

REG_T SetCharTabVal(DWORD nChar, DWORD nValue)
{
	REG_T eax = 0, edx;

	edx = nChar;
	edx &= 0x0FF;
	eax = nValue;
	*(BYTE *)(edx+CharTab) = RBYTE_LOW(eax);
	return eax;

} // SetCharTabVal

REG_T SetBlockDef(REG_T lpRABLOCKDEF)
{
	REG_T eax = 0, ecx, ebx, esi, edi;
	REG_T temp1, temp2;

	auto void TestString(void);

	if(!lpRABLOCKDEF)
	{
		memset(blockdefs, 0, sizeof(blockdefs));
	}
	else
	{
		ebx = lpRABLOCKDEF;
		esi = blockdefs;
		edi = esi+32*sizeof(REG_T);
		while(*(REG_T *)esi)
		{
			eax = IsBlockDefEqual(ebx, edi);
			if(eax)
			{
				goto Ex;
			} // endif
			edi = *(REG_T *)esi;
			esi += sizeof(REG_T);
		} // endw
		ecx = edi+sizeof(RABLOCKDEF);
		eax = ((RABLOCKDEF *)ebx)->flag;
		((RABLOCKDEF *)edi)->flag = eax;
		eax = ((RABLOCKDEF *)ebx)->lpszStart;
		if(eax)
		{
			((RABLOCKDEF *)edi)->lpszStart = ecx;
			temp1 = ecx;
			temp2 = eax;
			eax = lstrcpy(ecx, eax);
			eax = temp2;
			eax = strlen(eax);
			ecx = temp1;

			TestString();
			eax++;
			*(BYTE *)(ecx+eax) = 0;

			ecx = ecx+eax+1;
		} // endif
		eax = ((RABLOCKDEF *)ebx)->lpszEnd;
		if(eax)
		{
			((RABLOCKDEF *)edi)->lpszEnd = ecx;
			temp1 = ecx;
			temp2 = eax;
			eax = lstrcpy(ecx, eax);
			eax = temp2;
			eax = strlen(eax);
			ecx = temp1;
			TestString();
			eax++;
			*(BYTE *)(ecx+eax) = 0;
			ecx = ecx+eax+1;
		} // endif
		eax = ((RABLOCKDEF *)ebx)->lpszNot1;
		if(eax)
		{
			((RABLOCKDEF *)edi)->lpszNot1 = ecx;
			temp1 = ecx;
			temp2 = eax;
			eax = lstrcpy(ecx, eax);
			eax = temp2;
			eax = strlen(eax);
			ecx = temp1;
			ecx = ecx+eax+1;
		} // endif
		eax = ((RABLOCKDEF *)ebx)->lpszNot2;
		if(eax)
		{
			((RABLOCKDEF *)edi)->lpszNot2 = ecx;
			temp1 = ecx;
			temp2 = eax;
			eax = lstrcpy(ecx, eax);
			eax = temp2;
			eax = strlen(eax);
			ecx = temp1;
			ecx = ecx+eax+1;
		} // endif
		*(REG_T *)esi = ecx;
	} // endif
Ex:
	return eax;

	void TestString(void)
	{
		REG_T temp1;
		temp1 = ecx;
		while(*(BYTE *)ecx)
		{
			if(*(BYTE *)ecx=='|')
			{
				*(BYTE *)ecx = 0;
			} // endif
			ecx++;
		} // endw
		ecx = temp1;
		return;

	}

} // SetBlockDef

// --------------------------------------------------------------------------------

REG_T SplittBtnProc(HWND hWin, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	REG_T eax = 0, edx;
	REG_T temp1;
	EDIT *pMem;

	eax = uMsg;
	if(eax==WM_MOUSEMOVE)
	{
		eax = GetParent(hWin);
		eax = GetWindowLongPtr(eax, 0);
		pMem = eax;
		eax = SetCursor(hHSCur);
	}
	else if(eax==WM_LBUTTONDOWN)
	{
		eax = GetParent(hWin);
		eax = GetWindowLongPtr(eax, 0);
		pMem = eax;
		pMem->fresize = 1;
		eax = SetCapture(pMem->hwnd);
		eax = SetCursor(hHSCur);
	}
	else if(eax==WM_LBUTTONDBLCLK)
	{
		eax = GetParent(hWin);
		temp1 = eax;
		eax = GetWindowLongPtr(eax, 0);
		pMem = eax;
		eax = 511;
		if(pMem->fsplitt)
		{
			eax = 0;
		} // endif
		edx = temp1;
		eax = SendMessage(edx, REM_SETSPLIT, eax, 0);
		eax = 0;
		return eax;
	} // endif
	eax = CallWindowProc(OldSplittBtnProc, hWin, uMsg, wParam, lParam);
	return eax;

} // SplittBtnProc

REG_T StateProc(HWND hWin, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	REG_T eax = 0;
	PAINTSTRUCT ps;
	REG_T hBr;
	EDIT *pMem;

	if(uMsg==WM_PAINT)
	{
		eax = BeginPaint(hWin, &ps);
		eax = GetWindowLongPtr(hWin, GWLP_USERDATA);
		pMem = eax;
		if(pMem->fstyle&STYLE_READONLY)
		{
			eax = CreateSolidBrush(0x0FF);
			hBr = eax;
		}
		else if(pMem->fChanged)
		{
			eax = CreateSolidBrush(0x0B000);
			hBr = eax;
		}
		else
		{
			hBr = 0;
			eax = pMem->br.hBrSelBar;
		} // endif
		eax = FillRect(ps.hdc, &ps.rcPaint, eax);
		if(hBr)
		{
			eax = DeleteObject(hBr);
		} // endif
		eax = EndPaint(hWin, &ps);
		eax = 0;
		return eax;
	} // endif
	eax = CallWindowProc(OldStateProc, hWin, uMsg, wParam, lParam);
	return eax;

} // StateProc

REG_T EditFunc(HWND hWin, UINT uMsg, DWORD fAlt, DWORD fShift, DWORD fControl)
{
	REG_T eax = 0, ecx, edx, esi;
	REG_T temp1, temp2;
	POINT pt;
	DWORD cpOldMin;
	DWORD cpOldMax;
	RECT oldrects[2];
	DWORD nOldLine;
	DWORD fSel;
	EDIT *pMem;

	eax = GetWindowLongPtr(hWin, 0);
	pMem = eax;
	eax = hWin;
	if(eax==pMem->edta.hwnd)
	{
		esi = &pMem->edta;
	}
	else
	{
		esi = &pMem->edtb;
	} // endif
	eax = pMem->cpMin;
	cpOldMin = eax;
	edx = pMem->cpMax;
	cpOldMax = edx;
	edx -= eax;
	fSel = edx;
	eax = uMsg;
	eax = RWORD(eax);
	if(eax==CMD_LINE_UP)
	{
		if(fAlt)
		{
			if(fControl && !fShift)
			{
				// Up, size split
				if(pMem->fsplitt)
				{
					if(pMem->fsplitt>10)
					{
						pMem->fsplitt -= 10;
					}
					else
					{
						pMem->fsplitt = 0;
					} // endif
					eax = SendMessage(pMem->hwnd, WM_SIZE, 0, 0);
					eax = UpdateWindow(pMem->edta.hwnd);
					eax = UpdateWindow(pMem->edtb.hwnd);
					eax = UpdateWindow(pMem->hsbtn);
				} // endif
			} // endif
		}
		else
		{
			if(fControl)
			{
				eax = SendMessage(hWin, WM_VSCROLL, SB_LINEUP, ((RAEDT *)esi)->hvscroll);
			}
			else
			{
				if(!(pMem->nMode&MODE_BLOCK))
				{
					eax = pMem->cpMin;
					if(eax>pMem->cpMax && !fShift)
					{
						temp1 = eax;
						eax = pMem->cpMax;
						pMem->cpMax = temp1;
						pMem->cpMin = eax;
						eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
					} // endif
					temp1 = pMem->cpx;
					eax = GetCaretPos(&pt);
					eax = pMem->fntinfo.fntht;
					pt.y -= eax;
					eax = ((RAEDT *)esi)->cpxmax;
					eax += pMem->selbarwt;
					eax += pMem->linenrwt;
					eax -= pMem->cpx;
					eax = GetCharFromPos(pMem, ((RAEDT *)esi)->cpy, eax, pt.y);
					if(eax!=pMem->cpLine)
					{
						temp2 = eax;
						eax--;
						eax = IsCharLeadByte(pMem, eax);
						edx = temp2;
						eax += edx;
					} // endif
					pMem->cpMin = eax;
					if(!fShift)
					{
						pMem->cpMax = eax;
					} // endif
					pMem->cpx = temp1;
					eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
					eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
					if(!fShift && fSel)
					{
						eax = InvalidateEdit(pMem, pMem->edta.hwnd);
						eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
					}
					else if(fShift)
					{
						eax = InvalidateLine(pMem, pMem->edta.hwnd, pMem->line);
						eax = InvalidateLine(pMem, pMem->edtb.hwnd, pMem->line);
						temp1 = pMem->line;
						eax = GetCharPtr(pMem, cpOldMin, &ecx, &edx);
						eax = temp1;
						if(eax!=pMem->line)
						{
							eax = InvalidateLine(pMem, pMem->edta.hwnd, pMem->line);
							eax = InvalidateLine(pMem, pMem->edtb.hwnd, pMem->line);
						} // endif
						eax = UpdateWindow(pMem->edta.hwnd);
						eax = UpdateWindow(pMem->edtb.hwnd);
					} // endif
				}
				else
				{
					eax = GetBlockRects(pMem, &oldrects);
					eax = GetCaretPos(&pt);
					eax = pt.y;
					eax -= pMem->fntinfo.fntht;
					eax = GetCharFromPos(pMem, ((RAEDT *)esi)->cpy, pt.x, eax);
					pMem->cpMin = eax;
					pMem->cpMax = eax;
					eax = pt.x;
					eax += pMem->cpx;
					eax -= pMem->linenrwt;
					eax -= pMem->selbarwt;
					ecx = pMem->fntinfo.fntwt;
					eax /= ecx;
					edx = pMem->line;
					pMem->blrg.lnMin = edx;
					pMem->blrg.clMin = eax;
					if(!fShift)
					{
						pMem->blrg.lnMax = edx;
						pMem->blrg.clMax = eax;
					} // endif
					eax = InvalidateBlock(pMem, &oldrects);
					eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
					eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
				} // endif
				eax = SelChange(pMem, SEL_TEXT);
				nUndoid++;
			} // endif
		} // endif
	}
	else if(eax==CMD_LINE_DOWN)
	{
		if(fAlt)
		{
			if(fControl && !fShift)
			{
				// Down, size split
				if(pMem->fsplitt<960)
				{
					pMem->fsplitt += 10;
					if(pMem->fsplitt>960)
					{
						pMem->fsplitt = 960;
					} // endif
					eax = SendMessage(pMem->hwnd, WM_SIZE, 0, 0);
					eax = UpdateWindow(pMem->edta.hwnd);
					eax = UpdateWindow(pMem->edtb.hwnd);
					eax = UpdateWindow(pMem->hsbtn);
				} // endif
			} // endif
		}
		else
		{
			if(fControl)
			{
				eax = SendMessage(hWin, WM_VSCROLL, SB_LINEDOWN, ((RAEDT *)esi)->hvscroll);
			}
			else
			{
				if(!(pMem->nMode&MODE_BLOCK))
				{
					eax = GetCharPtr(pMem, pMem->cpMin, &ecx, &edx);
					nOldLine = edx;
					eax = pMem->cpMin;
					if(eax<pMem->cpMax && !fShift)
					{
						temp1 = eax;
						eax = pMem->cpMax;
						pMem->cpMax = temp1;
						pMem->cpMin = eax;
						eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
					} // endif
					temp1 = pMem->cpx;
					eax = GetCaretPos(&pt);
					eax = pMem->fntinfo.fntht;
					pt.y += eax;
					eax = ((RAEDT *)esi)->cpxmax;
					eax += pMem->selbarwt;
					eax += pMem->linenrwt;
					eax -= pMem->cpx;
					eax = GetCharFromPos(pMem, ((RAEDT *)esi)->cpy, eax, pt.y);
					temp2 = eax;
					if(eax)
					{
						eax--;
						eax = IsCharLeadByte(pMem, eax);
					} // endif
					edx = temp2;
					edx += eax;
					eax = edx;
					pMem->cpMin = eax;
					if(!fShift)
					{
						pMem->cpMax = eax;
					} // endif
					pMem->cpx = temp1;
					eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
					eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
					if(!fShift && fSel)
					{
						eax = InvalidateEdit(pMem, pMem->edta.hwnd);
						eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
					}
					else if(fShift)
					{
						eax = InvalidateLine(pMem, pMem->edta.hwnd, pMem->line);
						eax = InvalidateLine(pMem, pMem->edtb.hwnd, pMem->line);
						eax = nOldLine;
						if(eax!=pMem->line)
						{
							eax = InvalidateLine(pMem, pMem->edta.hwnd, nOldLine);
							eax = InvalidateLine(pMem, pMem->edtb.hwnd, nOldLine);
						} // endif
						eax = UpdateWindow(pMem->edta.hwnd);
						eax = UpdateWindow(pMem->edtb.hwnd);
					} // endif
				}
				else
				{
					eax = GetBlockRects(pMem, &oldrects);
					eax = GetCaretPos(&pt);
					eax = pt.y;
					eax += pMem->fntinfo.fntht;
					eax = GetCharFromPos(pMem, ((RAEDT *)esi)->cpy, pt.x, eax);
					pMem->cpMin = eax;
					pMem->cpMax = eax;
					eax = pt.x;
					eax += pMem->cpx;
					eax -= pMem->linenrwt;
					eax -= pMem->selbarwt;
					ecx = pMem->fntinfo.fntwt;
					eax /= ecx;
					edx = pMem->line;
					pMem->blrg.lnMin = edx;
					pMem->blrg.clMin = eax;
					if(!fShift)
					{
						pMem->blrg.lnMax = edx;
						pMem->blrg.clMax = eax;
					} // endif
					eax = InvalidateBlock(pMem, &oldrects);
					eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
					eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
				} // endif
				eax = SelChange(pMem, SEL_TEXT);
				nUndoid++;
			} // endif
		} // endif
	}
	else if(eax==CMD_LEFT)
	{
		if(fAlt)
		{
			if(fControl && !fShift)
			{
				// Left
				if(pMem->cpx>20)
				{
					pMem->cpx -= 20;
				}
				else
				{
					pMem->cpx = 0;
				} // endif
				eax = InvalidateEdit(pMem, pMem->edta.hwnd);
				eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
				eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
			} // endif
		}
		else
		{
			if(!(pMem->nMode&MODE_BLOCK))
			{
anon_3:
				eax = pMem->cpMin;
				if(eax>pMem->cpMax && !fShift)
				{
					eax = pMem->cpMax;
				}
				else if(eax>=pMem->cpMax || fShift)
				{
					if(eax)
					{
						if(eax>=2)
						{
							temp1 = eax;
							eax -= 2;
							eax = IsCharLeadByte(pMem, eax);
							edx = temp1;
							edx -= eax;
							eax = edx;
						} // endif
						eax--;
					} // endif
				} // endif
				if(fControl)
				{
					eax = SkipWhiteSpace(pMem, eax, TRUE);
					eax = GetWordStart(pMem, eax, 0);
				} // endif
				eax = GetCharPtr(pMem, eax, &ecx, &edx);
				pMem->cpMin = ecx;
				if(!fShift)
				{
					pMem->cpMax = ecx;
				} // endif
				eax = IsLineHidden(pMem, edx);
				if(eax!=0)
				{
					goto anon_3;
				} // endif
				eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
				eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
				if(!fShift && fSel)
				{
					eax = InvalidateEdit(pMem, pMem->edta.hwnd);
					eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
				}
				else if(fShift)
				{
					eax = InvalidateLine(pMem, pMem->edta.hwnd, pMem->line);
					eax = InvalidateLine(pMem, pMem->edtb.hwnd, pMem->line);
					temp1 = pMem->line;
					eax = GetCharPtr(pMem, cpOldMin, &ecx, &edx);
					eax = temp1;
					if(eax!=pMem->line)
					{
						eax = InvalidateLine(pMem, pMem->edta.hwnd, pMem->line);
						eax = InvalidateLine(pMem, pMem->edtb.hwnd, pMem->line);
					} // endif
					eax = UpdateWindow(pMem->edta.hwnd);
					eax = UpdateWindow(pMem->edtb.hwnd);
				} // endif
			}
			else
			{
				eax = GetBlockRects(pMem, &oldrects);
				eax = GetCaretPos(&pt);
				eax = pt.x;
				eax -= pMem->fntinfo.fntwt;
				eax = GetCharFromPos(pMem, ((RAEDT *)esi)->cpy, eax, pt.y);
				pMem->cpMin = eax;
				pMem->cpMax = eax;
				eax = pt.x;
				eax -= pMem->fntinfo.fntwt;
				eax += pMem->cpx;
				eax -= pMem->linenrwt;
				eax -= pMem->selbarwt;
				ecx = pMem->fntinfo.fntwt;
				// cdq
				R_SIGNED(eax) /= TO_R_SIGNED(ecx);
				if(R_SIGNED(eax) < 0)
				{
					eax = 0;
				} // endif
				edx = pMem->line;
				pMem->blrg.lnMin = edx;
				pMem->blrg.clMin = eax;
				if(!fShift)
				{
					pMem->blrg.lnMax = edx;
					pMem->blrg.clMax = eax;
				} // endif
				eax = InvalidateBlock(pMem, &oldrects);
				eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
				eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
			} // endif
			eax = SetCpxMax(pMem, hWin);
			eax = SelChange(pMem, SEL_TEXT);
			nUndoid++;
		} // endif
	}
	else if(eax==CMD_RIGHT)
	{
		if(fAlt)
		{
			if(fControl && !fShift)
			{
				// Right
				pMem->cpx += 20;
				eax = InvalidateEdit(pMem, pMem->edta.hwnd);
				eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
				eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
			} // endif
		}
		else
		{
			if(!(pMem->nMode&MODE_BLOCK))
			{
anon_4:
				eax = pMem->cpMin;
				if(eax<pMem->cpMax && !fShift)
				{
					eax = pMem->cpMax;
				}
				else if(eax<=pMem->cpMax || fShift)
				{
					temp1 = eax;
					eax = IsCharLeadByte(pMem, eax);
					edx = temp1;
					edx++;
					edx += eax;
					eax = edx;
				} // endif
				if(fControl)
				{
					eax = SkipWhiteSpace(pMem, eax, FALSE);
					eax = GetWordEnd(pMem, eax, 0);
				} // endif
				eax = GetCharPtr(pMem, eax, &ecx, &edx);
				eax = pMem->cpMin;
				pMem->cpMin = ecx;
				if(!fShift)
				{
					pMem->cpMax = ecx;
				} // endif
				eax = edx;
				eax++;
				eax *= sizeof(LINE);
				if(eax<pMem->rpLineFree)
				{
					eax = IsLineHidden(pMem, edx);
					if(eax!=0)
					{
						goto anon_4;
					} // endif
				} // endif
				eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
				eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
				if(!fShift && fSel)
				{
					eax = InvalidateEdit(pMem, pMem->edta.hwnd);
					eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
				}
				else if(fShift)
				{
					eax = InvalidateLine(pMem, pMem->edta.hwnd, pMem->line);
					eax = InvalidateLine(pMem, pMem->edtb.hwnd, pMem->line);
					temp1 = pMem->line;
					eax = GetCharPtr(pMem, cpOldMin, &ecx, &edx);
					eax = temp1;
					if(eax!=pMem->line)
					{
						eax = InvalidateLine(pMem, pMem->edta.hwnd, pMem->line);
						eax = InvalidateLine(pMem, pMem->edtb.hwnd, pMem->line);
					} // endif
					eax = UpdateWindow(pMem->edta.hwnd);
					eax = UpdateWindow(pMem->edtb.hwnd);
				} // endif
			}
			else
			{
				eax = GetBlockRects(pMem, &oldrects);
				eax = GetCaretPos(&pt);
				eax = pt.x;
				eax += pMem->fntinfo.fntwt;
				eax = GetCharFromPos(pMem, ((RAEDT *)esi)->cpy, eax, pt.y);
				pMem->cpMin = eax;
				pMem->cpMax = eax;
				eax = pt.x;
				eax += pMem->fntinfo.fntwt;
				eax += pMem->cpx;
				eax -= pMem->linenrwt;
				eax -= pMem->selbarwt;
				ecx = pMem->fntinfo.fntwt;
				// cdq
				R_SIGNED(eax) /= TO_R_SIGNED(ecx);
				if(R_SIGNED(eax) < 0)
				{
					eax = 0;
				} // endif
				edx = pMem->line;
				pMem->blrg.lnMin = edx;
				pMem->blrg.clMin = eax;
				if(!fShift)
				{
					pMem->blrg.lnMax = edx;
					pMem->blrg.clMax = eax;
				} // endif
				eax = InvalidateBlock(pMem, &oldrects);
				eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
				eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
			} // endif
			eax = SetCpxMax(pMem, hWin);
			eax = SelChange(pMem, SEL_TEXT);
			nUndoid++;
		} // endif
	}
	else if(eax==CMD_PAGE_UP)
	{
		if(fAlt)
		{
			if(fControl && !fShift)
			{
				// PgUp, collapse
				RWORD(eax) = -4;
				eax = RWORD(eax);
				ecx = BN_CLICKED;
				ecx <<= 16;
				eax |= ecx;
				eax = SendMessage(pMem->hpar, WM_COMMAND, eax, pMem->hcol);
				eax = SetFocus(pMem->focus);
			} // endif
		}
		else
		{
			if(fControl)
			{
				eax = SendMessage(hWin, WM_VSCROLL, SB_PAGEUP, ((RAEDT *)esi)->hvscroll);
			}
			else
			{
				if(!(pMem->nMode&MODE_BLOCK))
				{
					eax = pMem->cpMin;
					temp1 = eax;
					if(eax>pMem->cpMax && !fShift)
					{
						temp2 = eax;
						eax = pMem->cpMax;
						pMem->cpMax = temp2;
						pMem->cpMin = eax;
						eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
					} // endif
					eax = GetCaretPos(&pt);
					eax = ((RAEDT *)esi)->rc.bottom;
					ecx = pMem->fntinfo.fntht;
					eax /= ecx;
					eax *= ecx;
					if(eax>((RAEDT *)esi)->cpy)
					{
						eax -= ((RAEDT *)esi)->cpy;
						pt.y -= eax;
						((RAEDT *)esi)->cpy = 0;
					}
					else
					{
						((RAEDT *)esi)->cpy -= eax;
					} // endif
					eax = ((RAEDT *)esi)->cpxmax;
					eax += pMem->selbarwt;
					eax += pMem->linenrwt;
					eax -= pMem->cpx;
					eax = GetCharFromPos(pMem, ((RAEDT *)esi)->cpy, eax, pt.y);
					temp2 = eax;
					eax++;
					eax = IsCharLeadByte(pMem, eax);
					edx = temp2;
					edx += eax;
					eax = edx;
					pMem->cpMin = eax;
					if(!fShift)
					{
						pMem->cpMax = eax;
					} // endif
					eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
					eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
					eax = temp1;
					if(eax!=pMem->cpMin)
					{
						eax = InvalidateEdit(pMem, pMem->edta.hwnd);
						eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
						eax = SelChange(pMem, SEL_TEXT);
						nUndoid++;
					} // endif
				}
				else
				{
					eax = GetBlockRects(pMem, &oldrects);
					eax = ((RAEDT *)esi)->rc.bottom;
					eax -= ((RAEDT *)esi)->rc.top;
					ecx = pMem->fntinfo.fntht;
					eax /= ecx;
					edx = pMem->blrg.lnMin;
					if(edx>eax)
					{
						edx -= eax;
					}
					else
					{
						edx = 0;
					} // endif
					eax = pMem->blrg.clMin;
					pMem->blrg.lnMin = edx;
					if(!fShift)
					{
						pMem->blrg.clMax = eax;
						pMem->blrg.lnMax = edx;
					} // endif
					eax = GetBlockCp(pMem, edx, eax);
					pMem->cpMin = eax;
					pMem->cpMax = eax;
					eax = GetCharPtr(pMem, eax, &ecx, &edx);
					eax = InvalidateBlock(pMem, &oldrects);
					eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
					eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
					eax = SelChange(pMem, SEL_TEXT);
					nUndoid++;
				} // endif
			} // endif
		} // endif
	}
	else if(eax==CMD_PAGE_DOWN)
	{
		if(fAlt)
		{
			if(fControl && !fShift)
			{
				// PgDn, expand
				eax = SendMessage(pMem->hpar, WM_COMMAND, -2, 0);
				RWORD(eax) = -3;
				eax = RWORD(eax);
				ecx = BN_CLICKED;
				ecx <<= 16;
				eax |= ecx;
				eax = SendMessage(pMem->hpar, WM_COMMAND, eax, pMem->hexp);
				eax = SetFocus(pMem->focus);
			} // endif
		}
		else
		{
			if(fControl)
			{
				eax = SendMessage(hWin, WM_VSCROLL, SB_PAGEDOWN, ((RAEDT *)esi)->hvscroll);
			}
			else
			{
				if(!(pMem->nMode&MODE_BLOCK))
				{
					eax = pMem->cpMin;
					temp1 = eax;
					if(eax<pMem->cpMax && !fShift)
					{
						temp2 = eax;
						eax = pMem->cpMax;
						pMem->cpMax = temp2;
						pMem->cpMin = eax;
						eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
					} // endif
					eax = GetCaretPos(&pt);
					eax = ((RAEDT *)esi)->rc.bottom;
					ecx = pMem->fntinfo.fntht;
					eax /= ecx;
					eax *= ecx;
					temp2 = eax;
					eax = pMem->rpLineFree;
					eax /= sizeof(LINE);
					eax -= pMem->nHidden;
					ecx = pMem->fntinfo.fntht;
					eax *= ecx;
					ecx = temp2;
					eax -= ecx;
					ecx += ((RAEDT *)esi)->cpy;
					if(R_SIGNED(eax) >= R_SIGNED(ecx))
					{
						((RAEDT *)esi)->cpy = ecx;
						eax = ((RAEDT *)esi)->cpxmax;
						eax += pMem->selbarwt;
						eax += pMem->linenrwt;
						eax -= pMem->cpx;
						eax = GetCharFromPos(pMem, ((RAEDT *)esi)->cpy, eax, pt.y);
						temp2 = eax;
						eax++;
						eax = IsCharLeadByte(pMem, eax);
						edx = temp2;
						edx += eax;
						eax = edx;
						pMem->cpMin = eax;
						if(!fShift)
						{
							pMem->cpMax = eax;
						} // endif
						eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
						eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
					}
					else
					{
						if(R_SIGNED(eax) < 0)
						{
							eax = 0;
						} // endif
						ecx = ((RAEDT *)esi)->cpy;
						((RAEDT *)esi)->cpy = eax;
						eax -= ecx;
						eax -= ((RAEDT *)esi)->rc.bottom;
						eax = -eax;
						pt.y += eax;
						eax = ((RAEDT *)esi)->cpxmax;
						eax += pMem->selbarwt;
						eax += pMem->linenrwt;
						eax -= pMem->cpx;
						eax = GetCharFromPos(pMem, ((RAEDT *)esi)->cpy, eax, pt.y);
						temp2 = eax;
						eax++;
						eax = IsCharLeadByte(pMem, eax);
						edx = temp2;
						edx += eax;
						eax = edx;
						pMem->cpMin = eax;
						if(!fShift)
						{
							pMem->cpMax = eax;
						} // endif
						eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
					} // endif
					eax = temp1;
					if(eax!=pMem->cpMin)
					{
						eax = InvalidateEdit(pMem, pMem->edta.hwnd);
						eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
						eax = SelChange(pMem, SEL_TEXT);
						nUndoid++;
					} // endif
				}
				else
				{
					eax = GetBlockRects(pMem, &oldrects);
					eax = ((RAEDT *)esi)->rc.bottom;
					eax -= ((RAEDT *)esi)->rc.top;
					ecx = pMem->fntinfo.fntht;
					eax /= ecx;
					edx = pMem->blrg.lnMin;
					edx += eax;
					eax = pMem->rpLineFree;
					eax /= sizeof(LINE);
					if(edx>eax)
					{
						edx = eax;
					} // endif
					eax = pMem->blrg.clMin;
					pMem->blrg.lnMin = edx;
					if(!fShift)
					{
						pMem->blrg.clMax = eax;
						pMem->blrg.lnMax = edx;
					} // endif
					eax = GetBlockCp(pMem, edx, eax);
					pMem->cpMin = eax;
					pMem->cpMax = eax;
					eax = GetCharPtr(pMem, eax, &ecx, &edx);
					eax = InvalidateBlock(pMem, &oldrects);
					eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
					eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
					eax = SelChange(pMem, SEL_TEXT);
					nUndoid++;
				} // endif
			} // endif
		} // endif
	}
	else if(eax==CMD_HOME)
	{
		if(fAlt)
		{
			if(fControl && !fShift)
			{
				// Home, jump to split
				if(pMem->nsplitt)
				{
					eax = pMem->cpMin;
					((RAEDT *)esi)->cp = eax;
					eax = hWin;
					if(eax==pMem->edtb.hwnd)
					{
						eax = pMem->edta.cp;
						pMem->cpMin = eax;
						pMem->cpMax = eax;
						eax = SetFocus(pMem->edta.hwnd);
					}
					else
					{
						eax = pMem->edtb.cp;
						pMem->cpMin = eax;
						pMem->cpMax = eax;
						eax = SetFocus(pMem->edtb.hwnd);
					} // endif
					eax = SelChange(pMem, SEL_TEXT);
					nUndoid++;
				} // endif
			} // endif
		}
		else
		{
			if(!(pMem->nMode&MODE_BLOCK))
			{
				((RAEDT *)esi)->cpxmax = 0;
				pMem->cpx = 0;
				if(fControl)
				{
					eax = 0;
					((RAEDT *)esi)->cpy = eax;
				}
				else
				{
					eax = pMem->cpMin;
					if(eax>pMem->cpMax && !fShift)
					{
						temp1 = eax;
						eax = pMem->cpMax;
						pMem->cpMax = temp1;
						pMem->cpMin = eax;
						eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
						eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
					} // endif
					eax = GetCaretPos(&pt);
					eax = GetCharFromPos(pMem, ((RAEDT *)esi)->cpy, 0, pt.y);
					temp1 = eax;
					eax = SkipSpace(pMem, eax, FALSE);
					edx = temp1;
					if(eax==pMem->cpMin)
					{
						eax = edx;
					} // endif
				} // endif
				pMem->cpMin = eax;
				if(!fShift)
				{
					pMem->cpMax = eax;
				} // endif
				eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
				eax = InvalidateEdit(pMem, pMem->edta.hwnd);
				eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
			}
			else
			{
				eax = GetBlockRects(pMem, &oldrects);
				if(fControl)
				{
					eax = GetCharFromPos(pMem, 0, pMem->cpx, 0);
				}
				else
				{
					eax = GetCaretPos(&pt);
					eax = GetCharFromPos(pMem, ((RAEDT *)esi)->cpy, pMem->cpx, pt.y);
				} // endif
				eax = SetBlockFromCp(pMem, eax, fShift);
				eax = InvalidateBlock(pMem, &oldrects);
				eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
				eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
			} // endif
			eax = SelChange(pMem, SEL_TEXT);
			nUndoid++;
		} // endif
	}
	else if(eax==CMD_END)
	{
		if(fAlt)
		{
			if(fControl && !fShift)
			{
				// End, split
				eax = 0x1ff;
				if(pMem->fsplitt)
				{
					eax = 0;
				} // endif
				eax = SendMessage(pMem->hwnd, REM_SETSPLIT, eax, 0);
			} // endif
		}
		else
		{
			if(!(pMem->nMode&MODE_BLOCK))
			{
				eax = pMem->cpMin;
				if(eax<pMem->cpMax && !fShift)
				{
					temp1 = eax;
					eax = pMem->cpMax;
					pMem->cpMax = temp1;
					eax--;
					pMem->cpMin = eax;
					eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
				}
				else if(eax>pMem->cpMax && !fShift)
				{
					pMem->cpMin--;
					eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
				} // endif
				((RAEDT *)esi)->cpxmax = 999999999;
				if(fControl)
				{
					eax = GetCharFromPos(pMem, ((RAEDT *)esi)->cpy, 999999999, 999999999);
				}
				else
				{
					eax = GetCaretPos(&pt);
					eax = GetCharFromPos(pMem, ((RAEDT *)esi)->cpy, 999999999, pt.y);
					temp1 = eax;
					eax = SkipSpace(pMem, eax, TRUE);
					edx = temp1;
					if(eax==pMem->cpMin)
					{
						eax = edx;
					} // endif
				} // endif
				pMem->cpMin = eax;
				if(!fShift)
				{
					pMem->cpMax = eax;
				} // endif
				eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
				eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
				eax = InvalidateEdit(pMem, pMem->edta.hwnd);
				eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
				eax = SetCpxMax(pMem, hWin);
			}
			else
			{
				eax = GetBlockRects(pMem, &oldrects);
				if(fControl)
				{
					eax = GetCharFromPos(pMem, ((RAEDT *)esi)->cpy, 999999999, 999999999);
				}
				else
				{
					eax = GetCaretPos(&pt);
					eax = GetCharFromPos(pMem, ((RAEDT *)esi)->cpy, 999999999, pt.y);
				} // endif
				eax = SetBlockFromCp(pMem, eax, fShift);
				eax = InvalidateBlock(pMem, &oldrects);
				eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
				eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
			} // endif
			eax = SelChange(pMem, SEL_TEXT);
			nUndoid++;
		} // endif
	}
	else if(eax==CMD_INSERT)
	{
		if(fAlt)
		{
			if(fControl && !fShift)
			{
				// Insert, show/hide linenumbers
				eax = IsDlgButtonChecked(pMem->hwnd, -2);
				if(eax)
				{
					eax = BST_UNCHECKED;
				}
				else
				{
					eax = BST_CHECKED;
				} // endif
				eax = CheckDlgButton(pMem->hwnd, -2, eax);
				RWORD(eax) = -2;
				eax = RWORD(eax);
				ecx = BN_CLICKED;
				ecx <<= 16;
				eax |= ecx;
				eax = SendMessage(pMem->hwnd, WM_COMMAND, eax, pMem->hlin);
				eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
				eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
			} // endif
		}
		else
		{
			if(!fShift && !fControl)
			{
				// Insert
				pMem->fOvr ^= 1;
				if(pMem->fOvr)
				{
					pMem->nMode |= MODE_OVERWRITE;
				}
				else
				{
					pMem->nMode &= -1 ^ MODE_OVERWRITE;
				} // endif
				eax = SelChange(pMem, SEL_TEXT);
			}
			else if(fShift && !fControl)
			{
				// Shift+Insert, Paste
				nUndoid++;
				eax = IsSelectionLocked(pMem, pMem->cpMin, pMem->cpMax);
				if(eax!=0)
				{
					goto ErrBeep;
				} // endif
				eax = Paste(pMem, hWin, NULL);
				nUndoid++;
			}
			else if(!fShift && fControl)
			{
				// Ctrl+Insert, Copy
				eax = Copy(pMem);
			} // endif
		} // endif
	}
	else if(eax==CMD_DELETE)
	{
		if(fAlt)
		{
			if(fControl && !fShift)
			{
			} // endif
		}
		else
		{
			if(((!fShift && !fControl) || (!fShift && fControl) || (fShift && fControl)))
			{
				// Delete. Ctrl+Delete, delete to end of word. Ctrl+Shift+Delete, delete to end of line.
				if(!(pMem->nMode&MODE_BLOCK))
				{
					eax = pMem->cpMin;
					if(fControl)
					{
						if(eax>pMem->cpMax)
						{
							temp1 = pMem->cpMax;
							pMem->cpMax = eax;
							eax = temp1;
							pMem->cpMin = eax;
						} // endif
						if(fShift)
						{
							eax = GetLineEnd(pMem, pMem->cpMax);
						}
						else
						{
							eax = GetWordEnd(pMem, pMem->cpMax, 0);
						} // endif
						pMem->cpMax = eax;
					} // endif
					eax = pMem->cpMin;
					if(eax!=pMem->cpMax)
					{
						// Selection
						eax = IsSelectionLocked(pMem, pMem->cpMin, pMem->cpMax);
						if(eax!=0)
						{
							goto ErrBeep;
						} // endif
						nUndoid++;
						eax = DeleteSelection(pMem, pMem->cpMin, pMem->cpMax);
						pMem->cpMin = eax;
						pMem->cpMax = eax;
						if(eax<pMem->edta.cp)
						{
							pMem->edta.cp = eax;
						} // endif
						if(eax<pMem->edtb.cp)
						{
							pMem->edtb.cp = eax;
						} // endif
						eax = GetCharPtr(pMem, pMem->cpMin, &ecx, &edx);
						eax = GetYpFromLine(pMem, edx);
						if(eax<pMem->edta.cpy)
						{
							pMem->edta.cpy = eax;
						} // endif
						if(eax<pMem->edtb.cpy)
						{
							pMem->edtb.cpy = eax;
						} // endif
						eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
						eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
						eax = InvalidateEdit(pMem, pMem->edta.hwnd);
						eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
						nUndoid++;
					}
					else
					{
						// Single char
						eax++;
						eax = IsSelectionLocked(pMem, pMem->cpMin, eax);
						if(eax!=0)
						{
							goto ErrBeep;
						} // endif
						eax = GetChar(pMem, pMem->cpMin);
						if(RBYTE_LOW(eax)==VK_RETURN)
						{
							eax = GetBookMark(pMem, pMem->line);
							if(eax==2 || eax==8)
							{
								eax = Expand(pMem, pMem->line);
							} // endif
						}
						else
						{
							eax = IsCharLeadByte(pMem, pMem->cpMin);
							if(eax)
							{
								eax = DeleteChar(pMem, pMem->cpMin);
								eax = SaveUndo(pMem, UNDO_DELETE, pMem->cpMin, eax, 1);
							} // endif
						} // endif
						eax = DeleteChar(pMem, pMem->cpMin);
						temp1 = eax;
						eax = SaveUndo(pMem, UNDO_DELETE, pMem->cpMin, eax, 1);
						eax = temp1;
						if(eax==VK_RETURN)
						{
							eax = InvalidateEdit(pMem, pMem->edta.hwnd);
							eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
						}
						else
						{
							eax = InvalidateLine(pMem, hWin, pMem->line);
						} // endif
					} // endif
				}
				else
				{
					// Block
					eax = IsSelectionLocked(pMem, pMem->cpMin, pMem->cpMax);
					if(eax!=0)
					{
						goto ErrBeep;
					} // endif
					nUndoid++;
					eax = pMem->blrg.clMin;
					if(eax==pMem->blrg.clMax)
					{
						pMem->blrg.clMax++;
					} // endif
					eax = DeleteSelectionBlock(pMem, pMem->blrg.lnMin, pMem->blrg.clMin, pMem->blrg.lnMax, pMem->blrg.clMax);
					eax = pMem->blrg.clMin;
					if(eax>pMem->blrg.clMax)
					{
						eax = pMem->blrg.clMax;
					} // endif
					edx = pMem->blrg.lnMin;
					if(edx>pMem->blrg.lnMax)
					{
						edx = pMem->blrg.lnMax;
					} // endif
					pMem->blrg.clMin = eax;
					pMem->blrg.lnMin = edx;
					pMem->blrg.clMax = eax;
					pMem->blrg.lnMax = edx;
					eax = GetBlockCp(pMem, edx, eax);
					pMem->cpMin = eax;
					pMem->cpMax = eax;
					eax = GetCharPtr(pMem, pMem->cpMin, &ecx, &edx);
					eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
					eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
					eax = InvalidateEdit(pMem, pMem->edta.hwnd);
					eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
					nUndoid++;
				} // endif
				eax = SetCpxMax(pMem, hWin);
				eax = SelChange(pMem, SEL_TEXT);
			}
			else if(fShift && !fControl)
			{
				// Shift+Delete, Cut
				eax = IsSelectionLocked(pMem, pMem->cpMin, pMem->cpMax);
				if(eax!=0)
				{
					goto ErrBeep;
				} // endif
				nUndoid++;
				eax = Cut(pMem, hWin);
				nUndoid++;
			} // endif
		} // endif
	}
	else if(eax==CMD_BACKSPACE)
	{
		if(fAlt)
		{
			if(fControl && !fShift)
			{
			} // endif
		}
		else if(fControl)
		{
			// Ctrl+Backspace, Delete to start of word.
			// Ctrl+Shift+Backspace, Delete to start of line.
			if(!(pMem->nMode&MODE_BLOCK))
			{
				eax = pMem->cpMin;
				if(eax>pMem->cpMax)
				{
					temp1 = pMem->cpMax;
					pMem->cpMax = eax;
					eax = temp1;
					pMem->cpMin = eax;
				} // endif
				if(fShift)
				{
					eax = GetLineStart(pMem, pMem->cpMin);
				}
				else
				{
					eax = GetWordStart(pMem, pMem->cpMin, 0);
				} // endif
				if(eax && eax==pMem->cpMin)
				{
					eax--;
				} // endif
				pMem->cpMin = eax;
				if(eax!=pMem->cpMax)
				{
					eax = IsSelectionLocked(pMem, pMem->cpMin, pMem->cpMax);
					if(eax!=0)
					{
						goto ErrBeep;
					} // endif
					nUndoid++;
					eax = DeleteSelection(pMem, pMem->cpMin, pMem->cpMax);
					pMem->cpMin = eax;
					pMem->cpMax = eax;
					eax = GetCharPtr(pMem, pMem->cpMin, &ecx, &edx);
					eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
					eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
					eax = InvalidateEdit(pMem, pMem->edta.hwnd);
					eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
					eax = SetCpxMax(pMem, hWin);
					eax = SelChange(pMem, SEL_TEXT);
					nUndoid++;
				} // endif
			} // endif
		} // endif
	} // endif
Ex:
	return eax;

ErrBeep:
	eax = MessageBeep(MB_ICONHAND);
	eax = 0;
	return eax;

} // EditFunc

REG_T RAEditProc(HWND hWin, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	REG_T eax = 0, ecx, edx, esi, edi;
	REG_T temp1, temp2, temp3;
	SCROLLINFO sinf;
	POINT pt;
	DWORD fAlt;
	DWORD fShift;
	DWORD fControl;
	DWORD fSel;
	DWORD cp;
	DWORD cpOldMin;
	DWORD cpOldMax;
	DWORD nOldLine;
	REG_T hCur;
	RECT rect;
	RECT oldrects[2];
	EDIT *pMem;

	auto void SetBlock(void);
	auto void SetScroll(void);

	// Get memory pointers
	eax = GetWindowLongPtr(hWin, 0);
	pMem = eax;
	if(eax)
	{
		eax = hWin;
		if(eax==pMem->edta.hwnd)
		{
			esi = &pMem->edta;
		}
		else
		{
			esi = &pMem->edtb;
		} // endif
	}
	else
	{
		if(uMsg!=WM_CREATE)
		{
			goto ExDef;
		} // endif
	} // endif
	eax = uMsg;
	if(eax==WM_PAINT)
	{
		if(((RAEDT *)esi)->rc.bottom)
		{
			if(fSize)
			{
				fSize = FALSE;
				eax = RAEditPaint(hWin);
			}
			else
			{
				if(!(pMem->fstyle&STYLE_NOBACKBUFFER))
				{
					eax = RAEditPaint(hWin);
				}
				else
				{
					eax = GetUpdateRect(hWin, &rect, FALSE);
					eax = rect.bottom;
					eax -= rect.top;
					edx = pMem->fntinfo.fntht;
					edx *= 4;
					if(eax<=edx)
					{
						eax = RAEditPaint(hWin);
					}
					else
					{
						eax = RAEditPaintNoBuff(hWin);
					} // endif
				} // endif
			} // endif
			SetScroll();
		} // endif
		eax = 0;
		goto Ex;
	}
	else if(eax==WM_KEYDOWN)
	{
		eax = GetKeyState(VK_MENU);
		eax &= 0x80;
		fAlt = eax;
		eax = GetKeyState(VK_CONTROL);
		eax &= 0x80;
		fControl = eax;
		eax = GetKeyState(VK_SHIFT);
		eax &= 0x80;
		fShift = eax;
		eax = pMem->cpMin;
		cpOldMin = eax;
		edx = pMem->cpMax;
		cpOldMax = edx;
		edx -= eax;
		fSel = edx;
		edx = wParam;
		eax = lParam;
		eax >>= 16;
		eax &= 0x3FF;
		if(edx==0x26 && (eax==0x148 || eax==0x48))
		{
			// Up
			eax = EditFunc(hWin, CMD_LINE_UP, fAlt, fShift, fControl);
		}
		else if(edx==0x28 && (eax==0x150 || eax==0x50))
		{
			// Down
			eax = EditFunc(hWin, CMD_LINE_DOWN, fAlt, fShift, fControl);
		}
		else if(edx==0x25 && (eax==0x14B || eax==0x4B))
		{
			// Left
			eax = EditFunc(hWin, CMD_LEFT, fAlt, fShift, fControl);
		}
		else if(edx==0x27 && (eax==0x14D || eax==0x4D))
		{
			// Right
			eax = EditFunc(hWin, CMD_RIGHT, fAlt, fShift, fControl);
		}
		else if(edx==0x21 && (eax==0x149 || eax==0x49))
		{
			// PgUp
			eax = EditFunc(hWin, CMD_PAGE_UP, fAlt, fShift, fControl);
		}
		else if(edx==0x22 && (eax==0x151 || eax==0x51))
		{
			// PgDn
			eax = EditFunc(hWin, CMD_PAGE_DOWN, fAlt, fShift, fControl);
		}
		else if(edx==0x24 && (eax==0x147 || eax==0x47))
		{
			// Home
			eax = EditFunc(hWin, CMD_HOME, fAlt, fShift, fControl);
		}
		else if(edx==0x23 && (eax==0x14F || eax==0x4F))
		{
			// End
			eax = EditFunc(hWin, CMD_END, fAlt, fShift, fControl);
		}
		else if(edx==0x2D && (eax==0x152 || eax==0x52))
		{
			// Insert
			eax = EditFunc(hWin, CMD_INSERT, fAlt, fShift, fControl);
		}
		else if(edx==0x2E && (eax==0x153 || eax==0x53))
		{
			// Delete
			eax = EditFunc(hWin, CMD_DELETE, fAlt, fShift, fControl);
		}
		else if(edx==0x43 && fControl && !fShift && !fAlt)
		{
			// Ctrl+C, Copy
			eax = Copy(pMem);
		}
		else if(edx==0x58 && fControl && !fShift && !fAlt)
		{
			// Ctrl+X, Cut
			eax = IsSelectionLocked(pMem, pMem->cpMin, pMem->cpMax);
			if(eax!=0)
			{
				goto ErrBeep;
			} // endif
			nUndoid++;
			eax = Cut(pMem, hWin);
			nUndoid++;
		}
		else if(edx==0x56 && fControl && !fShift && !fAlt)
		{
			// Ctrl+V, Paste
			eax = IsSelectionLocked(pMem, pMem->cpMin, pMem->cpMax);
			if(eax!=0)
			{
				goto ErrBeep;
			} // endif
			nUndoid++;
			eax = Paste(pMem, hWin, NULL);
			nUndoid++;
		}
		else if(edx==0x41 && fControl && !fShift && !fAlt)
		{
			// Ctrl+A, Select all
			if(!(pMem->nMode&MODE_BLOCK))
			{
				eax = GetCharPtr(pMem, -1, &ecx, &edx);
				pMem->cpMax = ecx;
				eax = GetCharPtr(pMem, 0, &ecx, &edx);
				pMem->cpMin = ecx;
				eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
				eax = InvalidateEdit(pMem, pMem->edta.hwnd);
				eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
				eax = SelChange(pMem, SEL_TEXT);
				nUndoid++;
			} // endif
		}
		else if(edx==0x5A && fControl && !fShift && !fAlt)
		{
			// Ctrl+Z, Undo
			nUndoid++;
			eax = Undo(esi, pMem, hWin);
			nUndoid++;
		}
		else if(edx==0x59 && fControl && !fShift && !fAlt)
		{
			// Ctrl+Y, Redo
			nUndoid++;
			eax = Redo(esi, pMem, hWin);
			nUndoid++;
		}
		else if(edx==0x08 && eax==0x0E)
		{
			// Backspace
			eax = EditFunc(hWin, CMD_BACKSPACE, fAlt, fShift, fControl);
		}
		else
		{
			goto ExDef;
		} // endif
		eax = 0;
		goto Ex;
	}
	else if(eax==WM_CHAR)
	{
		pMem->cpselbar = -1;
		if(pMem->cpbrst!=-1 || pMem->cpbren!=-1)
		{
			pMem->cpbrst = -1;
			pMem->cpbren = -1;
			eax = InvalidateEdit(pMem, pMem->edta.hwnd);
			eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
		} // endif
		if(!(pMem->nMode&MODE_BLOCK))
		{
			eax = wParam;
			if((eax>=VK_SPACE || eax==VK_RETURN || eax==VK_TAB) && eax!=0x7F)
			{
				eax = IsSelectionLocked(pMem, pMem->cpMin, pMem->cpMax);
				if(eax!=0)
				{
					goto ErrBeep;
				} // endif
				eax = pMem->cpMin;
				eax -= pMem->cpMax;
				temp1 = eax;
				eax = DeleteSelection(pMem, pMem->cpMin, pMem->cpMax);
				pMem->cpMin = eax;
				pMem->cpMax = eax;
				ecx = 1;
				if(wParam==VK_TAB && pMem->fExpandTab)
				{
					wParam = VK_SPACE;
					eax = GetTabPos(pMem, pMem->cpMin);
					ecx = pMem->nTab;
					ecx -= eax;
				} // endif
anon_5:
				temp2 = ecx;
				temp3 = pMem->cpMin;
				eax = GetCharPtr(pMem, pMem->cpMin, &ecx, &edx);
				eax = InsertChar(pMem, pMem->cpMin, wParam);
				ecx = temp3;
				if(!eax)
				{
					eax = SaveUndo(pMem, UNDO_INSERT, ecx, wParam, 1);
				}
				else
				{
					eax = SaveUndo(pMem, UNDO_OVERWRITE, ecx, eax, 1);
				} // endif
				eax = pMem->cpMin;
				eax++;
				pMem->cpMin = eax;
				pMem->cpMax = eax;
				ecx = temp2;
				ecx--;
				if(ecx!=0)
				{
					goto anon_5;
				} // endif
				if(wParam==VK_RETURN && pMem->fIndent)
				{
					eax = AutoIndent(pMem);
				} // endif
				eax = GetCharPtr(pMem, pMem->cpMin, &ecx, &edx);
				eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
				temp2 = eax;
				if(!eax)
				{
					eax = InvalidateLine(pMem, pMem->edta.hwnd, pMem->line);
					eax = InvalidateLine(pMem, pMem->edtb.hwnd, pMem->line);
				} // endif
				eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
				eax = temp2;
				edx = temp1;
				if(eax && !edx)
				{
					if(wParam==VK_RETURN)
					{
						eax = InvalidateEdit(pMem, pMem->edta.hwnd);
						eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
					}
					else
					{
						eax = InvalidateLine(pMem, pMem->edta.hwnd, pMem->line);
						eax = InvalidateLine(pMem, pMem->edtb.hwnd, pMem->line);
					} // endif
				}
				else if((eax && edx) || wParam==VK_RETURN)
				{
					eax = InvalidateEdit(pMem, pMem->edta.hwnd);
					eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
				} // endif
				eax = SetCpxMax(pMem, hWin);
				eax = SelChange(pMem, SEL_TEXT);
				eax = pMem->cpMin;
				eax--;
				eax = BracketMatch(pMem, wParam, eax);
			}
			else if(eax==0x08)
			{
				eax = pMem->cpMin;
				if(eax!=pMem->cpMax || eax)
				{
					eax = IsSelectionLocked(pMem, pMem->cpMin, pMem->cpMax);
					if(eax!=0)
					{
						goto ErrBeep;
					} // endif
					eax = pMem->cpMin;
					if(eax!=pMem->cpMax)
					{
						eax = DeleteSelection(pMem, pMem->cpMin, pMem->cpMax);
						pMem->cpMin = eax;
						pMem->cpMax = eax;
						eax = GetCharPtr(pMem, pMem->cpMin, &ecx, &edx);
						eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
						eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
						eax = InvalidateEdit(pMem, pMem->edta.hwnd);
						eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
					}
					else if(eax)
					{
						eax--;
						eax = IsSelectionLocked(pMem, eax, eax);
						if(eax!=0)
						{
							goto ErrBeep;
						} // endif
						pMem->cpMin--;
						pMem->cpMax--;
						eax = GetChar(pMem, pMem->cpMin);
						if(RBYTE_LOW(eax)==VK_RETURN)
						{
							eax = IsLineHidden(pMem, pMem->line);
							if(eax)
							{
								eax = PreviousBookMark(pMem, pMem->line, 2);
								if(eax!=-1)
								{
									eax = Expand(pMem, eax);
								} // endif
							} // endif
							eax = DeleteChar(pMem, pMem->cpMin);
							wParam = eax;
							eax = SaveUndo(pMem, UNDO_BACKDELETE, pMem->cpMin, eax, 1);
						}
						else
						{
							eax = pMem->cpMin;
							if(eax)
							{
								eax--;
								eax = IsCharLeadByte(pMem, eax);
								if(eax)
								{
									eax = DeleteChar(pMem, pMem->cpMin);
									wParam = eax;
									eax = SaveUndo(pMem, UNDO_BACKDELETE, pMem->cpMin, eax, 1);
									pMem->cpMin--;
									pMem->cpMax--;
								} // endif
							} // endif
							eax = DeleteChar(pMem, pMem->cpMin);
							wParam = eax;
							eax = SaveUndo(pMem, UNDO_BACKDELETE, pMem->cpMin, eax, 1);
						} // endif
						eax = GetCharPtr(pMem, pMem->cpMin, &ecx, &edx);
						eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
						if(wParam==VK_RETURN)
						{
							if(R_SIGNED(eax) < 0)
							{
								((RAEDT *)esi)->cpy += eax;
								eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
							} // endif
							eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
							eax = InvalidateEdit(pMem, pMem->edta.hwnd);
							eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
						}
						else
						{
							eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
							eax = InvalidateLine(pMem, pMem->edta.hwnd, pMem->line);
							eax = InvalidateLine(pMem, pMem->edtb.hwnd, pMem->line);
						} // endif
					} // endif
					eax = SetCpxMax(pMem, hWin);
					eax = SelChange(pMem, SEL_TEXT);
				} // endif
			} // endif
		}
		else
		{
			eax = wParam;
			if((eax>=VK_SPACE || eax==VK_TAB) && eax!=0x7F)
			{
				eax = IsSelectionLocked(pMem, pMem->cpMin, pMem->cpMax);
				if(eax!=0)
				{
					goto ErrBeep;
				} // endif
				temp1 = esi;
				nUndoid++;
				eax = pMem->blrg.lnMin;
				edx = pMem->blrg.lnMax;
				if(eax<edx)
				{
					temp2 = eax;
					eax = edx;
					edx = temp2;
				} // endif
				eax -= edx;
				eax++;
				edi = eax;
				eax = pMem->blrg.clMin;
				edx = pMem->blrg.clMax;
				if(eax<edx)
				{
					temp2 = eax;
					eax = edx;
					edx = temp2;
				} // endif
				eax -= edx;
				if(!eax)
				{
					if(wParam==VK_TAB)
					{
						eax = pMem->blrg.clMin;
						ecx = pMem->nTab;
						eax /= ecx;
						eax++;
						eax *= ecx;
						eax -= pMem->blrg.clMin;
						cp = eax;
						if(pMem->fExpandTab)
						{
							wParam = VK_SPACE;
						}
						else
						{
							eax = 1;
						} // endif
					}
					else
					{
						eax++;
						cp = eax;
					} // endif
				}
				else
				{
					cp = eax;
				} // endif
				esi = eax;
				eax += 2;
				eax *= edi;
				eax++;
				eax = xGlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, eax);
				temp2 = eax;
				edx = eax;
				eax = wParam;
				while(edi)
				{
					temp3 = esi;
					while(esi)
					{
						*(BYTE *)edx = RBYTE_LOW(eax);
						edx++;
						esi--;
					} // endw
					*(BYTE *)edx = 0x0D;
					edx++;
					*(BYTE *)edx = 0x0A;
					edx++;
					esi = temp3;
					edi--;
				} // endw
				eax = temp2;
				temp2 = eax;
				eax = Paste(pMem, hWin, eax);
				eax = pMem->blrg.clMin;
				if(eax==pMem->blrg.clMax)
				{
					eax += cp;
					pMem->blrg.clMin = eax;
					pMem->blrg.clMax = eax;
					eax = GetBlockCp(pMem, pMem->blrg.lnMin, eax);
					pMem->cpMin = eax;
					pMem->cpMax = eax;
				}
				else
				{
					nUndoid++;
				} // endif
				eax = temp2;
				eax = GlobalFree(eax);
				esi = temp1;
				eax = GetCharPtr(pMem, pMem->cpMin, &ecx, &edx);
				eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
				eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
				eax = SelChange(pMem, SEL_TEXT);
			}
			else if(eax==0x08)
			{
				eax = IsSelectionLocked(pMem, pMem->cpMin, pMem->cpMax);
				if(eax!=0)
				{
					goto ErrBeep;
				} // endif
				eax = pMem->blrg.clMin;
				if(eax==pMem->blrg.clMax && eax)
				{
					pMem->blrg.clMin--;
				} // endif
				eax = DeleteSelectionBlock(pMem, pMem->blrg.lnMin, pMem->blrg.clMin, pMem->blrg.lnMax, pMem->blrg.clMax);
				eax = pMem->blrg.clMin;
				if(eax>pMem->blrg.clMax)
				{
					eax = pMem->blrg.clMax;
				} // endif
				pMem->blrg.clMin = eax;
				pMem->blrg.clMax = eax;
				eax = GetBlockCp(pMem, pMem->blrg.lnMin, eax);
				pMem->cpMin = eax;
				pMem->cpMax = eax;
				eax = GetCharPtr(pMem, pMem->cpMin, &ecx, &edx);
				eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
				eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
				eax = InvalidateEdit(pMem, pMem->edta.hwnd);
				eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
				eax = SelChange(pMem, SEL_TEXT);
			} // endif
		} // endif
		eax = 0;
		goto Ex;
	}
	else if(eax==WM_MOUSEMOVE)
	{
		eax = wParam;
		eax &= MK_SHIFT;
		fShift = eax;
		if(fOnSel)
		{
			eax = LoadCursor(0, IDC_ARROW);
			eax = SetCursor(eax);
		} // endif
		eax = SendMessage(pMem->htt, TTM_ACTIVATE, FALSE, 0);
		eax = SendMessage(pMem->htt, TTM_ACTIVATE, TRUE, 0);
		eax = GetCursorPos(&pt);
		eax = ScreenToClient(hWin, &pt);
		if(fSelState==1 || fSelState==2 || fSelState==10)
		{
			// Selection, not on selbar
			if(fSelState==1)
			{
				fSelState = 2;
			} // endif
			if(wParam&MK_LBUTTON)
			{
				if(!tmr1.hwnd)
				{
					eax = pt.x;
					edx = pt.y;
					ecx = pMem->selbarwt;
					ecx += pMem->linenrwt;
					if(eax>((RAEDT *)esi)->rc.right || R_SIGNED(eax) < R_SIGNED(ecx) || edx>((RAEDT *)esi)->rc.bottom)
					{
						if(R_SIGNED(edx) > (SDWORD)((RAEDT *)esi)->rc.bottom)
						{
							edx -= ((RAEDT *)esi)->rc.bottom;
							eax = hWin;
							tmr1.hwnd = eax;
							tmr1.umsg = WM_VSCROLL;
							tmr1.wparam = SB_LINEDOWN;
							eax = ((RAEDT *)esi)->hvscroll;
							tmr1.lparam = eax;
						}
						else if(R_SIGNED(edx) < 0)
						{
							edx = -edx;
							eax = hWin;
							tmr1.hwnd = eax;
							tmr1.umsg = WM_VSCROLL;
							tmr1.wparam = SB_LINEUP;
							eax = ((RAEDT *)esi)->hvscroll;
							tmr1.lparam = eax;
						}
						else if(R_SIGNED(eax) > (SDWORD)((RAEDT *)esi)->rc.right)
						{
							edx = eax;
							edx -= ((RAEDT *)esi)->rc.right;
							eax = pMem->hwnd;
							tmr1.hwnd = eax;
							tmr1.umsg = WM_HSCROLL;
							tmr1.wparam = SB_LINEDOWN;
							eax = pMem->hhscroll;
							tmr1.lparam = eax;
						}
						else if(R_SIGNED(eax) < R_SIGNED(ecx))
						{
							edx = ecx;
							edx -= eax;
							eax = pMem->hwnd;
							tmr1.hwnd = eax;
							tmr1.umsg = WM_HSCROLL;
							tmr1.wparam = SB_LINEUP;
							eax = pMem->hhscroll;
							tmr1.lparam = eax;
						} // endif
						eax = hWin;
						tmr2.hwnd = eax;
						eax = uMsg;
						tmr2.umsg = eax;
						eax = wParam;
						tmr2.wparam = eax;
						eax = lParam;
						tmr2.lparam = eax;
						edx *= 4;
						eax = 100;
						eax -= edx;
						if(R_SIGNED(eax) < 10)
						{
							eax = 10;
						} // endif
						eax = SetTimer(NULL, 0, eax, TimerProc);
						TimerID = eax;
					} // endif
				} // endif
			}
			else
			{
				eax = 0;
				tmr1.hwnd = eax;
				tmr2.hwnd = eax;
			} // endif
			if(fSelState==2)
			{
				eax = pt.y;
				if(R_SIGNED(eax) < 0)
				{
					eax = 0;
				}
				else if(eax>((RAEDT *)esi)->rc.bottom)
				{
					eax = ((RAEDT *)esi)->rc.bottom;
				} // endif
				eax += ((RAEDT *)esi)->cpy;
				ecx = pMem->fntinfo.fntht;
				if(R_SIGNED(eax) < 0)
				{
					eax = -eax;
					eax /= ecx;
					eax *= ecx;
					eax = -eax;
				}
				else
				{
					eax /= ecx;
					eax *= ecx;
				} // endif
				eax -= ((RAEDT *)esi)->cpy;
				edi = eax;
				eax = pt.x;
				if(R_SIGNED(eax) < 0)
				{
					eax = 0;
				}
				else if(eax>((RAEDT *)esi)->rc.right)
				{
					eax = ((RAEDT *)esi)->rc.right;
				} // endif
				temp1 = eax;
				eax = SetCpxMax(pMem, hWin);
				eax = temp1;
				eax = GetCharFromPos(pMem, ((RAEDT *)esi)->cpy, eax, edi);
				temp1 = eax;
				if(eax)
				{
					eax--;
					eax = IsCharLeadByte(pMem, eax);
				} // endif
				edx = temp1;
				edx += eax;
				eax = edx;
				if(eax!=pMem->cpMin)
				{
					temp1 = pMem->cpMin;
					cpOldMin = temp1;
					pMem->cpMin = eax;
					if(!pMem->fCaretHide)
					{
						eax = HideCaret(hWin);
						pMem->fCaretHide = TRUE;
					} // endif
					eax = InvalidateSelection(pMem, pMem->edta.hwnd, cpOldMin, pMem->cpMin);
					eax = InvalidateSelection(pMem, pMem->edtb.hwnd, cpOldMin, pMem->cpMin);
					eax = SelChange(pMem, SEL_TEXT);
				} // endif
			}
			else
			{
				// Block Selection, not on selbar
				fShift = TRUE;
				eax = 0;
				SetBlock();
				eax = SelChange(pMem, SEL_TEXT);
			} // endif
		}
		else if(fOnSel==2)
		{
			eax = pt.x;
			eax -= ptDrag.x;
			if(R_SIGNED(eax) < 0)
			{
				eax = -eax;
			} // endif
			edx = pt.y;
			edx -= ptDrag.y;
			if(R_SIGNED(edx) < 0)
			{
				edx = -edx;
			} // endif
			if(eax>3 || edx>3)
			{
				fOnSel = 0;
				peff = 0;
				eax = LoadCursor(0, IDC_ARROW);
				eax = SetCursor(eax);
				hDragSourceMem = pMem;
				eax = hWin;
				hDragWin = eax;
				eax = pMem->cpMin;
				edx = pMem->cpMax;
				if(eax>edx)
				{
					temp1 = eax;
					eax = edx;
					edx = temp1;
				} // endif
				cpDragSource.cpMin = eax;
				cpDragSource.cpMax = edx;
				eax = DoDragDrop(&DataObject, &DropSource, DROPEFFECT_COPY | DROPEFFECT_MOVE, &peff);
				eax = peff;
				if(eax==DROPEFFECT_MOVE && !(pMem->fstyle & STYLE_READONLY))
				{
					eax = IsSelectionLocked(pMem, cpDragSource.cpMin, cpDragSource.cpMax);
					if(!eax)
					{
						eax = pMem->cpMin;
						if(eax>pMem->cpMax)
						{
							eax = pMem->cpMax;
						} // endif
						temp1 = eax;
						eax = DeleteSelection(pMem, cpDragSource.cpMin, cpDragSource.cpMax);
						eax = temp1;
						if(eax>cpDragSource.cpMin)
						{
							eax -= cpDragSource.cpMax;
							eax += cpDragSource.cpMin;
						} // endif
						pMem->cpMin = eax;
						pMem->cpMax = eax;
						eax = GetCharPtr(pMem, pMem->cpMin, &ecx, &edx);
						eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
						eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
						eax = InvalidateEdit(pMem, pMem->edta.hwnd);
						eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
						eax = SetCpxMax(pMem, hWin);
						eax = SelChange(pMem, SEL_TEXT);
					} // endif
				} // endif
				eax = GetFocus();
				if(eax==pMem->edta.hwnd || eax==pMem->edtb.hwnd)
				{
					eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
					eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
				} // endif
				hDragSourceMem = 0;
			} // endif
		}
		else
		{
			fOnBM = FALSE;
			fOnSel = 0;
			ecx = pMem->selbarwt;
			ecx += pMem->linenrwt;
			eax = pt.x;
			if(R_SIGNED(eax) <= R_SIGNED(ecx) || fSelState)
			{
				// Selection on selbar
				eax = hSelCur;
				hCur = eax;
				eax = pt.y;
				if(R_SIGNED(eax) > (SDWORD)((RAEDT *)esi)->rc.bottom)
				{
					eax = ((RAEDT *)esi)->rc.bottom;
				}
				else if(R_SIGNED(eax) < 0)
				{
					eax = 0;
				} // endif
				if(fSelState)
				{
					temp1 = pMem->cpMin;
					cpOldMin = temp1;
					temp1 = pMem->cpMax;
					cpOldMax = temp1;
					eax += ((RAEDT *)esi)->cpy;
					edx = 4;
					if(fShift)
					{
						edx = eax;
						edx -= iYp;
						if(R_SIGNED(edx) < 0)
						{
							edx = -edx;
						} // endif
					} // endif
					if(edx>3)
					{
						if(eax>=iYp)
						{
							edi = eax;
							eax = GetCharFromPos(pMem, 0, 0, iYp);
							pMem->cpMin = eax;
							eax = GetCharFromPos(pMem, 0, 0, edi);
							pMem->cpMax = eax;
							edi += pMem->fntinfo.fntht;
							eax = GetCharFromPos(pMem, 0, 0, edi);
							if(eax==pMem->cpMax)
							{
								eax = GetCharFromPos(pMem, 0, 9999, edi);
							} // endif
							pMem->cpMax = eax;
						}
						else
						{
							temp1 = eax;
							eax = iYp;
							eax += pMem->fntinfo.fntht;
							eax = GetCharFromPos(pMem, 0, 0, eax);
							pMem->cpMax = eax;
							eax = temp1;
							eax = GetCharFromPos(pMem, 0, 0, eax);
							pMem->cpMin = eax;
						} // endif
					} // endif
					eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
					eax = pMem->cpMin;
					if(eax!=cpOldMin)
					{
						eax = InvalidateSelection(pMem, pMem->edta.hwnd, cpOldMin, pMem->cpMin);
						eax = InvalidateSelection(pMem, pMem->edtb.hwnd, cpOldMin, pMem->cpMin);
					} // endif
					eax = pMem->cpMax;
					if(eax!=cpOldMax)
					{
						eax = InvalidateSelection(pMem, pMem->edta.hwnd, cpOldMax, pMem->cpMax);
						eax = InvalidateSelection(pMem, pMem->edtb.hwnd, cpOldMax, pMem->cpMax);
					} // endif
					eax = SelChange(pMem, SEL_TEXT);
					if(wParam&MK_LBUTTON)
					{
						eax = pt.y;
						if(eax>((RAEDT *)esi)->rc.bottom)
						{
							if(!tmr1.hwnd)
							{
								eax = pt.y;
								if(R_SIGNED(eax) > (SDWORD)((RAEDT *)esi)->rc.bottom)
								{
									edx = eax;
									edx -= ((RAEDT *)esi)->rc.bottom;
									eax = hWin;
									tmr1.hwnd = eax;
									tmr1.umsg = WM_VSCROLL;
									tmr1.wparam = SB_LINEDOWN;
									eax = ((RAEDT *)esi)->hvscroll;
									tmr1.lparam = eax;
								}
								else if(R_SIGNED(eax) < 0)
								{
									edx = eax;
									edx = -edx;
									eax = hWin;
									tmr1.hwnd = eax;
									tmr1.umsg = WM_VSCROLL;
									tmr1.wparam = SB_LINEUP;
									eax = ((RAEDT *)esi)->hvscroll;
									tmr1.lparam = eax;
								} // endif
								eax = hWin;
								tmr2.hwnd = eax;
								eax = uMsg;
								tmr2.umsg = eax;
								eax = wParam;
								tmr2.wparam = eax;
								eax = lParam;
								tmr2.lparam = eax;
								edx *= 4;
								eax = 100;
								eax -= edx;
								if(R_SIGNED(eax) < 10)
								{
									eax = 10;
								} // endif
								eax = SetTimer(NULL, 0, eax, TimerProc);
								TimerID = eax;
							} // endif
						} // endif
					} // endif
				}
				else
				{
					edx = pMem->cpx;
					edx = -edx;
					eax = GetCharFromPos(pMem, ((RAEDT *)esi)->cpy, edx, eax);
					cp = eax;
					eax = GetCharPtr(pMem, eax, &ecx, &edx);
					edi = pMem->rpChars;
					edi += pMem->hChars;
					ecx = ((CHARS *)edi)->len;
					if(((CHARS *)edi)->state&STATE_BMMASK)
					{
						eax = GetPosFromChar(pMem, cp, &pt);
						edx = pt.y;
						edx -= ((RAEDT *)esi)->cpy;
						eax = lParam;
						eax >>= 16;
						R_SIGNED(eax) = RWORD_SIGNED(eax);
						eax -= edx;
						edx = pMem->fntinfo.fntht;
						edx -= 7;
						edx /= 2;
						eax -= edx;
						if(eax<12)
						{
							eax = lParam;
							eax &= 0x0FFFF;
							R_SIGNED(eax) = RWORD_SIGNED(eax);
							eax -= pMem->linenrwt;
							eax -= pMem->selbarwt;
							eax += 15;
							if(eax<12)
							{
								eax = LoadCursor(0, IDC_ARROW);
								hCur = eax;
								fOnBM = TRUE;
							} // endif
						} // endif
					} // endif
				} // endif
			}
			else
			{
				eax = LoadCursor(0, IDC_IBEAM);
				hCur = eax;
				eax = pMem->cpMin;
				if(eax!=pMem->cpMax)
				{
					eax = GetCharFromPos(pMem, ((RAEDT *)esi)->cpy, pt.x, pt.y);
					cp = eax;
					ecx = pMem->cpMin;
					edx = pMem->cpMax;
					if(ecx>edx)
					{
						temp1 = ecx;
						ecx = edx;
						edx = temp1;
					} // endif
					if(eax>=ecx && eax<edx)
					{
						eax = GetChar(pMem, cp);
						if(eax!=0x0D)
						{
							// On selection
							fOnSel = 1;
							eax = LoadCursor(0, IDC_ARROW);
							hCur = eax;
						} // endif
					} // endif
				} // endif
			} // endif
			eax = SetCursor(hCur);
		} // endif
		eax = 0;
		return eax;
	}
	else if(eax==WM_LBUTTONDOWN)
	{
		if(!fOnSel)
		{
			eax = wParam;
			eax &= MK_SHIFT;
			fShift = eax;
			eax = pMem->cpMax;
			eax -= pMem->cpMin;
			fSel = eax;
			// Get mouse x position
			eax = lParam;
			edx = eax;
			edx >>= 16;
			ptDrag.y = edx;
			R_SIGNED(eax) = RWORD_SIGNED(eax);
			ptDrag.x = eax;
			ecx = pMem->selbarwt;
			ecx += pMem->linenrwt;
			if(eax<=ecx)
			{
				// On selection bar
				eax = GetTopFromYp(pMem, hWin, ((RAEDT *)esi)->cpy);
				eax = SetCapture(hWin);
				if(!fOnBM)
				{
					if(!(pMem->nMode&MODE_BLOCK))
					{
						if(pMem->cpx)
						{
							pMem->cpx = 0;
							eax = InvalidateEdit(pMem, pMem->edta.hwnd);
							eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
						} // endif
						edx = lParam;
						edx >>= 16;
						edx += ((RAEDT *)esi)->cpy;
						iYp = edx;
						fSelState = 3;
						if(fShift)
						{
							if(pMem->cpselbar==-1)
							{
								eax = pMem->cpMin;
								pMem->cpselbar = eax;
							} // endif
							eax = GetCharFromPos(pMem, 0, 0, iYp);
							pMem->cpMin = eax;
							eax = GetCharFromPos(pMem, 0, 9999, iYp);
							pMem->cpMax = eax;
							eax = iYp;
							eax += pMem->fntinfo.fntht;
							eax = GetCharFromPos(pMem, 0, 0, eax);
							if(eax>pMem->cpMax)
							{
								pMem->cpMax = eax;
							} // endif
							eax = pMem->cpselbar;
							eax = SendMessage(pMem->hwnd, EM_EXLINEFROMCHAR, 0, pMem->cpselbar);
							nOldLine = eax;
							eax = SendMessage(pMem->hwnd, EM_LINEINDEX, eax, 0);
							if(eax<pMem->cpMax)
							{
								pMem->cpMin = eax;
							}
							else
							{
								eax = pMem->cpMin;
								pMem->cpMax = eax;
								nOldLine++;
								eax = SendMessage(pMem->hwnd, EM_LINEINDEX, nOldLine, 0);
								pMem->cpMin = eax;
							} // endif
							eax = InvalidateEdit(pMem, pMem->edta.hwnd);
							eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
						}
						else
						{
							eax = SendMessage(hWin, WM_MOUSEMOVE, wParam, lParam);
							pMem->cpselbar = -1;
						} // endif
					} // endif
				}
				else
				{
					// On bookmark
					eax = SetFocus(hWin);
					eax = pMem->cpx;
					eax = -eax;
					edx = lParam;
					edx >>= 16;
					eax = GetCharFromPos(pMem, ((RAEDT *)esi)->cpy, eax, edx);
					if(fSel)
					{
						temp1 = eax;
						eax = InvalidateSelection(pMem, pMem->edta.hwnd, pMem->cpMin, pMem->cpMax);
						eax = InvalidateSelection(pMem, pMem->edtb.hwnd, pMem->cpMin, pMem->cpMax);
						eax = temp1;
					} // endif
					pMem->cpMin = eax;
					pMem->cpMax = eax;
					eax = GetCharPtr(pMem, eax, &ecx, &edx);
					eax = LoadCursor(0, IDC_ARROW);
					eax = SetCursor(eax);
					eax = SetCaretVisible(((RAEDT *)esi)->hwnd, ((RAEDT *)esi)->cpy);
					eax = SelChange(pMem, SEL_OBJECT);
					eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
					eax = InvalidateSelection(pMem, pMem->edta.hwnd, pMem->cpMin, pMem->cpMax);
					eax = InvalidateSelection(pMem, pMem->edtb.hwnd, pMem->cpMin, pMem->cpMax);
				} // endif
			}
			else
			{
				pMem->cpselbar = -1;
				if(!(pMem->nMode&MODE_BLOCK))
				{
					if(fSel)
					{
						eax = InvalidateSelection(pMem, pMem->edta.hwnd, pMem->cpMin, pMem->cpMax);
						eax = InvalidateSelection(pMem, pMem->edtb.hwnd, pMem->cpMin, pMem->cpMax);
					} // endif
					eax = lParam;
					edx = eax;
					R_SIGNED(eax) = RWORD_SIGNED(eax);
					edx >>= 16;
					eax += pMem->cpx;
					((RAEDT *)esi)->cpxmax = eax;
					eax -= pMem->cpx;
					eax = GetCharFromPos(pMem, ((RAEDT *)esi)->cpy, eax, edx);
					temp1 = eax;
					eax--;
					eax = IsCharLeadByte(pMem, eax);
					edx = temp1;
					edx += eax;
					eax = edx;
					pMem->cpMin = eax;
					if(!fShift)
					{
						pMem->cpMax = eax;
					} // endif
					eax = SetFocus(hWin);
					eax = SendMessage(hWin, WM_SETFOCUS, 0, 0);
					eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
					if(fShift)
					{
						eax = InvalidateSelection(pMem, pMem->edta.hwnd, pMem->cpMin, pMem->cpMax);
						eax = InvalidateSelection(pMem, pMem->edtb.hwnd, pMem->cpMin, pMem->cpMax);
					} // endif
					eax = SetCapture(hWin);
					fSelState = 1;
				}
				else
				{
					fSelState = 10;
					eax = GetCursorPos(&pt);
					eax = ScreenToClient(hWin, &pt);
					SetBlock();
					eax = SetCapture(hWin);
					eax = SetFocus(hWin);
				} // endif
				eax = SetCpxMax(pMem, hWin);
				eax = SelChange(pMem, SEL_TEXT);
			} // endif
		}
		else if(pMem->fstyle & STYLE_DRAGDROP)
		{
			eax = GetCursorPos(&ptDrag);
			eax = ScreenToClient(hWin, &ptDrag);
			fOnSel = 2;
			eax = LoadCursor(0, IDC_ARROW);
			eax = SetCursor(eax);
		} // endif
		eax = 0;
		return eax;
	}
	else if(eax==WM_LBUTTONUP)
	{
		if(fOnBM)
		{
			eax = LoadCursor(0, IDC_ARROW);
			eax = SetCursor(eax);
		}
		else if(fOnSel==1)
		{
			eax = LoadCursor(0, IDC_ARROW);
			eax = SetCursor(eax);
		}
		else if(fOnSel==2)
		{
			fOnSel = 0;
			eax = SendMessage(hWin, WM_LBUTTONDOWN, wParam, lParam);
		} // endif
		eax = ReleaseCapture();
		eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
		nUndoid++;
		eax = 0;
		fSelState = eax;
		return eax;
	}
	else if(eax==WM_LBUTTONDBLCLK)
	{
		eax = pMem->fstyle;
		eax &= STYLE_NODBLCLICK;
		edx = pMem->nMode;
		edx &= MODE_BLOCK;
		if(fOnBM)
		{
			eax = LoadCursor(0, IDC_ARROW);
			eax = SetCursor(eax);
			eax = SetCaretVisible(((RAEDT *)esi)->hwnd, ((RAEDT *)esi)->cpy);
			eax = SelChange(pMem, SEL_OBJECT);
		}
		else if(!eax && !edx)
		{
			eax = pMem->cpMin;
			cpOldMin = eax;
			edx = pMem->cpMax;
			cpOldMax = edx;
			eax = lParam;
			edx = eax;
			R_SIGNED(eax) = RWORD_SIGNED(eax);
			edx >>= 16;
			eax += pMem->cpx;
			((RAEDT *)esi)->cpxmax = eax;
			eax -= pMem->cpx;
			eax = GetCharFromPos(pMem, ((RAEDT *)esi)->cpy, eax, edx);
			temp1 = eax;
			eax--;
			eax = IsCharLeadByte(pMem, eax);
			edx = temp1;
			edx += eax;
			eax = edx;
			pMem->cpMin = eax;
			pMem->cpMax = eax;
			eax = InvalidateSelection(pMem, pMem->edta.hwnd, cpOldMin, cpOldMax);
			eax = InvalidateSelection(pMem, pMem->edtb.hwnd, cpOldMin, cpOldMax);
			eax = GetWordStart(pMem, pMem->cpMin, 0);
			pMem->cpMin = eax;
			eax = GetWordEnd(pMem, eax, 0);
			pMem->cpMax = eax;
			eax = InvalidateSelection(pMem, pMem->edta.hwnd, pMem->cpMin, pMem->cpMax);
			eax = InvalidateSelection(pMem, pMem->edtb.hwnd, pMem->cpMin, pMem->cpMax);
			eax = SelChange(pMem, SEL_TEXT);
			eax = pMem->cpMin;
			if(eax!=pMem->cpMax)
			{
				eax = LoadCursor(0, IDC_ARROW);
				eax = SetCursor(eax);
				fOnSel = 1;
			} // endif
		}
		else
		{
			eax = ReleaseCapture();
			eax = SendMessage(pMem->hwnd, uMsg, wParam, lParam);
		} // endif
		nUndoid++;
		eax = 0;
		return eax;
	}
	else if(eax==WM_RBUTTONDOWN)
	{
		eax = pMem->cpMin;
		if(eax==pMem->cpMax)
		{
			eax = SendMessage(hWin, WM_LBUTTONDOWN, wParam, lParam);
			eax = SendMessage(hWin, WM_LBUTTONUP, wParam, lParam);
		} // endif
		eax = GetParent(hWin);
		eax = GetParent(eax);
		eax = SetFocus(eax);
		eax = SetFocus(hWin);
		eax = 0;
		return eax;
	}
	else if(eax==WM_MOUSEWHEEL)
	{
		eax = wParam;
		eax &= MK_CONTROL | MK_SHIFT;
		if(pMem->nScroll)
		{
			if(!eax)
			{
				temp1 = ((RAEDT *)esi)->cpy;
				sinf.cbSize = sizeof(sinf);
				sinf.fMask = SIF_ALL;
				eax = GetScrollInfo(((RAEDT *)esi)->hvscroll, SB_CTL, &sinf);
				eax = pMem->nScroll;
				edx = pMem->fntinfo.fntht;
				eax *= edx;
				edx = eax;
				eax = RDWORD_SIGNED(wParam);
				if(R_SIGNED(eax) > 0)
				{
					if(((RAEDT *)esi)->cpy>edx)
					{
						((RAEDT *)esi)->cpy -= edx;
					}
					else
					{
						((RAEDT *)esi)->cpy = 0;
					} // endif
				}
				else
				{
					eax = sinf.nMax;
					if(eax<sinf.nPage)
					{
						goto anon_6;
					} // endif
					eax -= sinf.nPage;
					((RAEDT *)esi)->cpy += edx;
					if(R_SIGNED(eax) > (SDWORD)((RAEDT *)esi)->cpy)
					{
						goto anon_6;
					} // endif
					((RAEDT *)esi)->cpy = eax;
anon_6: ;
				} // endif
				eax = temp1;
				eax -= ((RAEDT *)esi)->cpy;
				if(eax)
				{
					temp1 = eax;
					eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
					eax = temp1;
					eax = ScrollEdit(pMem, hWin, 0, eax);
				} // endif
			}
			else if(eax==(MK_CONTROL | MK_SHIFT))
			{
				eax = RDWORD_SIGNED(wParam);
				if(R_SIGNED(eax) > 0)
				{
					eax = SB_LINELEFT;
				}
				else
				{
					eax = SB_LINERIGHT;
				} // endif
				ecx = pMem->nScroll;
				while(ecx)
				{
					temp1 = eax;

					temp2 = ecx;
					eax = PostMessage(pMem->hwnd, WM_HSCROLL, eax, pMem->hhscroll);
					ecx = temp2;
					eax = temp1;
					ecx--;
				} // endw
			} // endif
		} // endif
		eax = 0;
		return eax;
	}
	else if(eax==WM_VSCROLL)
	{
		temp1 = ((RAEDT *)esi)->cpy;
		sinf.cbSize = sizeof(sinf);
		sinf.fMask = SIF_ALL;
		eax = GetScrollInfo(lParam, SB_CTL, &sinf);
		eax = wParam;
		eax = RWORD(eax);
		if(eax==SB_THUMBTRACK || eax==SB_THUMBPOSITION)
		{
			eax = sinf.nTrackPos;
			((RAEDT *)esi)->cpy = eax;
		}
		else if(eax==SB_LINEDOWN)
		{
			eax = sinf.nMax;
			eax -= sinf.nPage;
			if(R_SIGNED(eax) > (SDWORD)((RAEDT *)esi)->cpy)
			{
				eax = ((RAEDT *)esi)->cpy;
				ecx = pMem->fntinfo.fntht;
				eax /= ecx;
				eax *= ecx;
				eax += ecx;
				((RAEDT *)esi)->cpy = eax;
			} // endif
		}
		else if(eax==SB_LINEUP)
		{
			if(((RAEDT *)esi)->cpy)
			{
				eax = ((RAEDT *)esi)->cpy;
				ecx = pMem->fntinfo.fntht;
				eax /= ecx;
				eax *= ecx;
				if(eax>ecx)
				{
					eax -= ecx;
				}
				else
				{
					eax = 0;
				} // endif
				((RAEDT *)esi)->cpy = eax;
			} // endif
		}
		else if(eax==SB_PAGEDOWN)
		{
			eax = sinf.nPage;
			((RAEDT *)esi)->cpy += eax;
			eax = sinf.nMax;
			eax -= sinf.nPage;
			if(eax<((RAEDT *)esi)->cpy)
			{
				((RAEDT *)esi)->cpy = eax;
			} // endif
		}
		else if(eax==SB_PAGEUP)
		{
			eax = sinf.nPage;
			if(((RAEDT *)esi)->cpy>eax)
			{
				((RAEDT *)esi)->cpy -= eax;
			}
			else
			{
				((RAEDT *)esi)->cpy = 0;
			} // endif
		}
		else if(eax==SB_TOP)
		{
			((RAEDT *)esi)->cpy = 0;
		}
		else if(eax==SB_BOTTOM)
		{
			eax = sinf.nMax;
			eax -= sinf.nPage;
			((RAEDT *)esi)->cpy = eax;
		} // endif
		edx = temp1;
		edx -= ((RAEDT *)esi)->cpy;
		if(edx!=0)
		{
			eax = ScrollEdit(pMem, hWin, 0, edx);
		} // endif
		if(pMem->fstyle&STYLE_SCROLLTIP)
		{
			eax = wParam;
			eax = RWORD(eax);
			if(eax==SB_THUMBTRACK)
			{
				eax = GetCursorPos(&pt);
				pt.y += 15;
				eax = pt.y;
				if(eax!=MpY)
				{
					MpY = eax;
					if(fTlln==FALSE)
					{
						eax = pt.x;
						eax -= 60;
						MpX = eax;
						fTlln = TRUE;
					} // endif
					eax = MoveWindow(pMem->htlt, MpX, pt.y, 60, 15, TRUE);
					// Get the top line number
					eax = ((RAEDT *)esi)->topln;
					eax++;
					eax = DwToAscii(eax, szLine+4);
					eax = SetWindowText(pMem->htlt, szLine);
					eax = ShowWindow(pMem->htlt, SW_SHOWNOACTIVATE);
					eax = InvalidateRect(pMem->htlt, NULL, TRUE);
					eax = UpdateWindow(pMem->htlt);
				} // endif
			}
			else if(eax==SB_ENDSCROLL)
			{
				MpY = 0;
				fTlln = FALSE;
				eax = ShowWindow(pMem->htlt, SW_HIDE);
			} // endif
		} // endif
		eax = 0;
		return eax;
	}
	else if(eax==WM_HSCROLL)
	{
		eax = PostMessage(pMem->hwnd, uMsg, wParam, lParam);
		eax = 0;
		return eax;
	}
	else if(eax==WM_SETFOCUS)
	{
		eax = DefWindowProc(hWin, uMsg, wParam, lParam);
		eax = hWin;
		pMem->focus = eax;
		if(!(pMem->nMode&MODE_BLOCK))
		{
			eax = 2;
		}
		else
		{
			eax = 3;
		} // endif
		edx = pMem->fntinfo.fntht;
		eax = CreateCaret(hWin, NULL, eax, edx);
		eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
		eax = SelChange(pMem, SEL_TEXT);
		eax = 0;
		goto Ex;
	}
	else if(eax==WM_KILLFOCUS)
	{
		eax = DestroyCaret();
	}
	else if(eax==WM_CREATE)
	{
		eax = GetParent(hWin);
		eax = GetWindowLongPtr(eax, 0);
		eax = SetWindowLongPtr(hWin, 0, eax);
	}
	else if(eax==WM_WINDOWPOSCHANGED)
	{
		fSize = TRUE;
		eax = pMem->edta.hwnd;
		if(eax==hWin)
		{
			eax = GetClientRect(hWin, & pMem->edta.rc);
			eax = pMem->fstyle;
			eax &= STYLE_NOSPLITT | STYLE_NOVSCROLL;
			if(eax!=(STYLE_NOSPLITT | STYLE_NOVSCROLL))
			{
				eax = SBWT;
				pMem->edta.rc.right -= eax;
				eax = MoveWindow(pMem->edta.hvscroll, pMem->edta.rc.right, 0, SBWT, pMem->edta.rc.bottom, TRUE);
			}
			else
			{
				eax = MoveWindow(pMem->edta.hvscroll, 0, 0, 0, 0, TRUE);
			} // endif
		}
		else
		{
			eax = GetClientRect(hWin, & pMem->edtb.rc);
			eax = pMem->fstyle;
			eax &= STYLE_NOSPLITT | STYLE_NOVSCROLL;
			if(eax!=(STYLE_NOSPLITT | STYLE_NOVSCROLL))
			{
				eax = SBWT;
				pMem->edtb.rc.right -= eax;
				ecx = pMem->edtb.rc.bottom;
				edx = 0;
				if(!pMem->nsplitt)
				{
					if(!(pMem->fstyle&STYLE_NOSPLITT))
					{
						edx = BTNHT;
						ecx -= edx;
					} // endif
				} // endif
				eax = MoveWindow(pMem->edtb.hvscroll, pMem->edtb.rc.right, edx, SBWT, ecx, TRUE);
			}
			else
			{
				eax = MoveWindow(pMem->edtb.hvscroll, 0, 0, 0, 0, TRUE);
			} // endif
		} // endif
		eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
		eax = 0;
		goto Ex;
	}
	else if(eax==WM_GETDLGCODE)
	{
		eax = DLGC_WANTCHARS | DLGC_WANTALLKEYS;
		goto Ex;
	}
	else if(eax==WM_SETCURSOR)
	{
		eax = 0;
		goto Ex;
	} // endif
ExDef:
	eax = DefWindowProc(hWin, uMsg, wParam, lParam);
Ex:
	return eax;

ErrBeep:
	eax = MessageBeep(MB_ICONHAND);
	eax = 0;
	return eax;

	void SetBlock(void)
	{
		eax = GetBlockRects(pMem, &oldrects);
		eax = pt.y;
		if(R_SIGNED(eax) < 0)
		{
			eax = 0;
		}
		else if(eax>((RAEDT *)esi)->rc.bottom)
		{
			eax = ((RAEDT *)esi)->rc.bottom;
		} // endif
		eax += ((RAEDT *)esi)->cpy;
		eax = GetLineFromYp(pMem, eax);
		pMem->blrg.lnMin = eax;
		if(!fShift)
		{
			pMem->blrg.lnMax = eax;
		} // endif
		edx = pMem->linenrwt;
		edx += pMem->selbarwt;
		eax = pt.x;
		if(R_SIGNED(eax) < R_SIGNED(edx))
		{
			eax = edx;
		}
		else if(eax>((RAEDT *)esi)->rc.right)
		{
			eax = ((RAEDT *)esi)->rc.right;
		} // endif
		eax -= pMem->linenrwt;
		eax -= pMem->selbarwt;
		eax += pMem->cpx;
		ecx = pMem->fntinfo.fntwt;
		// cdq
		R_SIGNED(eax) /= TO_R_SIGNED(ecx);
		pMem->blrg.clMin = eax;
		if(!fShift)
		{
			pMem->blrg.clMax = eax;
		} // endif
		eax = GetBlockCp(pMem, pMem->blrg.lnMin, pMem->blrg.clMin);
		pMem->cpMin = eax;
		pMem->cpMax = eax;
		eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
		eax = InvalidateBlock(pMem, &oldrects);
		return;

ErrBeep:
		eax = MessageBeep(MB_ICONHAND);
		eax = 0;
		return;

	}

	void SetScroll(void)
	{
		sinf.cbSize = sizeof(sinf);
		sinf.fMask = SIF_ALL;
		sinf.nMin = 0;
		eax = pMem->rpLineFree;
		eax /= sizeof(LINE);
		eax -= pMem->nHidden;
		ecx = pMem->fntinfo.fntht;
		eax *= ecx;
		if(eax<((RAEDT *)esi)->rc.bottom)
		{
			eax = ((RAEDT *)esi)->rc.bottom;
			eax /= ecx;
			eax *= ecx;
		} // endif
		sinf.nMax = eax;
		eax = ((RAEDT *)esi)->rc.bottom;
		eax /= ecx;
		eax *= ecx;
		sinf.nPage = eax;
		eax = ((RAEDT *)esi)->cpy;
		sinf.nPos = eax;
		eax = SetScrollInfo(((RAEDT *)esi)->hvscroll, SB_CTL, &sinf, TRUE);
		eax = pMem->fntinfo.fntwt;
		eax <<= 4;
		sinf.nPage = eax;
		eax <<= 5;
		sinf.nMax = eax;
		eax = pMem->cpx;
		sinf.nPos = eax;
		eax = SetScrollInfo(pMem->hhscroll, SB_CTL, &sinf, TRUE);
		return;

	}

} // RAEditProc

REG_T GetText(EDIT *pMem, DWORD cpMin, DWORD cpMax, REG_T lpText, DWORD fLf)
{
	REG_T eax = 0, ecx, edx, esi, edi;
	REG_T temp1;
	DWORD nLf;

	nLf = 0;
	eax = cpMin;
	edx = cpMax;
	edi = lpText;
	if(eax>edx)
	{
		temp1 = eax;
		eax = edx;
		edx = temp1;
	} // endif
	cpMin = eax;
	cpMax = edx;
	eax = GetCharPtr(pMem, cpMax, &ecx, &edx);
	cpMax = ecx;
	eax = GetCharPtr(pMem, cpMin, &ecx, &edx);
	cpMin = ecx;
	ecx = eax;
	edx = cpMin;
	esi = pMem->hLine;
	esi += pMem->rpLine;
	while(edx<cpMax)
	{
		eax = pMem->hChars;
		eax += ((LINE *)esi)->rpChars;
		temp1 = eax;
		RBYTE_LOW(eax) = *(BYTE *)(eax+ecx+sizeof(CHARS));
		ecx++;
		if(edi)
		{
			*(BYTE *)edi = RBYTE_LOW(eax);
			edi++;
		} // endif
		if(fLf && RBYTE_LOW(eax)==0x0D)
		{
			if(edi)
			{
				RBYTE_LOW(eax) = 0x0A;
				*(BYTE *)edi = RBYTE_LOW(eax);
				edi++;
			} // endif
			nLf++;
		} // endif
		eax = temp1;
		if(ecx==((CHARS *)eax)->len)
		{
			ecx = 0;
			esi += sizeof(LINE);
		} // endif
		edx++;
	} // endw
	if(edi)
	{
		*(BYTE *)edi = 0;
	} // endif
	eax = cpMax;
	eax -= cpMin;
	return eax;

} // GetText

REG_T FakeToolTipProc(HWND hWin, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	REG_T eax = 0;
	REG_T temp1;
	PAINTSTRUCT ps;
	BYTE buffer[16];
	REG_T hFnt;

	eax = uMsg;
	if(eax==WM_PAINT)
	{
		eax = SendMessage(hWin, WM_GETFONT, 0, 0);
		hFnt = eax;
		eax = GetWindowText(hWin, &buffer, sizeof(buffer));
		eax = BeginPaint(hWin, &ps);
		eax = SelectObject(ps.hdc, hFnt);
		temp1 = eax;
		eax = SetBkMode(ps.hdc, TRANSPARENT);
		eax = GetSysColor(COLOR_INFOTEXT);
		eax = SetTextColor(ps.hdc, eax);
		eax = FillRect(ps.hdc, &ps.rcPaint, hBrTlt);
		eax = strlen(&buffer);
		eax = TextOut(ps.hdc, 5, 0, &buffer, eax);
		eax = temp1;
		eax = SelectObject(ps.hdc, eax);
		eax = EndPaint(hWin, &ps);
		eax = 0;
		return eax;
	} // endif
	eax = CallWindowProc(OldFakeToolTipProc, hWin, uMsg, wParam, lParam);
	return eax;

} // FakeToolTipProc

REG_T ConvTwipsToPixels(HDC hDC, DWORD fHorz, DWORD lSize)
{
	REG_T eax = 0;

	if(fHorz)
	{
		eax = GetDeviceCaps(hDC, LOGPIXELSX);
	}
	else
	{
		eax = GetDeviceCaps(hDC, LOGPIXELSY);
	} // endif
	eax = MulDiv(lSize, 1440, eax);
	return eax;

} // ConvTwipsToPixels

// The edit controls callback (WndProc).
REG_T RAWndProc(HWND hWin, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	REG_T eax = 0, ecx, edx, esi, edi;
	REG_T temp1, temp2, temp3;
	RECT rect;
	POINT pt;
	SCROLLINFO sinf;
	CHARRANGE chrg;
	TOOLINFO ti;
	RECT oldrects[2];
	DWORD nLine;
	DWORD len;
	DWORD tabWt;
	LOGFONT lf;
	DWORD fAlt;
	DWORD fShift;
	DWORD fControl;
	EDIT *pMem;

	auto void SetToolTip(void);
	auto void SizeIt(void);
	auto void AllocMem(void);
	auto void RelMem(void);

	// Get memory pointer
	eax = GetWindowLongPtr(hWin, 0);
	if(eax)
	{
		pMem = eax;
		eax = pMem->focus;
		if(eax==pMem->edta.hwnd)
		{
			esi = &pMem->edta;
		}
		else
		{
			esi = &pMem->edtb;
		} // endif
	}
	else
	{
		if(uMsg!=WM_CREATE)
		{
			goto ExDef;
		} // endif
	} // endif
	eax = uMsg;
	if(eax>=REM_BASE && eax<=REM_GETLINEBEGIN)
	{
		eax -= REM_BASE;
		switch(eax + REM_BASE)
		{
			// RAEdit_REM_Msg_JumpTable.asm
			// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

		case REM_SETHILITEWORDS:
			// wParam=Color
			// lParam=lpszWords
			eax = SetHiliteWords(wParam, lParam);
			eax = 0;
			return eax;

		case REM_SETFONT:
			// wParam=nLineSpacing
			// lParam=lpRAFONT
			ecx = pMem->fntinfo.fntht;
			if(ecx)
			{
				eax = pMem->edta.cpy;
				eax /= ecx;
				temp1 = eax;
				eax = pMem->edtb.cpy;
				eax /= ecx;
				temp2 = eax;
			}
			else
			{
				temp1 = 0;
				temp2 = 0;
			} // endif
			eax = wParam;
			pMem->fntinfo.linespace = eax;
			eax = SetFont(pMem, lParam);
			ecx = pMem->fntinfo.fntht;
			eax = temp2;
			eax *= ecx;
			pMem->edtb.cpy = eax;
			eax = temp1;
			eax *= ecx;
			pMem->edta.cpy = eax;
			eax = 0;
			pMem->edta.topyp = eax;
			pMem->edta.topln = eax;
			pMem->edta.topcp = eax;
			pMem->edtb.topyp = eax;
			pMem->edtb.topln = eax;
			pMem->edtb.topcp = eax;
			eax = InvalidateEdit(pMem, pMem->edta.hwnd);
			eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
			if(!pMem->fntinfo.monospace)
			{
				eax = pMem->nMode;
				if(eax&MODE_BLOCK)
				{
					eax ^= MODE_BLOCK;
					eax = SendMessage(hWin, REM_SETMODE, eax, 0);
				} // endif
			} // endif
			eax = GetFocus();
			if(eax==pMem->focus && eax)
			{
				eax = SetFocus(hWin);
			} // endif
			eax = 0;
			return eax;

		case REM_GETFONT:
			// wParam=0
			// lParam=lpRAFONT
			edx = lParam;
			eax = pMem->fnt.hFont;
			((RAFONT *)edx)->hFont = eax;
			eax = pMem->fnt.hIFont;
			((RAFONT *)edx)->hIFont = eax;
			eax = pMem->fnt.hLnrFont;
			((RAFONT *)edx)->hLnrFont = eax;
			eax = pMem->fntinfo.linespace;
			return eax;

		case REM_SETCOLOR:
			// wParam=0
			// lParam=lpRACOLOR
			edx = lParam;
			eax = ((RACOLOR *)edx)->bckcol;
			pMem->clr.bckcol = eax;
			eax = ((RACOLOR *)edx)->txtcol;
			pMem->clr.txtcol = eax;
			eax = ((RACOLOR *)edx)->selbckcol;
			if(eax==((RACOLOR *)edx)->bckcol)
			{
				eax ^= 0x03F3F3F;
			} // endif
			pMem->clr.selbckcol = eax;
			eax = ((RACOLOR *)edx)->seltxtcol;
			pMem->clr.seltxtcol = eax;
			eax = ((RACOLOR *)edx)->cmntcol;
			pMem->clr.cmntcol = eax;
			eax = ((RACOLOR *)edx)->strcol;
			pMem->clr.strcol = eax;
			eax = ((RACOLOR *)edx)->oprcol;
			pMem->clr.oprcol = eax;
			eax = ((RACOLOR *)edx)->hicol1;
			pMem->clr.hicol1 = eax;
			eax = ((RACOLOR *)edx)->hicol2;
			pMem->clr.hicol2 = eax;
			eax = ((RACOLOR *)edx)->hicol3;
			pMem->clr.hicol3 = eax;
			eax = ((RACOLOR *)edx)->selbarbck;
			pMem->clr.selbarbck = eax;
			eax = ((RACOLOR *)edx)->selbarpen;
			pMem->clr.selbarpen = eax;
			eax = ((RACOLOR *)edx)->lnrcol;
			pMem->clr.lnrcol = eax;
			eax = ((RACOLOR *)edx)->numcol;
			pMem->clr.numcol = eax;
			eax = ((RACOLOR *)edx)->cmntback;
			pMem->clr.cmntback = eax;
			eax = ((RACOLOR *)edx)->strback;
			pMem->clr.strback = eax;
			eax = ((RACOLOR *)edx)->numback;
			pMem->clr.numback = eax;
			eax = ((RACOLOR *)edx)->oprback;
			pMem->clr.oprback = eax;
			eax = ((RACOLOR *)edx)->changed;
			pMem->clr.changed = eax;
			eax = ((RACOLOR *)edx)->changesaved;
			pMem->clr.changesaved = eax;
			eax = CreateBrushes(pMem);
			eax = InvalidateEdit(pMem, pMem->edta.hwnd);
			eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
			eax = InvalidateRect(pMem->hsta, NULL, FALSE);
			eax = 0;
			return eax;

		case REM_GETCOLOR:
			// wParam=0
			// lParam=lpRACOLOR
			edx = lParam;
			eax = pMem->clr.bckcol;
			((RACOLOR *)edx)->bckcol = eax;
			eax = pMem->clr.txtcol;
			((RACOLOR *)edx)->txtcol = eax;
			eax = pMem->clr.selbckcol;
			((RACOLOR *)edx)->selbckcol = eax;
			eax = pMem->clr.seltxtcol;
			((RACOLOR *)edx)->seltxtcol = eax;
			eax = pMem->clr.cmntcol;
			((RACOLOR *)edx)->cmntcol = eax;
			eax = pMem->clr.strcol;
			((RACOLOR *)edx)->strcol = eax;
			eax = pMem->clr.oprcol;
			((RACOLOR *)edx)->oprcol = eax;
			eax = pMem->clr.hicol1;
			((RACOLOR *)edx)->hicol1 = eax;
			eax = pMem->clr.hicol2;
			((RACOLOR *)edx)->hicol2 = eax;
			eax = pMem->clr.hicol3;
			((RACOLOR *)edx)->hicol3 = eax;
			eax = pMem->clr.selbarbck;
			((RACOLOR *)edx)->selbarbck = eax;
			eax = pMem->clr.selbarpen;
			((RACOLOR *)edx)->selbarpen = eax;
			eax = pMem->clr.lnrcol;
			((RACOLOR *)edx)->lnrcol = eax;
			eax = pMem->clr.numcol;
			((RACOLOR *)edx)->numcol = eax;
			eax = pMem->clr.cmntback;
			((RACOLOR *)edx)->cmntback = eax;
			eax = pMem->clr.strback;
			((RACOLOR *)edx)->strback = eax;
			eax = pMem->clr.numback;
			((RACOLOR *)edx)->numback = eax;
			eax = pMem->clr.oprback;
			((RACOLOR *)edx)->oprback = eax;
			eax = pMem->clr.changed;
			((RACOLOR *)edx)->changed = eax;
			eax = pMem->clr.changesaved;
			((RACOLOR *)edx)->changesaved = eax;
			eax = 0;
			return eax;

		case REM_SETHILITELINE:
			// wParam=Line
			// lParam=nColor
			eax = HiliteLine(pMem, wParam, lParam);
			return eax;

		case REM_GETHILITELINE:
			// wParam=Line
			// lParam=0
			eax = 0;
			eax--;
			edx = wParam;
			edx *= sizeof(LINE);
			if(edx<pMem->rpLineFree)
			{
				edx += pMem->hLine;
				edx = ((LINE *)edx)->rpChars;
				edx += pMem->hChars;
				eax = ((CHARS *)edx)->state;
				eax &= STATE_HILITEMASK;
			} // endif
			return eax;

		case REM_SETBOOKMARK:
			// wParam=Line
			// lParam=nType
			eax = SetBookMark(pMem, wParam, lParam);
			temp1 = eax;
			eax = InvalidateLine(pMem, pMem->edta.hwnd, wParam);
			eax = InvalidateLine(pMem, pMem->edtb.hwnd, wParam);
			eax = temp1;
			return eax;

		case REM_GETBOOKMARK:
			// wParam=Line
			// lParam=0
			eax = GetBookMark(pMem, wParam);
			return eax;

		case REM_CLRBOOKMARKS:
			// wParam=0
			// lParam=nType
			eax = ClearBookMarks(pMem, lParam);
			eax = InvalidateEdit(pMem, pMem->edta.hwnd);
			eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
			eax = 0;
			return eax;

		case REM_NXTBOOKMARK:
			// wParam=Line
			// lParam=nType
			eax = NextBookMark(pMem, wParam, lParam);
			return eax;

		case REM_PRVBOOKMARK:
			// wParam=Line
			// lParam=nType
			eax = PreviousBookMark(pMem, wParam, lParam);
			return eax;

		case REM_FINDBOOKMARK:
			// wParam=BmID
			// lParam=0
			eax = 0;
			eax--;
			ecx = wParam;
			edi = 0;
			while(edi<pMem->rpLineFree)
			{
				edx = edi;
				edx += pMem->hLine;
				edx = ((LINE *)edx)->rpChars;
				edx += pMem->hChars;
				if(ecx==((CHARS *)edx)->bmid)
				{
					eax = edi;
					eax /= sizeof(LINE);
					break;
				} // endif
				edi += sizeof(LINE);
			} // endw
			return eax;

		case REM_SETBLOCKS:
			// wParam=[lpLINERANGE]
			// lParam=0
			temp1 = nBmid;
			esi = blockdefs;
			edi = esi+32*sizeof(REG_T);
			while(*(REG_T *)esi)
			{
				eax = ((RABLOCKDEF *)edi)->flag;
				eax >>= 16;
				if(eax==pMem->nWordGroup)
				{
					eax = SetBlocks(pMem, wParam, edi);
				} // endif
				edi = *(REG_T *)esi;
				esi += sizeof(REG_T);
			} // endw
			eax = temp1;
			if(eax!=nBmid)
			{
				eax = InvalidateRect(pMem->edta.hwnd, NULL, FALSE);
				eax = InvalidateRect(pMem->edtb.hwnd, NULL, FALSE);
			} // endif
			eax = 0;
			return eax;

		case REM_ISLINE:
			// wParam=Line
			// lParam=lpszDef
			eax = IsLine(pMem, wParam, lParam);
			return eax;

		case REM_GETWORD:
			// wParam=BuffSize
			// lParam=lpBuff
			eax = GetWordStart(pMem, pMem->cpMin, 0);
			esi = pMem->rpChars;
			ecx = eax;
			ecx -= pMem->cpLine;
			temp1 = ecx;
			temp2 = eax;
			eax = GetWordEnd(pMem, eax, 0);
			ecx = temp2;
			edx = temp1;
			eax -= ecx;
			ecx = eax;
			edi = lParam;
			if(ecx>=wParam)
			{
				ecx = wParam;
				ecx--;
			} // endif
			esi += pMem->hChars;
			esi += edx;
			esi += sizeof(CHARS);
			eax = ecx;
			while(ecx > 0)
			{
				*(BYTE *)edi = *(BYTE *)esi;
				edi++;
				esi++;
				ecx--;
			}
			*(BYTE *)edi = 0;
			return eax;

		case REM_COLLAPSE:
			// wParam=Line
			// lParam=0
			eax = Collapse(pMem, wParam);
			eax = InvalidateEdit(pMem, pMem->edta.hwnd);
			eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
			return eax;

		case REM_COLLAPSEALL:
			// wParam=0
			// lParam=0
			eax = CollapseAll(pMem);
			if(eax)
			{
				eax = GetLineFromCp(pMem, pMem->cpMin);
				esi = eax;
				edi = eax;
anon_7:
				eax = IsLineHidden(pMem, esi);
				if(eax)
				{
					esi--;
					goto anon_7;
				} // endif
				if(esi!=edi)
				{
					eax = GetCpFromLine(pMem, esi);
					chrg.cpMin = eax;
					chrg.cpMax = eax;
					eax = SendMessage(hWin, EM_EXSETSEL, 0, &chrg);
				} // endif
				if(pMem->fsplitt)
				{
					eax = InvalidateEdit(pMem, pMem->edta.hwnd);
					eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
				} // endif
				eax = SendMessage(hWin, REM_VCENTER, 0, 0);
			} // endif
			return eax;

		case REM_EXPAND:
			// wParam=Line
			// lParam=0
			eax = Expand(pMem, wParam);
			temp1 = eax;
			eax = InvalidateEdit(pMem, pMem->edta.hwnd);
			eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
			eax = temp1;
			return eax;

		case REM_EXPANDALL:
			// wParam=0
			// lParam=0
			eax = ExpandAll(pMem);
			if(eax)
			{
				if(pMem->fsplitt)
				{
					eax = InvalidateEdit(pMem, pMem->edta.hwnd);
					eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
				} // endif
				eax = SendMessage(hWin, REM_VCENTER, 0, 0);
			} // endif
			return eax;

		case REM_LOCKLINE:
			// wParam=Line
			// lParam=TRUE/FALSE
			eax = LockLine(pMem, wParam, lParam);
			return eax;

		case REM_ISLINELOCKED:
			// wParam=Line
			// lParam=0
			eax = IsLineLocked(pMem, wParam);
			if(eax)
			{
				eax = TRUE;
			} // endif
			return eax;

		case REM_HIDELINE:
			// wParam=Line
			// lParam=TRUE/FALSE
			eax = HideLine(pMem, wParam, lParam);
			temp1 = eax;
			eax = InvalidateEdit(pMem, pMem->edta.hwnd);
			eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
			eax = temp1;
			return eax;

		case REM_ISLINEHIDDEN:
			// wParam=Line
			// lParam=0
			eax = IsLineHidden(pMem, wParam);
			if(eax)
			{
				eax = TRUE;
			} // endif
			return eax;

		case REM_AUTOINDENT:
			// wParam=0
			// lParam=TRUE/FALSE
			eax = lParam;
			pMem->fIndent = eax;
			return eax;

		case REM_TABWIDTH:
			// wParam=nChars
			// lParam=0
			eax = wParam;
			pMem->nTab = eax;
			eax = lParam;
			pMem->fExpandTab = eax;
			eax = SetFont(pMem, & pMem->fnt);
			eax = InvalidateEdit(pMem, pMem->edta.hwnd);
			eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
			eax = 0;
			return eax;

		case REM_SELBARWIDTH:
			// wParam=nWidth
			// lParam=0
			eax = wParam;
			pMem->selbarwt = eax;
			eax = InvalidateEdit(pMem, pMem->edta.hwnd);
			eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
			eax = 0;
			return eax;

		case REM_LINENUMBERWIDTH:
			// wParam=nWidth
			// lParam=0
			eax = wParam;
			pMem->nlinenrwt = eax;
			if(pMem->linenrwt)
			{
				pMem->linenrwt = eax;
				eax = InvalidateEdit(pMem, pMem->edta.hwnd);
				eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
			} // endif
			eax = 0;
			return eax;

		case REM_MOUSEWHEEL:
			// wParam=nLines
			// lParam=0
			eax = wParam;
			pMem->nScroll = eax;
			return eax;

		case REM_SUBCLASS:
			// wParam=0
			// lParam=lpWndProc
			eax = SetWindowLongPtr(pMem->edta.hwnd, GWLP_WNDPROC, lParam);
			eax = SetWindowLongPtr(pMem->edtb.hwnd, GWLP_WNDPROC, lParam);
			return eax;

		case REM_SETSPLIT:
			// wParam=nSplit
			// lParam=0
			eax = wParam;
			eax &= 0x1FF;
			pMem->fsplitt = eax;
			if(!eax)
			{
				eax = pMem->focus;
				if(eax==pMem->edta.hwnd)
				{
					eax = pMem->edta.cpxmax;
					pMem->edtb.cpxmax = eax;
					eax = pMem->edta.cpy;
					pMem->edtb.cpy = eax;
				} // endif
			} // endif
			SizeIt();
			eax = SetFocus(pMem->edtb.hwnd);
			eax = SetCaretVisible(pMem->edtb.hwnd, pMem->edtb.cpy);
			return eax;

		case REM_GETSPLIT:
			// wParam=0
			// lParam=0
			eax = pMem->fsplitt;
			return eax;

		case REM_VCENTER:
			// wParam=0
			// lParam=0
			eax = ((RAEDT *)esi)->rc.bottom;
			eax /= 2;
			ecx = pMem->fntinfo.fntht;
			eax /= ecx;
			eax *= ecx;
			temp1 = eax;
			eax = GetLineFromCp(pMem, pMem->cpMin);
			eax = GetYpFromLine(pMem, eax);
			edx = temp1;
			if(eax>edx)
			{
				eax -= edx;
			}
			else
			{
				eax = 0;
			} // endif
			((RAEDT *)esi)->cpy = eax;
			eax = SetCaretVisible(((RAEDT *)esi)->hwnd, ((RAEDT *)esi)->cpy);
			eax = InvalidateEdit(pMem, ((RAEDT *)esi)->hwnd);
			eax = 0;
			return eax;

		case REM_REPAINT:
			// wParam=0
			// lParam=TRUE/FALSE (Paint Now)
			eax = InvalidateRect(pMem->edta.hwnd, NULL, FALSE);
			eax = InvalidateRect(pMem->edta.hvscroll, NULL, TRUE);
			eax = InvalidateRect(pMem->edtb.hwnd, NULL, FALSE);
			eax = InvalidateRect(pMem->edtb.hvscroll, NULL, TRUE);
			eax = InvalidateRect(pMem->hhscroll, NULL, TRUE);
			eax = InvalidateRect(pMem->hgrip, NULL, TRUE);
			eax = InvalidateRect(pMem->hnogrip, NULL, TRUE);
			eax = InvalidateRect(pMem->hsbtn, NULL, TRUE);
			eax = InvalidateRect(pMem->hlin, NULL, TRUE);
			eax = InvalidateRect(pMem->hexp, NULL, TRUE);
			eax = InvalidateRect(pMem->hcol, NULL, TRUE);
			eax = InvalidateRect(pMem->hlock, NULL, TRUE);
			eax = InvalidateRect(pMem->hsta, NULL, TRUE);
			if(lParam)
			{
				eax = UpdateWindow(pMem->edta.hwnd);
				eax = UpdateWindow(pMem->edtb.hwnd);
			} // endif
			eax = 0;
			return eax;

		case REM_BMCALLBACK:
			// wParam=0
			// lParam=lpBmProc
			eax = lParam;
			pMem->lpBmCB = eax;
			return eax;

		case REM_READONLY:
			// wParam=0
			// lParam=TRUE/FALSE
			eax = GetWindowLong(hWin, GWL_STYLE);
			if(lParam)
			{
				eax |= STYLE_READONLY;
			}
			else
			{
				eax &= -1 ^ STYLE_READONLY;
			} // endif
			pMem->fstyle = eax;
			eax = SetWindowLong(hWin, GWL_STYLE, eax);
			eax = InvalidateRect(pMem->hsta, NULL, TRUE);
			eax = 0;
			return eax;

		case REM_INVALIDATELINE:
			// wParam=nLine
			// lParam=0
			eax = InvalidateLine(pMem, pMem->edta.hwnd, wParam);
			eax = InvalidateLine(pMem, pMem->edtb.hwnd, wParam);
			eax = 0;
			return eax;

		case REM_SETPAGESIZE:
			// wParam=nLines
			// lParam=0
			eax = wParam;
			pMem->nPageBreak = eax;
			eax = InvalidateEdit(pMem, pMem->edta.hwnd);
			eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
			eax = 0;
			return eax;

		case REM_GETPAGESIZE:
			// wParam=0
			// lParam=0
			eax = pMem->nPageBreak;
			return eax;

		case REM_GETCHARTAB:
			// wParam=nChar
			// lParam=0
			edx = wParam;
			edx &= 0x0FF;
			eax = *(BYTE *)(edx+CharTab);
			return eax;

		case REM_SETCHARTAB:
			// wParam=nChar
			// lParam=nType
			edx = wParam;
			edx &= 0x0FF;
			eax = lParam;
			*(BYTE *)(edx+CharTab) = RBYTE_LOW(eax);
			return eax;

		case REM_SETCOMMENTBLOCKS:
			// wParam=lpStart
			// lParam=lpEnd
			eax = SetCommentBlocks(pMem, wParam, lParam);
			return eax;

		case REM_SETWORDGROUP:
			// wParam=0
			// lParam=nGroup (0-15)
			eax = lParam;
			eax &= 0x0F;
			pMem->nWordGroup = eax;
			return eax;

		case REM_GETWORDGROUP:
			// wParam=0
			// lParam=0
			eax = pMem->nWordGroup;
			return eax;

		case REM_SETBMID:
			// wParam=nLine
			// lParam=nBmID
			edx = wParam;
			edx *= sizeof(LINE);
			if(edx<pMem->rpLineFree)
			{
				edx += pMem->hLine;
				edx = ((LINE *)edx)->rpChars;
				edx += pMem->hChars;
				eax = lParam;
				((CHARS *)edx)->bmid = eax;
			} // endif
			return eax;

		case REM_GETBMID:
			// wParam=nLine
			// lParam=0
			eax = 0;
			edx = wParam;
			edx *= sizeof(LINE);
			if(edx<pMem->rpLineFree)
			{
				edx += pMem->hLine;
				edx = ((LINE *)edx)->rpChars;
				edx += pMem->hChars;
				eax = ((CHARS *)edx)->bmid;
			} // endif
			return eax;

		case REM_ISCHARPOS:
			// wParam=CP
			// lParam=0
			eax = IsCharPos(pMem, wParam);
			return eax;

		case REM_HIDELINES:
			// wParam=nLine
			// lParam=nLines
			eax = 0;
			if(lParam>1)
			{
				eax = GetBookMark(pMem, wParam);
				if(!eax)
				{
					temp1 = pMem->nHidden;
					ecx = lParam;
					edx = wParam;
					ecx--;
					while(ecx)
					{
						edx++;
						temp2 = ecx;
						temp3 = edx;
						eax = HideLine(pMem, edx, TRUE);
						if(eax)
						{
							edx = temp3;
							temp3 = edx;
							edx *= sizeof(LINE);
							if(edx<pMem->rpLineFree)
							{
								edx += pMem->hLine;
								edx = ((LINE *)edx)->rpChars;
								edx += pMem->hChars;
								eax = nBmid;
								eax++;
								((CHARS *)edx)->bmid = eax;
							} // endif
						} // endif
						edx = temp3;
						ecx = temp2;
						ecx--;
					} // endw
					edx = temp1;
					eax = pMem->nHidden;
					eax -= edx;
					if(eax)
					{
						temp1 = eax;
						eax = SetBookMark(pMem, wParam, 8);
						eax = pMem->cpMin;
						if(eax>pMem->cpMax)
						{
							eax = pMem->cpMax;
						} // endif
						pMem->cpMin = eax;
						pMem->cpMax = eax;
						eax = pMem->rpLineFree;
						eax /= sizeof(LINE);
						eax -= pMem->nHidden;
						ecx = pMem->fntinfo.fntht;
						eax *= ecx;
						ecx = 0;
						if(eax<pMem->edta.cpy)
						{
							pMem->edta.cpy = eax;
							pMem->edta.topyp = ecx;
							pMem->edta.topln = ecx;
							pMem->edta.topcp = ecx;
						} // endif
						if(eax<pMem->edtb.cpy)
						{
							pMem->edtb.cpy = eax;
							pMem->edtb.topyp = ecx;
							pMem->edtb.topln = ecx;
							pMem->edtb.topcp = ecx;
						} // endif
						eax = InvalidateEdit(pMem, pMem->edta.hwnd);
						eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
						eax = temp1;
					} // endif
				}
				else
				{
					eax = 0;
				} // endif
			} // endif
			return eax;

		case REM_SETDIVIDERLINE:
			// wParam=nLine
			// lParam=TRUE/FALSE
			edx = wParam;
			edx *= sizeof(LINE);
			if(edx<pMem->rpLineFree)
			{
				edx += pMem->hLine;
				edx = ((LINE *)edx)->rpChars;
				edx += pMem->hChars;
				if(lParam)
				{
					((CHARS *)edx)->state |= STATE_DIVIDERLINE;
				}
				else
				{
					((CHARS *)edx)->state &= -1 ^ STATE_DIVIDERLINE;
				} // endif
				eax = InvalidateLine(pMem, pMem->edta.hwnd, wParam);
				eax = InvalidateLine(pMem, pMem->edtb.hwnd, wParam);
			} // endif
			eax = 0;
			return eax;

		case REM_ISINBLOCK:
			// wParam=nLine
			// lParam=lpRABLOCKDEF
			eax = IsInBlock(pMem, wParam, lParam);
			return eax;

		case REM_TRIMSPACE:
			// wParam=nLine
			// lParam=fLeft
			eax = TrimSpace(pMem, wParam, lParam);
			if(eax)
			{
				temp1 = eax;
				eax = SelChange(pMem, SEL_TEXT);
				eax = temp1;
			} // endif
			return eax;

		case REM_SAVESEL:
			// wParam=0
			// lParam=0
			eax = pMem->cpMin;
			pMem->savesel.cpMin = eax;
			eax = pMem->cpMax;
			pMem->savesel.cpMax = eax;
			eax = 0;
			return eax;

		case REM_RESTORESEL:
			// wParam=0
			// lParam=0
			if(!pMem->fHideSel)
			{
				eax = pMem->cpMin;
				if(eax!=pMem->cpMax)
				{
					eax = InvalidateSelection(pMem, pMem->edta.hwnd, pMem->cpMin, pMem->cpMax);
					eax = InvalidateSelection(pMem, pMem->edtb.hwnd, pMem->cpMin, pMem->cpMax);
				} // endif
			} // endif
			eax = GetCharPtr(pMem, pMem->savesel.cpMax, &ecx, &edx);
			pMem->cpMax = ecx;
			eax = GetCharPtr(pMem, pMem->savesel.cpMin, &ecx, &edx);
			pMem->cpMin = ecx;
			if(!pMem->fHideSel)
			{
				eax = TestExpand(pMem, pMem->line);
				eax = pMem->cpMin;
				if(eax!=pMem->cpMax)
				{
					eax = InvalidateSelection(pMem, pMem->edta.hwnd, pMem->cpMin, pMem->cpMax);
					eax = InvalidateSelection(pMem, pMem->edtb.hwnd, pMem->cpMin, pMem->cpMax);
					eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
					eax = SetCpxMax(pMem, ((RAEDT *)esi)->hwnd);
				} // endif
			} // endif
			eax = SelChange(pMem, SEL_TEXT);
			eax = 0;
			return eax;

		case REM_GETCURSORWORD:
			// wParam=BuffSize
			// lParam=lpBuff
			edi = lParam;
			*(BYTE *)edi = 0;
			eax = GetCursorPos(&pt);
			eax = ScreenToClient(hWin, &pt);
			eax = pMem->selbarwt;
			eax += pMem->linenrwt;
			if(eax<=pt.x)
			{
				eax = ChildWindowFromPoint(hWin, pt);
				if(eax==pMem->edta.hwnd)
				{
					esi = &pMem->edta;
				}
				else
				{
					esi = &pMem->edtb;
				} // endif
				eax = ClientToScreen(hWin, &pt);
				eax = ScreenToClient(((RAEDT *)esi)->hwnd, &pt);
				eax = GetCharFromPos(pMem, ((RAEDT *)esi)->cpy, pt.x, pt.y);
				temp1 = eax;
				edx = eax;
				temp2 = pt.x;
				eax = GetPosFromChar(pMem, edx, &pt);
				edx = temp2;
				eax = temp1;
				edx -= pMem->fntinfo.fntwt;
				if(edx<=pt.x)
				{
					eax = GetWordStart(pMem, eax, pMem->nCursorWordType);
					esi = pMem->rpChars;
					ecx = eax;
					ecx -= pMem->cpLine;
					temp1 = ecx;
					temp2 = eax;
					eax = GetWordEnd(pMem, eax, pMem->nCursorWordType);
					ecx = temp2;
					edx = temp1;
					eax -= ecx;
					ecx = eax;
					if(ecx>=wParam)
					{
						ecx = wParam;
						ecx--;
					} // endif
					esi += pMem->hChars;
					esi += edx;
					esi += sizeof(CHARS);
					eax = ecx;
					while(ecx > 0)
					{
						*(BYTE *)edi = *(BYTE *)esi;
						edi++;
						esi++;
						ecx--;
					}
					*(BYTE *)edi = 0;
					eax = pMem->line;
				}
				else
				{
					eax = -1;
				} // endif
			}
			else
			{
				eax = -1;
			} // endif
			return eax;

		case REM_SETSEGMENTBLOCK:
			// wParam=nLine
			// lParam=TRUE/FALSE
			edx = wParam;
			edx *= sizeof(LINE);
			if(edx<pMem->rpLineFree)
			{
				edx += pMem->hLine;
				edx = ((LINE *)edx)->rpChars;
				edx += pMem->hChars;
				if(lParam)
				{
					((CHARS *)edx)->state |= STATE_SEGMENTBLOCK;
				}
				else
				{
					((CHARS *)edx)->state &= -1 ^ STATE_SEGMENTBLOCK;
				} // endif
			} // endif
			eax = 0;
			return eax;

		case REM_GETMODE:
			// wParam=0
			// lParam=0
			eax = pMem->nMode;
			// .if pMem->fOvr
			// or		eax,MODE_OVERWRITE
			// .endif
			return eax;

		case REM_SETMODE:
			// wParam=nMode
			// lParam=0
			eax = wParam;
			if(!(eax&MODE_OVERWRITE))
			{
				pMem->fOvr = FALSE;
			}
			else
			{
				pMem->fOvr = TRUE;
			} // endif
			if(!pMem->fntinfo.monospace)
			{
				eax &= -1 ^ MODE_BLOCK;
			} // endif
			edx = pMem->nMode;
			pMem->nMode = eax;
			eax ^= edx;
			if(eax&MODE_BLOCK)
			{
				if(!pMem->fntinfo.monospace)
				{
					pMem->nMode &= -1 ^ MODE_BLOCK;
				} // endif
				if(!(pMem->nMode&MODE_BLOCK))
				{
					eax = 2;
				}
				else
				{
					eax = 3;
				} // endif
				edx = pMem->fntinfo.fntht;
				eax = CreateCaret(pMem->focus, NULL, eax, edx);
				eax = GetCaretPos(&pt);
				eax = GetCharFromPos(pMem, ((RAEDT *)esi)->cpy, pt.x, pt.y);
				pMem->cpMin = eax;
				pMem->cpMax = eax;
				if(!(pMem->nMode&MODE_BLOCK))
				{
					eax = 0;
					pMem->blrg.lnMin = eax;
					pMem->blrg.clMin = eax;
					pMem->blrg.lnMax = eax;
					pMem->blrg.clMax = eax;
				}
				else
				{
					eax = SetBlockFromCp(pMem, pMem->cpMin, FALSE);
				} // endif
				eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
				eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
				eax = InvalidateRect(pMem->edta.hwnd, NULL, TRUE);
				eax = InvalidateRect(pMem->edtb.hwnd, NULL, TRUE);
				eax = SelChange(pMem, SEL_TEXT);
			} // endif
			eax = 0;
			return eax;

		case REM_GETBLOCK:
			// wParam=0
			// lParam=lpBLOCKRANGE
			edx = lParam;
			eax = pMem->blrg.lnMin;
			((BLOCKRANGE *)edx)->lnMin = eax;
			eax = pMem->blrg.clMin;
			((BLOCKRANGE *)edx)->clMin = eax;
			eax = pMem->blrg.lnMax;
			((BLOCKRANGE *)edx)->lnMax = eax;
			eax = pMem->blrg.clMax;
			((BLOCKRANGE *)edx)->clMax = eax;
			eax = 0;
			return eax;

		case REM_SETBLOCK:
			// wParam=0
			// lParam=lpBLOCKRANGE
			if(pMem->nMode&MODE_BLOCK)
			{
				eax = GetBlockRects(pMem, &oldrects);
				edx = lParam;
				eax = ((BLOCKRANGE *)edx)->lnMin;
				pMem->blrg.lnMin = eax;
				eax = ((BLOCKRANGE *)edx)->clMin;
				pMem->blrg.clMin = eax;
				eax = ((BLOCKRANGE *)edx)->lnMax;
				pMem->blrg.lnMax = eax;
				eax = ((BLOCKRANGE *)edx)->clMax;
				pMem->blrg.clMax = eax;
				eax = GetBlockCp(pMem, pMem->blrg.lnMin, pMem->blrg.clMin);
				pMem->cpMin = eax;
				pMem->cpMax = eax;
				eax = InvalidateBlock(pMem, &oldrects);
				eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
				eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
				eax = SelChange(pMem, SEL_TEXT);
			} // endif
			eax = 0;
			return eax;

		case REM_BLOCKINSERT:
			if(pMem->nMode&MODE_BLOCK)
			{
				eax = IsSelectionLocked(pMem, pMem->cpMin, pMem->cpMax);
				if(eax!=0)
				{
					goto ErrBeep;
				} // endif
				nUndoid++;
				eax = pMem->blrg.lnMin;
				edx = pMem->blrg.lnMax;
				if(eax<edx)
				{
					temp1 = eax;
					eax = edx;
					edx = temp1;
				} // endif
				eax -= edx;
				eax++;
				edi = eax;
				eax = strlen(lParam);
				esi = eax;
				eax += 2;
				eax *= edi;
				eax++;
				eax = xGlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, eax);
				temp1 = eax;
				edx = eax;
				while(edi)
				{
					temp2 = esi;
					ecx = lParam;
					while(esi)
					{
						RBYTE_LOW(eax) = *(BYTE *)ecx;
						*(BYTE *)edx = RBYTE_LOW(eax);
						ecx++;
						edx++;
						esi--;
					} // endw
					*(BYTE *)edx = 0x0D;
					edx++;
					*(BYTE *)edx = 0x0A;
					edx++;
					esi = temp2;
					edi--;
				} // endw
				eax = temp1;
				temp1 = eax;
				eax = Paste(pMem, pMem->focus, eax);
				eax = temp1;
				eax = GlobalFree(eax);
				nUndoid++;
				eax = InvalidateEdit(pMem, pMem->edta.hwnd);
				eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
			} // endif
			eax = 0;
			return eax;

		case REM_LOCKUNDOID:
			if(wParam)
			{
				eax = nUndoid;
				eax++;
				pMem->lockundoid = eax;
				eax++;
				nUndoid = eax;
			}
			else
			{
				pMem->lockundoid = 0;
			} // endif
			eax = 0;
			return eax;

		case REM_ADDBLOCKDEF:
			eax = SetBlockDef(lParam);
			eax = 0;
			return eax;

		case REM_CONVERT:
			if(!(pMem->nMode&MODE_BLOCK))
			{
				eax = IsSelectionLocked(pMem, pMem->cpMin, pMem->cpMax);
				if(eax!=0)
				{
					goto ErrBeep;
				} // endif
				if(wParam==CONVERT_TABTOSPACE || wParam==CONVERT_SPACETOTAB)
				{
					eax = ConvertIndent(pMem, wParam);
				}
				else
				{
					eax = ConvertCase(pMem, wParam);
				} // endif
				eax = InvalidateEdit(pMem, pMem->edta.hwnd);
				eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
				eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
			} // endif
			eax = 0;
			return eax;

		case REM_BRACKETMATCH:
			if(lParam)
			{
				edx = lParam;
				ecx = bracketstart;
				while(*(BYTE *)edx && *(BYTE *)edx!=',')
				{
					RBYTE_LOW(eax) = *(BYTE *)edx;
					*(BYTE *)ecx = RBYTE_LOW(eax);
					edx++;
					ecx++;
				} // endw
				*(BYTE *)ecx = 0;
				ecx = bracketend;
				if(*(BYTE *)edx==',')
				{
					edx++;
					while(*(BYTE *)edx && *(BYTE *)edx!=',')
					{
						RBYTE_LOW(eax) = *(BYTE *)edx;
						*(BYTE *)ecx = RBYTE_LOW(eax);
						edx++;
						ecx++;
					} // endw
				} // endif
				*(BYTE *)ecx = 0;
				ecx = bracketcont;
				if(*(BYTE *)edx==',')
				{
					edx++;
					while(*(BYTE *)edx)
					{
						RBYTE_LOW(eax) = *(BYTE *)edx;
						*(BYTE *)ecx = RBYTE_LOW(eax);
						edx++;
						ecx++;
					} // endw
				} // endif
				*(BYTE *)ecx = 0x0FF;
				eax = 0;
			}
			else
			{
				eax = pMem->cpMin;
				if(eax==pMem->cpMax)
				{
					eax = GetChar(pMem, pMem->cpMin);
					eax = BracketMatch(pMem, eax, pMem->cpMin);
				} // endif
			} // endif
			return eax;

		case REM_COMMAND:
			eax = GetFocus();
			if(eax==pMem->edta.hwnd || eax==pMem->edtb.hwnd)
			{
				ecx = wParam;
				fAlt = 0;
				fControl = 0;
				fShift = 0;
				if(ecx&CMD_ALT)
				{
					fAlt = TRUE;
				} // endif
				if(ecx&CMD_CTRL)
				{
					fControl = TRUE;
				} // endif
				if(ecx&CMD_SHIFT)
				{
					fShift = TRUE;
				} // endif
				ecx = RBYTE_LOW(ecx);
				eax = EditFunc(eax, ecx, fAlt, fShift, fControl);
			} // endif
			eax = 0;
			return eax;

		case REM_CASEWORD:
			// wParam=cp
			// lParam=lpBuff
			edx = wParam;
			eax = GetWordStart(pMem, edx, 0);
			esi = pMem->rpChars;
			eax -= pMem->cpLine;
			esi += pMem->hChars;
			ecx = ((CHARS *)esi)->len;
			esi += eax;
			ecx -= eax;
			esi += sizeof(CHARS);
			edi = lParam;
			while(*(BYTE *)edi && R_SIGNED(ecx) >= 0)
			{
				RBYTE_LOW(eax) = *(BYTE *)edi;
				*(BYTE *)esi = RBYTE_LOW(eax);
				edi++;
				esi++;
				ecx--;
			} // endw
			eax = 0;
			return eax;

		case REM_GETBLOCKEND:
			// wParam=nLine
			// lParam=0
			eax = CollapseGetEnd(pMem, wParam);
			return eax;

		case REM_SETLOCK:
			// wParam=TRUE/FALSE
			// lParam=0
			if(wParam)
			{
				eax = TRUE;
			}
			else
			{
				eax = 0;
			} // endif
			pMem->fLock = eax;
			eax = CheckDlgButton(hWin, -5, eax);
			eax = 0;
			return eax;

		case REM_GETLOCK:
			// wParam=0
			// lParam=0
			eax = pMem->fLock;
			return eax;

		case REM_GETWORDFROMPOS:
			// wParam=cp
			// lParam=lpBuff
			eax = GetWordStart(pMem, wParam, 0);
			esi = pMem->rpChars;
			ecx = eax;
			ecx -= pMem->cpLine;
			temp1 = ecx;
			temp2 = eax;
			eax = GetWordEnd(pMem, eax, 0);
			ecx = temp2;
			edx = temp1;
			eax -= ecx;
			ecx = eax;
			edi = lParam;
			esi += pMem->hChars;
			esi += edx;
			esi += sizeof(CHARS);
			eax = ecx;
			while(ecx > 0)
			{
				*(BYTE *)edi = *(BYTE *)esi;
				edi++;
				esi++;
				ecx--;
			}
			*(BYTE *)edi = 0;
			return eax;

		case REM_SETNOBLOCKLINE:
			// wParam=Line
			// lParam=TRUE/FALSE
			eax = NoBlockLine(pMem, wParam, lParam);
			return eax;

		case REM_ISLINENOBLOCK:
			// wParam=Line
			// lParam=0
			eax = IsLineNoBlock(pMem, wParam);
			if(eax)
			{
				eax = TRUE;
			} // endif
			return eax;

		case REM_SETALTHILITELINE:
			// wParam=nLine
			// lParam=TRUE/FALSE
			eax = AltHiliteLine(pMem, wParam, lParam);
			return eax;

		case REM_ISLINEALTHILITE:
			// wParam=nLine
			// lParam=0
			eax = IsLineAltHilite(pMem, wParam);
			if(eax)
			{
				eax = TRUE;
			} // endif
			return eax;

		case REM_SETCURSORWORDTYPE:
			// wParam=Type
			// lParam=0
			eax = wParam;
			pMem->nCursorWordType = eax;
			return eax;

		case REM_SETBREAKPOINT:
			// wParam=nLine
			// lParam=TRUE/FALSE
			eax = SetBreakpoint(pMem, wParam, lParam);
			eax = InvalidateLine(pMem, pMem->edta.hwnd, wParam);
			eax = InvalidateLine(pMem, pMem->edtb.hwnd, wParam);
			return eax;

		case REM_NEXTBREAKPOINT:
			// wParam=nLine
			// lParam=0
			eax = NextBreakpoint(pMem, wParam);
			return eax;

		case REM_GETLINESTATE:
			// wParam=nLine
			// lParam=0
			eax = GetLineState(pMem, wParam);
			return eax;

		case REM_SETERROR:
			// wParam=nLine
			// lParam=nErrID
			eax = SetError(pMem, wParam, lParam);
			eax = InvalidateLine(pMem, pMem->edta.hwnd, wParam);
			eax = InvalidateLine(pMem, pMem->edtb.hwnd, wParam);
			return eax;
		case REM_GETERROR:
			// wParam=nLine
			// lParam=0
			eax = GetError(pMem, wParam);
			return eax;

		case REM_NEXTERROR:
			// wParam=nLine
			// lParam=0
			eax = NextError(pMem, wParam);
			return eax;

		case REM_CHARTABINIT:
			// wParam=0
			// lParam=0
			esi = CharTabInit;
			edi = CharTab;
			ecx = 256;
			while(ecx > 0)
			{
				*(BYTE *)edi = *(BYTE *)esi;
				edi++;
				esi++;
				ecx--;
			}
			return eax;

		case REM_LINEREDTEXT:
			// wParam=nLine
			// lParam=TRUE/FALSE
			eax = SetRedText(pMem, wParam, lParam);
			eax = InvalidateLine(pMem, pMem->edta.hwnd, wParam);
			eax = InvalidateLine(pMem, pMem->edtb.hwnd, wParam);
			return eax;

		case REM_SETSTYLEEX:
			eax = wParam;
			pMem->fstyleex = eax;
			eax = InvalidateRect(hWin, NULL, TRUE);
			return eax;

		case REM_GETUNICODE:
			eax = pMem->funicode;
			return eax;

		case REM_SETUNICODE:
			eax = wParam;
			pMem->funicode = eax;
			return eax;

		case REM_SETCHANGEDSTATE:
			eax = SetChangedState(pMem, wParam);
			return eax;

		case REM_SETTOOLTIP:
			eax = wParam;
			if(eax==1)
			{
				eax = pMem->hsta;
			}
			else if(eax==2)
			{
				eax = pMem->hsbtn;
			}
			else if(eax==3)
			{
				eax = pMem->hlin;
			}
			else if(eax==4)
			{
				eax = pMem->hexp;
			}
			else if(eax==5)
			{
				eax = pMem->hcol;
			}
			else if(eax==6)
			{
				eax = pMem->hlock;
			}
			else
			{
				eax = 0;
			} // endif
			if(eax)
			{
				edx = lParam;
				SetToolTip();
			} // endif
			return eax;

		case REM_HILITEACTIVELINE:
			eax = lParam;
			pMem->fhilite = eax;
			eax = HiliteLine(pMem, pMem->line, pMem->fhilite);
			return eax;

		case REM_GETUNDO:
			eax = GetUndo(pMem, wParam, lParam);
			return eax;

		case REM_SETUNDO:
			eax = SetUndo(pMem, wParam, lParam);
			return eax;

		case REM_GETLINEBEGIN:
			eax = GetLineBegin(pMem, wParam);
			return eax;


			// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		} // END OF: RAEdit_REM_Msg_JumpTable.asm
	} // endif
	if(eax>=EM_CANPASTE && eax<=EM_SETZOOM)
	{
		eax -= EM_CANPASTE;
		if(eax>35)
		{
			eax = DefWindowProc(hWin, uMsg, wParam, lParam);
			return eax;
		} // endif
		switch(eax + EM_CANPASTE)
		{
			// RAEdit_Richedit_Msg_JumpTable.asm
			// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

		case EM_EXLINEFROMCHAR:
			// wParam=0
			// lParam=cp
			eax = GetCharPtr(pMem, lParam, &ecx, &edx);
			eax = edx;
			return eax;

		case EM_EXSETSEL:
			// wParam=0
			// lParam=lpCHARRANGE
			if(!(pMem->nMode&MODE_BLOCK))
			{
				if(!pMem->fHideSel)
				{
					eax = pMem->cpMin;
					if(eax!=pMem->cpMax)
					{
						eax = InvalidateSelection(pMem, pMem->edta.hwnd, pMem->cpMin, pMem->cpMax);
						eax = InvalidateSelection(pMem, pMem->edtb.hwnd, pMem->cpMin, pMem->cpMax);
					} // endif
				} // endif
				edi = lParam;
				eax = GetCharPtr(pMem, ((CHARRANGE *)edi)->cpMax, &ecx, &edx);
				pMem->cpMax = ecx;
				eax = GetCharPtr(pMem, ((CHARRANGE *)edi)->cpMin, &ecx, &edx);
				pMem->cpMin = ecx;
				temp1 = edx;
				if(!pMem->fHideSel)
				{
					eax = TestExpand(pMem, pMem->line);
					eax = pMem->cpMin;
					if(eax!=pMem->cpMax)
					{
						eax = InvalidateSelection(pMem, pMem->edta.hwnd, pMem->cpMin, pMem->cpMax);
						eax = InvalidateSelection(pMem, pMem->edtb.hwnd, pMem->cpMin, pMem->cpMax);
						eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
						eax = SetCpxMax(pMem, ((RAEDT *)esi)->hwnd);
					} // endif
				} // endif
				eax = SelChange(pMem, SEL_TEXT);
				eax = temp1;
			}
			else
			{
				eax = 0;
				eax--;
			} // endif
			return eax;

		case EM_EXGETSEL:
			// wParam=0
			// lParam=lpCHARRANGE
			edx = lParam;
			eax = pMem->cpMin;
			ecx = pMem->cpMax;
			if(eax>ecx)
			{
				temp1 = eax;
				eax = ecx;
				ecx = temp1;
			} // endif
			((CHARRANGE *)edx)->cpMin = eax;
			((CHARRANGE *)edx)->cpMax = ecx;
			eax = 0;
			return eax;

		case EM_FINDTEXTEX:
			// wParam=Flags
			// lParam=lpFINDTEXTEX
			if(!(pMem->nMode&MODE_BLOCK))
			{
				eax = FindTextEx(pMem, wParam, lParam);
			}
			else
			{
				eax = 0;
				eax--;
			} // endif
			return eax;

		case EM_GETTEXTRANGE:
			// wParam=0
			// lParam=lpTEXTRANGE
			edx = lParam;
			eax = GetText(pMem, ((TEXTRANGE *)edx)->chrg.cpMin, ((TEXTRANGE *)edx)->chrg.cpMax, ((TEXTRANGE *)edx)->lpstrText, FALSE);
			return eax;

		case EM_FINDWORDBREAK:
			// wParam=uFlags
			// lParam=cp
			eax = wParam;
			if(eax==WB_MOVEWORDLEFT)
			{
				eax = GetWordStart(pMem, lParam, 0);
			}
			else if(eax==WB_MOVEWORDRIGHT)
			{
				eax = GetWordEnd(pMem, lParam, 0);
			}
			else
			{
				eax = lParam;
			} // endif
			return eax;

		case EM_CANREDO:
			// wParam=0
			// lParam=0
			eax = 0;
			if(!(pMem->nMode&MODE_BLOCK))
			{
				edx = pMem->hUndo;
				edx += pMem->rpUndo;
				eax = ((RAUNDO *)edx)->cb;
				if(eax)
				{
					eax = TRUE;
				} // endif
			} // endif
			return eax;

		case EM_REDO:
			// wParam=0
			// lParam=0
			nUndoid++;
			eax = Redo(esi, pMem, ((RAEDT *)esi)->hwnd);
			nUndoid++;
			return eax;

		case EM_HIDESELECTION:
			// wParam=TRUE/FALSE
			// lParam=0
			eax = wParam;
			pMem->fHideSel = eax;
			return eax;

		case EM_GETSELTEXT:
			// wParam=0
			// lParam=lpBuff
			eax = GetText(pMem, pMem->cpMin, pMem->cpMax, lParam, FALSE);
			return eax;

		case EM_CANPASTE:
			// wParam=CF_TEXT
			// lParam=0
			eax = IsClipboardFormatAvailable(wParam);
			return eax;

		case EM_STREAMIN:
			// wParam=SF_TEXT
			// lParam=lpStream
			eax = StreamIn(pMem, lParam);
			eax = 0;
			pMem->edta.cpy = eax;
			pMem->edta.cpxmax = eax;
			pMem->edta.topyp = eax;
			pMem->edta.topln = eax;
			pMem->edta.topcp = eax;
			pMem->edtb.cpy = eax;
			pMem->edtb.cpxmax = eax;
			pMem->edtb.topyp = eax;
			pMem->edtb.topln = eax;
			pMem->edtb.topcp = eax;
			pMem->cpMin = eax;
			pMem->cpMax = eax;
			pMem->blrg.lnMin = eax;
			pMem->blrg.clMin = eax;
			pMem->blrg.lnMax = eax;
			pMem->blrg.clMax = eax;
			pMem->line = eax;
			pMem->cpx = eax;
			pMem->cpLine = eax;
			pMem->rpLine = eax;
			pMem->rpChars = eax;
			eax = GetCharPtr(pMem, 0, &ecx, &edx);
			eax = InvalidateEdit(pMem, pMem->edta.hwnd);
			eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
			eax = SetCaret(pMem, 0);
			eax = SelChange(pMem, SEL_TEXT);
			nUndoid++;
			eax = 0;
			return eax;

		case EM_STREAMOUT:
			// wParam=SF_TEXT
			// lParam=lpStream
			eax = StreamOut(pMem, lParam);
			eax = 0;
			return eax;

		case EM_FORMATRANGE:
			edi = lParam;
			if(edi)
			{
				eax = GetStockObject(SYSTEM_FONT);
				eax = SelectObject(((FORMATRANGE *)edi)->hdc, eax);
				temp1 = eax;
				edx = eax;
				eax = GetObject(edx, sizeof(lf), &lf);
				eax = temp1;
				eax = SelectObject(((FORMATRANGE *)edi)->hdc, eax);
				eax = GetDeviceCaps(((FORMATRANGE *)edi)->hdc, LOGPIXELSY);
				temp1 = eax;
				eax = GetDeviceCaps(((FORMATRANGE *)edi)->hdcTarget, LOGPIXELSY);
				ecx = eax;
				eax = lf.lfHeight;
				if(R_SIGNED(eax) < 0)
				{
					eax = -eax;
				} // endif
				eax *= ecx;
				ecx = temp1;
				eax /= ecx;
				// neg		eax
				lf.lfHeight = eax;

				eax = GetDeviceCaps(((FORMATRANGE *)edi)->hdc, LOGPIXELSX);
				temp1 = eax;
				eax = GetDeviceCaps(((FORMATRANGE *)edi)->hdcTarget, LOGPIXELSX);
				ecx = eax;
				eax = lf.lfWidth;
				eax *= ecx;
				ecx = temp1;
				eax /= ecx;
				lf.lfWidth = eax;
				eax = CreateFontIndirect(&lf);
				eax = SelectObject(((FORMATRANGE *)edi)->hdcTarget, eax);
				temp1 = eax;
				// Get tab width
				eax = 0x57575757; // 'WWWW'
				pt.x = eax;
				eax = GetTextExtentPoint32(((FORMATRANGE *)edi)->hdcTarget, &pt.x, 4, &pt);
				eax = pt.x;
				eax /= 4;
				ecx = pMem->nTab;
				eax *= ecx;
				tabWt = eax;
				eax = xGlobalAlloc(GMEM_FIXED, 16384);
				esi = eax;
				eax = ConvTwipsToPixels(((FORMATRANGE *)edi)->hdcTarget, TRUE, ((FORMATRANGE *)edi)->rc.left);
				rect.left = eax;
				eax = ConvTwipsToPixels(((FORMATRANGE *)edi)->hdcTarget, FALSE, ((FORMATRANGE *)edi)->rc.top);
				rect.top = eax;
				eax = ConvTwipsToPixels(((FORMATRANGE *)edi)->hdcTarget, TRUE, ((FORMATRANGE *)edi)->rc.right);
				rect.right = eax;
				eax = ConvTwipsToPixels(((FORMATRANGE *)edi)->hdcTarget, FALSE, ((FORMATRANGE *)edi)->rc.bottom);
				rect.bottom = eax;
				eax = CreateRectRgn(rect.left, rect.top, rect.right, rect.bottom);
				eax = SelectObject(((FORMATRANGE *)edi)->hdcTarget, eax);
				temp2 = eax;
				eax = SendMessage(hWin, EM_LINEFROMCHAR, ((FORMATRANGE *)edi)->chrg.cpMin, 0);
				nLine = eax;
anon_8:
				*(WORD *)esi = 16383;
				eax = SendMessage(hWin, EM_GETLINE, nLine, esi);
				len = eax;
				eax = rect.top;
				eax += pt.y;
				if(eax<rect.bottom)
				{
					if(wParam)
					{
						eax = TabbedTextOut(((FORMATRANGE *)edi)->hdcTarget, rect.left, rect.top, esi, len, 1, &tabWt, rect.left);
					} // endif
					eax = pt.y;
					rect.top += eax;
					nLine++;
					eax = SendMessage(hWin, EM_LINEINDEX, nLine, 0);
					if(eax<((FORMATRANGE *)edi)->chrg.cpMax)
					{
						goto anon_8;
					} // endif
				} // endif
				eax = GlobalFree(esi);
				eax = temp2;
				eax = SelectObject(((FORMATRANGE *)edi)->hdcTarget, eax);
				eax = DeleteObject(eax);
				eax = temp1;
				eax = SelectObject(((FORMATRANGE *)edi)->hdcTarget, eax);
				eax = DeleteObject(eax);
				eax = SendMessage(hWin, EM_LINEINDEX, nLine, 0);
			} // endif
			return eax;

		default:
			eax = DefWindowProc(hWin, uMsg, wParam, lParam);
			return eax;


			// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		} // END OF: RAEdit_Richedit_Msg_JumpTable.asm
	} // endif
	if(eax>=EM_GETSEL && eax<=EM_GETIMESTATUS)
	{
		eax -= EM_GETSEL;
		switch(eax + EM_GETSEL)
		{
			// RAEdit_Edit_Msg_JumpTable.asm
			// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

		case EM_CHARFROMPOS:
			// wParam=0
			// lParam=lpPoint
			edx = lParam;
			eax = GetCharFromPos(pMem, ((RAEDT *)esi)->cpy, ((POINT *)edx)->x, ((POINT *)edx)->y);
			return eax;

		case EM_POSFROMCHAR:
			// wParam=lpPoint
			// lParam=cp
			eax = GetPosFromChar(pMem, lParam, wParam);
			edx = wParam;
			eax = pMem->cpx;
			((POINT *)edx)->x -= eax;
			eax = ((RAEDT *)esi)->cpy;
			((POINT *)edx)->y -= eax;
			eax = 0;
			return eax;

		case EM_LINEFROMCHAR:
			// wParam=cp
			// lParam=0
			eax = wParam;
			if(eax==-1)
			{
				eax = pMem->cpMin;
			} // endif
			eax = GetCharPtr(pMem, eax, &ecx, &edx);
			eax = edx;
			return eax;

		case EM_LINEINDEX:
			// wParam=line
			// lParam=0
			eax = wParam;
			if(eax==-1)
			{
				eax = pMem->line;
			} // endif
			eax = GetCpFromLine(pMem, eax);
			return eax;

		case EM_GETLINE:
			// wParam=line
			// lParam=lpBuff
			edx = wParam;
			edx *= sizeof(LINE);
			if(edx<pMem->rpLineFree)
			{
				edx += pMem->hLine;
				edx = ((LINE *)edx)->rpChars;
				edx += pMem->hChars;
				ecx = ((CHARS *)edx)->len;
				if(*(BYTE *)(edx+ecx+sizeof(CHARS)-1)==VK_RETURN && ecx)
				{
					ecx--;
				} // endif
				edi = lParam;
				if(RWORD(ecx)>*(WORD *)edi)
				{
					ecx = *(WORD *)edi;
				} // endif
				temp1 = ecx;
				esi = edx+sizeof(CHARS);
				while(ecx > 0)
				{
					*(BYTE *)edi = *(BYTE *)esi;
					edi++;
					esi++;
					ecx--;
				}
				eax = temp1;
			}
			else
			{
				eax = 0;
			} // endif
			return eax;

		case EM_LINELENGTH:
			// wParam=cp
			// lParam=0
			eax = GetLineFromCp(pMem, wParam);
			edx = eax;
			edx *= sizeof(LINE);
			edx += pMem->hLine;
			edx = ((LINE *)edx)->rpChars;
			edx += pMem->hChars;
			eax = ((CHARS *)edx)->len;
			if(eax)
			{
				if(*(BYTE *)(edx+eax+sizeof(CHARS)-1)==0x0D)
				{
					eax--;
				} // endif
			} // endif
			return eax;

		case EM_GETFIRSTVISIBLELINE:
			// wParam=0
			// lParam=0
			eax = ((RAEDT *)esi)->topln;
			return eax;

		case EM_LINESCROLL:
			// wParam=cxScroll
			// lParam=cyScroll
			eax = wParam;
			edx = pMem->fntinfo.fntwt;
			if(R_SIGNED(eax) < 0)
			{
				eax = -eax;
				eax *= edx;
				eax = -eax;
			}
			else
			{
				eax *= edx;
			} // endif
			eax += pMem->cpx;
			pMem->cpx = eax;
			eax = lParam;
			edx = pMem->fntinfo.fntht;
			if(R_SIGNED(eax) < 0)
			{
				eax = -eax;
				eax *= edx;
				eax = -eax;
			}
			else
			{
				eax *= edx;
			} // endif
			eax += ((RAEDT *)esi)->cpy;
			if(R_SIGNED(eax) < 0)
			{
				eax = 0;
			} // endif
			((RAEDT *)esi)->cpy = eax;
			eax = InvalidateEdit(pMem, ((RAEDT *)esi)->hwnd);
			eax = TRUE;
			return eax;

		case EM_SCROLLCARET:
			// wParam=0
			// lParam=0
			eax = SetCaretVisible(((RAEDT *)esi)->hwnd, ((RAEDT *)esi)->cpy);
			eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
			eax = TRUE;
			return eax;

		case EM_SETSEL:
			// wParam=cpMin
			// lParam=cpMax
			if(!(pMem->nMode&MODE_BLOCK))
			{
				if(!pMem->fHideSel)
				{
					eax = pMem->cpMin;
					if(eax!=pMem->cpMax)
					{
						eax = InvalidateSelection(pMem, pMem->edta.hwnd, pMem->cpMin, pMem->cpMax);
						eax = InvalidateSelection(pMem, pMem->edtb.hwnd, pMem->cpMin, pMem->cpMax);
					} // endif
				} // endif
				eax = GetCharPtr(pMem, lParam, &ecx, &edx);
				pMem->cpMax = ecx;
				eax = GetCharPtr(pMem, wParam, &ecx, &edx);
				pMem->cpMin = ecx;
				if(!pMem->fHideSel)
				{
					eax = TestExpand(pMem, pMem->line);
					eax = pMem->cpMin;
					if(eax!=pMem->cpMax)
					{
						eax = InvalidateSelection(pMem, pMem->edta.hwnd, pMem->cpMin, pMem->cpMax);
						eax = InvalidateSelection(pMem, pMem->edtb.hwnd, pMem->cpMin, pMem->cpMax);
						eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
						eax = SetCpxMax(pMem, ((RAEDT *)esi)->hwnd);
					} // endif
				} // endif
				eax = SelChange(pMem, SEL_TEXT);
			} // endif
			eax = 0;
			return eax;

		case EM_GETSEL:
			// wParam=lpcpMin
			// lParam=lpcpMax
			eax = pMem->cpMin;
			ecx = pMem->cpMax;
			if(eax>ecx)
			{
				temp1 = eax;
				eax = ecx;
				ecx = temp1;
			} // endif
			edx = wParam;
			if(edx)
			{
				*(DWORD *)edx = eax;
			} // endif
			edx = lParam;
			if(edx)
			{
				*(DWORD *)edx = ecx;
			} // endif
			eax &= 0x0FFFF;
			ecx <<= 16;
			eax |= ecx;
			return eax;

		case EM_GETMODIFY:
			// wParam=0
			// lParam=0
			eax = pMem->fChanged;
			return eax;

		case EM_SETMODIFY:
			// wParam=TRUE/FALSE
			// lParam=0
			eax = wParam;
			pMem->fChanged = eax;
			eax = InvalidateRect(pMem->hsta, NULL, TRUE);
			return eax;

		case EM_REPLACESEL:
			// wParam=TRUE/FALSE
			// lParam=lpText
			if(!wParam)
			{
				fNoSaveUndo = TRUE;
			} // endif
			eax = IsSelectionLocked(pMem, pMem->cpMin, pMem->cpMax);
			if(eax!=0)
			{
				goto ErrBeep;
			} // endif
			nUndoid++;
			eax = DeleteSelection(pMem, pMem->cpMin, pMem->cpMax);
			pMem->cpMin = eax;
			pMem->cpMax = eax;
			temp1 = eax;
			temp2 = pMem->fOvr;
			pMem->fOvr = 0;
			eax = EditInsert(pMem, pMem->cpMin, lParam);
			pMem->fOvr = temp2;
			ecx = temp1;
			pMem->cpMin += eax;
			pMem->cpMax += eax;
			if(wParam && eax)
			{
				eax = SaveUndo(pMem, UNDO_INSERTBLOCK, ecx, lParam, eax);
			} // endif
			eax = GetCharPtr(pMem, pMem->cpMin, &ecx, &edx);
			eax = SetCaretVisible(((RAEDT *)esi)->hwnd, ((RAEDT *)esi)->cpy);
			eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
			eax = InvalidateEdit(pMem, pMem->edta.hwnd);
			eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
			eax = SetCpxMax(pMem, ((RAEDT *)esi)->hwnd);
			eax = SelChange(pMem, SEL_TEXT);
			nUndoid++;
			fNoSaveUndo = FALSE;
			eax = 0;
			return eax;

		case EM_GETLINECOUNT:
			// wParam=0
			// lParam=0
			eax = pMem->rpLineFree;
			eax /= sizeof(LINE);
			eax--;
			return eax;

		case EM_GETRECT:
			// wParam=0
			// lParam=lpRECT
			edx = lParam;
			eax = ((RAEDT *)esi)->rc.left;
			((RECT *)edx)->left = eax;
			eax = ((RAEDT *)esi)->rc.top;
			((RECT *)edx)->top = eax;
			eax = ((RAEDT *)esi)->rc.right;
			((RECT *)edx)->right = eax;
			eax = ((RAEDT *)esi)->rc.bottom;
			((RECT *)edx)->bottom = eax;
			eax = pMem->focus;
			if(eax==pMem->edtb.hwnd && pMem->nsplitt)
			{
				eax = pMem->nsplitt;
				eax += BTNHT;
				((RECT *)edx)->top += eax;
				((RECT *)edx)->bottom += eax;
			} // endif
			return eax;

		case EM_CANUNDO:
			// wParam=0
			// lParam=0
			eax = 0;
			if(!(pMem->nMode&MODE_BLOCK))
			{
				eax = pMem->rpUndo;
				if(eax)
				{
					eax = TRUE;
				} // endif
			} // endif
			return eax;

		case EM_UNDO:
			// wParam=0
			// lParam=0
			nUndoid++;
			eax = Undo(esi, pMem, ((RAEDT *)esi)->hwnd);
			nUndoid++;
			return eax;

		case EM_EMPTYUNDOBUFFER:
			// wParam=0
			// lParam=0
			edi = pMem->hUndo;
			ecx = pMem->cbUndo;
			eax = 0;
			pMem->rpUndo = eax;
			while(ecx > 0)
			{
				*(BYTE *)edi = RBYTE_LOW(eax);
				edi++;
				ecx--;
			}
			nUndoid++;
			return eax;

		default:
			eax = DefWindowProc(hWin, uMsg, wParam, lParam);
			return eax;


			// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		} // END OF: RAEdit_Edit_Msg_JumpTable.asm
	} // endif
	if(eax==WM_COPY)
	{
		// wParam=0
		// lParam=0
		eax = Copy(pMem);
		eax = 0;
		goto Ex;
	}
	else if(eax==WM_CUT)
	{
		// wParam=0
		// lParam=0
		nUndoid++;
		eax = Cut(pMem, ((RAEDT *)esi)->hwnd);
		nUndoid++;
		goto Ex;
	}
	else if(eax==WM_PASTE)
	{
		// wParam=0
		// lParam=0
		eax = IsSelectionLocked(pMem, pMem->cpMin, pMem->cpMax);
		if(eax!=0)
		{
			goto ErrBeep;
		} // endif
		nUndoid++;
		eax = Paste(pMem, ((RAEDT *)esi)->hwnd, NULL);
		nUndoid++;
		eax = 0;
		goto Ex;
	}
	else if(eax==WM_CLEAR)
	{
		// wParam=0
		// lParam=0
		eax = IsSelectionLocked(pMem, pMem->cpMin, pMem->cpMax);
		if(eax!=0)
		{
			goto ErrBeep;
		} // endif
		nUndoid++;
		if(!(pMem->nMode&MODE_BLOCK))
		{
			eax = DeleteSelection(pMem, pMem->cpMin, pMem->cpMax);
			pMem->cpMin = eax;
			pMem->cpMax = eax;
			eax = GetCharPtr(pMem, pMem->cpMin, &ecx, &edx);
			eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
			eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
			eax = InvalidateRect(pMem->edta.hwnd, NULL, FALSE);
			eax = InvalidateRect(pMem->edtb.hwnd, NULL, FALSE);
		}
		else
		{
			eax = DeleteSelectionBlock(pMem, pMem->blrg.lnMin, pMem->blrg.clMin, pMem->blrg.lnMax, pMem->blrg.clMax);
			eax = pMem->blrg.clMin;
			if(eax>pMem->blrg.clMax)
			{
				eax = pMem->blrg.clMax;
			} // endif
			edx = pMem->blrg.lnMin;
			if(edx>pMem->blrg.lnMax)
			{
				edx = pMem->blrg.lnMax;
			} // endif
			pMem->blrg.clMin = eax;
			pMem->blrg.lnMin = edx;
			pMem->blrg.clMax = eax;
			pMem->blrg.lnMax = edx;
			eax = GetBlockCp(pMem, edx, eax);
			pMem->cpMin = eax;
			pMem->cpMax = eax;
			eax = GetCharPtr(pMem, pMem->cpMin, &ecx, &edx);
			eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
			eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
			eax = InvalidateEdit(pMem, pMem->edta.hwnd);
			eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
		} // endif
		eax = SetCpxMax(pMem, ((RAEDT *)esi)->hwnd);
		eax = SelChange(pMem, SEL_TEXT);
		nUndoid++;
		eax = 0;
		goto Ex;
	}
	else if(eax==WM_SETTEXT)
	{
		// wParam=0
		// lParam=lpszText
		RelMem();
		AllocMem();
		eax = EditInsert(pMem, 0, lParam);
		eax = 0;
		pMem->edta.cpy = eax;
		pMem->edta.cpxmax = eax;
		pMem->edta.topyp = eax;
		pMem->edta.topln = eax;
		pMem->edta.topcp = eax;
		pMem->edtb.cpy = eax;
		pMem->edtb.cpxmax = eax;
		pMem->edtb.topyp = eax;
		pMem->edtb.topln = eax;
		pMem->edtb.topcp = eax;
		pMem->cpMin = eax;
		pMem->cpMax = eax;
		pMem->blrg.lnMin = eax;
		pMem->blrg.clMin = eax;
		pMem->blrg.lnMax = eax;
		pMem->blrg.clMax = eax;
		pMem->line = eax;
		pMem->cpx = eax;
		pMem->cpLine = eax;
		pMem->rpLine = eax;
		pMem->rpChars = eax;
		eax = GetCharPtr(pMem, 0, &ecx, &edx);
		eax = InvalidateEdit(pMem, pMem->edta.hwnd);
		eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
		eax = SetCaret(pMem, 0);
		eax = SelChange(pMem, SEL_TEXT);
		nUndoid++;
		eax = TRUE;
		goto Ex;
	}
	else if(eax==WM_GETTEXT)
	{
		// wParam=cbBuff
		// lParam=lpBuff
		eax = wParam;
		edi = lParam;
		if(eax)
		{
			chrg.cpMin = 0;
			eax--;
			chrg.cpMax = eax;
			edx = pMem->hLine;
			eax = edx;
			eax += pMem->rpLineFree;
			while(edx<eax)
			{
				esi = ((LINE *)edx)->rpChars;
				esi += pMem->hChars;
				ecx = ((CHARS *)esi)->len;
				esi += sizeof(CHARS);
				if(ecx>chrg.cpMax)
				{
					ecx = chrg.cpMax;
					chrg.cpMin += ecx;
					while(ecx > 0)
					{
						*(BYTE *)edi = *(BYTE *)esi;
						edi++;
						esi++;
						ecx--;
					}
					break;
				} // endif
				chrg.cpMin += ecx;
				while(ecx > 0)
				{
					*(BYTE *)edi = *(BYTE *)esi;
					edi++;
					esi++;
					ecx--;
				}
				edx += sizeof(LINE);
			} // endw
			*(BYTE *)edi = 0;
			eax = chrg.cpMin;
		} // endif
		goto Ex;
	}
	else if(eax==WM_GETTEXTLENGTH)
	{
		// wParam=0
		// lParam=0
		edx = pMem->hLine;
		edi = edx;
		edi += pMem->rpLineFree;
		eax = ((RAEDT *)esi)->topln;
		eax *= sizeof(LINE);
		edx += eax;
		eax = ((RAEDT *)esi)->topcp;
		while(edx<edi)
		{
			ecx = ((LINE *)edx)->rpChars;
			ecx += pMem->hChars;
			eax += ((CHARS *)ecx)->len;
			edx += sizeof(LINE);
		} // endw
		goto Ex;
	}
	else if(eax==WM_MOUSEMOVE)
	{
		// Get mouse position
		eax = lParam;
		edx = eax;
		eax &= 0x0FFFF;
		edx >>= 16;
		if(pMem->fresize==1)
		{
			pMem->fresize = 2;
		}
		else if(pMem->fresize==2)
		{
			eax = lParam;
			eax >>= 16;
			R_SIGNED(eax) = RWORD_SIGNED(eax);
			eax -= 2;
			edx = pMem->rc.bottom;
			edx -= 30;
			if(R_SIGNED(eax) < 0)
			{
				eax = 0;
			}
			else if(R_SIGNED(eax) > R_SIGNED(edx))
			{
				eax = edx;
			} // endif
			if(eax)
			{
				eax <<= 10;
				ecx = pMem->rc.bottom;
				eax /= ecx;
			} // endif
			pMem->fsplitt = eax;
			SizeIt();
			eax = UpdateWindow(pMem->hsbtn);
			eax = UpdateWindow(pMem->edta.hwnd);
			eax = UpdateWindow(pMem->edtb.hwnd);
		} // endif
		eax = 0;
		goto Ex;
	}
	else if(eax==WM_LBUTTONDOWN)
	{
		eax = 0;
		goto Ex;
	}
	else if(eax==WM_LBUTTONUP)
	{
		if(pMem->fresize)
		{
			pMem->fresize = 0;
			eax = ReleaseCapture();
			SizeIt();
			if(pMem->fsplitt==0)
			{
				eax = SetFocus(pMem->edtb.hwnd);
			} // endif
		} // endif
		eax = 0;
		goto Ex;
	}
	else if(eax==WM_LBUTTONDBLCLK)
	{
		eax = 0;
		goto Ex;
	}
	else if(eax==WM_CREATE)
	{
		eax = GetSystemMetrics(SM_CXVSCROLL);
		SBWT = eax;
		// Allocate memory for the edit control
		eax = GetProcessHeap();
		edx = sizeof(EDIT);
		edx >>= 3;
		edx++;
		edx <<= 3;
		eax = xHeapAlloc(eax, HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY, edx);
		pMem = eax;
		// Save the pointer
		eax = SetWindowLongPtr(hWin, 0, pMem);
		eax = hWin;
		pMem->hwnd = eax;
		eax = GetParent(eax);
		pMem->hpar = eax;
		eax = GetWindowLong(hWin, GWL_STYLE);
		pMem->fstyle = eax;
		eax = GetWindowLongPtr(pMem->hwnd, GWLP_ID);
		pMem->ID = eax;
		AllocMem();
		pMem->nlinenrwt = LNRWT;
		pMem->linenrwt = 0;
		pMem->selbarwt = SELWT;
		pMem->fIndent = TRUE;
		pMem->nTab = 4;
		pMem->nScroll = 3;
		pMem->fntinfo.fntht = 10;
		pMem->clr.bckcol = BCKCLR;
		pMem->clr.txtcol = TXTCLR;
		pMem->clr.selbckcol = SELBCKCLR;
		pMem->clr.seltxtcol = SELTXTCLR;
		pMem->clr.cmntcol = CMNTCLR;
		pMem->clr.strcol = STRCLR;
		pMem->clr.oprcol = OPRCLR;
		pMem->clr.hicol1 = HILITE1;
		pMem->clr.hicol2 = HILITE2;
		pMem->clr.hicol3 = HILITE3;
		pMem->clr.selbarbck = SELBARCLR;
		pMem->clr.selbarpen = SELBARPEN;
		pMem->clr.lnrcol = LNRCLR;
		pMem->clr.numcol = NUMCLR;
		pMem->clr.cmntback = CMNTBCK;
		pMem->clr.strback = STRBCK;
		pMem->clr.numback = NUMBCK;
		pMem->clr.oprback = OPRBCK;
		pMem->clr.changed = CHANGEDCLR;
		pMem->clr.changesaved = CHANGESAVEDCLR;
		eax = CreateBrushes(pMem);

		eax = CreateWindowEx(NULL, szToolTips, NULL, TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
		pMem->htt = eax;

		eax = CreateWindowEx(1, szStatic, NULL, SS_NOTIFY | WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWin, -1, hInstance, 0);
		pMem->hsbtn = eax;
		edx = szSplitterBar;
		SetToolTip();
		eax = SetWindowLongPtr(pMem->hsbtn, GWLP_WNDPROC, &SplittBtnProc);
		OldSplittBtnProc = eax;

		eax = CreateWindowEx(NULL, szEditClassName, NULL, WS_CLIPSIBLINGS | WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWin, NULL, hInstance, 0);
		pMem->edta.hwnd = eax;
		eax = CreateWindowEx(NULL, szScrollBar, NULL, WS_CHILD | WS_VISIBLE | SBS_VERT, 0, 0, 0, 0, pMem->edta.hwnd, NULL, hInstance, 0);
		pMem->edta.hvscroll = eax;

		eax = CreateWindowEx(NULL, szEditClassName, NULL, WS_CLIPSIBLINGS | WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWin, NULL, hInstance, 0);
		pMem->edtb.hwnd = eax;
		eax = CreateWindowEx(NULL, szScrollBar, NULL, WS_CHILD | WS_VISIBLE | SBS_VERT, 0, 0, 0, 0, pMem->edtb.hwnd, NULL, hInstance, 0);
		pMem->edtb.hvscroll = eax;

		eax = CreateWindowEx(NULL, szScrollBar, NULL, WS_CHILD | WS_VISIBLE | SBS_HORZ, 0, 0, 0, 0, hWin, NULL, hInstance, 0);
		pMem->hhscroll = eax;
		eax = CreateWindowEx(NULL, szScrollBar, NULL, WS_CHILD | WS_VISIBLE | SBS_SIZEGRIP, 0, 0, 0, 0, hWin, NULL, hInstance, 0);
		pMem->hgrip = eax;

		eax = CreateWindowEx(NULL, szStatic, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWin, NULL, hInstance, 0);
		pMem->hnogrip = eax;

		eax = CreateWindowEx(NULL, szStatic, NULL, SS_NOTIFY | WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWin, NULL, hInstance, 0);
		pMem->hsta = eax;
		edx = szChanged;
		SetToolTip();
		eax = SetWindowLongPtr(pMem->hsta, GWLP_USERDATA, pMem);
		eax = SetWindowLongPtr(pMem->hsta, GWLP_WNDPROC, &StateProc);
		OldStateProc = eax;

		eax = CreateWindowEx(NULL, szStatic, NULL, WS_POPUP | WS_BORDER | SS_OWNERDRAW, 0, 0, 0, 0, hWin, NULL, hInstance, 0);
		pMem->htlt = eax;

		eax = SetWindowLongPtr(eax, GWLP_WNDPROC, FakeToolTipProc);
		OldFakeToolTipProc = eax;
		eax = SendMessage(pMem->htt, WM_GETFONT, 0, 0);
		eax = SendMessage(pMem->htlt, WM_SETFONT, eax, FALSE);

		eax = CreateWindowEx(NULL, szButton, NULL, BS_BITMAP | BS_PUSHLIKE | BS_AUTOCHECKBOX | WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWin, -2, hInstance, 0);
		pMem->hlin = eax;
		edx = szLineNumber;
		SetToolTip();
		eax = SendMessage(pMem->hlin, BM_SETIMAGE, IMAGE_BITMAP, hBmpLnr);

		eax = CreateWindowEx(NULL, szButton, NULL, BS_BITMAP | BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWin, -3, hInstance, 0);
		pMem->hexp = eax;
		edx = szExpand;
		SetToolTip();
		eax = SendMessage(pMem->hexp, BM_SETIMAGE, IMAGE_BITMAP, hBmpExp);

		eax = CreateWindowEx(NULL, szButton, NULL, BS_BITMAP | BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWin, -4, hInstance, 0);
		pMem->hcol = eax;
		edx = szCollapse;
		SetToolTip();
		eax = SendMessage(pMem->hcol, BM_SETIMAGE, IMAGE_BITMAP, hBmpCol);

		eax = CreateWindowEx(NULL, szButton, NULL, BS_BITMAP | BS_PUSHLIKE | BS_AUTOCHECKBOX | WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWin, -5, hInstance, 0);
		pMem->hlock = eax;
		edx = szLock;
		SetToolTip();
		eax = SendMessage(pMem->hlock, BM_SETIMAGE, IMAGE_BITMAP, hBmpLck);

		eax = SetWindowPos(pMem->hsta, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOSIZE); // SWP_NOREPOSITION
		eax = SetWindowPos(pMem->hsbtn, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOSIZE); // SWP_NOREPOSITION
		if(pMem->fstyle&STYLE_DRAGDROP)
		{
			eax = RegisterDragDrop(pMem->edta.hwnd, &DropTarget);
			eax = RegisterDragDrop(pMem->edtb.hwnd, &DropTarget);
		} // endif
	}
	else if(eax==WM_DESTROY)
	{
		if(pMem->fstyle&STYLE_DRAGDROP)
		{
			eax = RevokeDragDrop(pMem->edta.hwnd);
			eax = RevokeDragDrop(pMem->edtb.hwnd);
		} // endif
		eax = SetWindowLongPtr(hWin, 0, 0);
		eax = DestroyWindow(pMem->htt);
		eax = DestroyWindow(pMem->hsbtn);
		eax = DestroyWindow(pMem->edta.hvscroll);
		eax = DestroyWindow(pMem->edtb.hvscroll);
		eax = DestroyWindow(pMem->hhscroll);
		eax = DestroyWindow(pMem->hgrip);
		eax = DestroyWindow(pMem->hnogrip);
		eax = DestroyWindow(pMem->hsta);
		eax = DestroyWindow(pMem->hlin);
		eax = DestroyWindow(pMem->hexp);
		eax = DestroyWindow(pMem->hcol);
		eax = DestroyWindow(pMem->hlock);
		eax = DestroyWindow(pMem->edta.hwnd);
		eax = DestroyWindow(pMem->edtb.hwnd);
		eax = DestroyBrushes(pMem);
		// Free memory
		RelMem();
		eax = GetProcessHeap();
		eax = HeapFree(eax, 0, pMem);
	}
	else if(eax==WM_SETFOCUS)
	{
		eax = pMem->edta.hwnd;
		if(eax!=pMem->focus)
		{
			eax = pMem->edtb.hwnd;
		} // endif
		eax = SetFocus(eax);
		eax = 0;
		goto Ex;
	}
	else if(eax==WM_KEYDOWN || eax==WM_CHAR || eax==WM_KEYUP)
	{
		eax = SendMessage(pMem->focus, eax, wParam, lParam);
		goto Ex;
	}
	else if(eax==WM_COMMAND)
	{
		eax = wParam;
		eax &= 0x0FFFF;
		if(RWORD(eax)==(WORD)-2)
		{
			eax = IsDlgButtonChecked(hWin, -2);
			if(eax)
			{
				eax = pMem->nlinenrwt;
				pMem->linenrwt = eax;
			}
			else
			{
				pMem->linenrwt = 0;
			} // endif
			eax = SetFocus(pMem->focus);
			SizeIt();
			eax = InvalidateEdit(pMem, pMem->edta.hwnd);
			eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
		}
		else if(RWORD(eax)==(WORD)-3)
		{
			ecx = BN_CLICKED;
			ecx <<= 16;
			eax |= ecx;
			eax = SendMessage(pMem->hpar, WM_COMMAND, eax, pMem->hexp);
			eax = SetFocus(pMem->focus);
		}
		else if(RWORD(eax)==(WORD)-4)
		{
			ecx = BN_CLICKED;
			ecx <<= 16;
			eax |= ecx;
			eax = SendMessage(pMem->hpar, WM_COMMAND, eax, pMem->hcol);
			eax = SetFocus(pMem->focus);
		}
		else if(RWORD(eax)==(WORD)-5)
		{
			ecx = pMem->fLock;
			ecx ^= 1;
			pMem->fLock = ecx;
			ecx = BN_CLICKED;
			ecx <<= 16;
			eax |= ecx;
			eax = SendMessage(pMem->hpar, WM_COMMAND, eax, pMem->hlock);
			eax = SetFocus(pMem->focus);
		} // endif
	}
	else if(eax==WM_SIZE)
	{
		eax = GetWindowLong(hWin, GWL_STYLE);
		pMem->fstyle = eax;
		eax = GetClientRect(hWin, & pMem->rc);
		eax = pMem->fstyle;
		eax &= STYLE_NOLINENUMBER | STYLE_NOCOLLAPSE | STYLE_NOHSCROLL;
		edx = pMem->fstyleex;
		edx &= STYLEEX_LOCK;
		if(eax!=(STYLE_NOLINENUMBER | STYLE_NOCOLLAPSE | STYLE_NOHSCROLL) || edx==STYLEEX_LOCK)
		{
			eax = SBWT;
			pMem->rc.bottom -= eax;
		} // endif
		SizeIt();
		edi = 0;
		if(!(pMem->fstyle&STYLE_NOLINENUMBER))
		{
			eax = MoveWindow(pMem->hlin, 0, pMem->rc.bottom, BTNWT, SBWT, TRUE);
			edi += BTNWT;
		}
		else
		{
			eax = MoveWindow(pMem->hlin, 0, 0, 0, 0, TRUE);
		} // endif
		if(!(pMem->fstyle&STYLE_NOCOLLAPSE))
		{
			eax = MoveWindow(pMem->hexp, edi, pMem->rc.bottom, BTNWT, SBWT, TRUE);
			edi += BTNWT;
			eax = MoveWindow(pMem->hcol, edi, pMem->rc.bottom, BTNWT, SBWT, TRUE);
			edi += BTNWT;
		}
		else
		{
			eax = MoveWindow(pMem->hexp, 0, 0, 0, 0, TRUE);
			eax = MoveWindow(pMem->hcol, 0, 0, 0, 0, TRUE);
		} // endif
		if(pMem->fstyleex&STYLEEX_LOCK)
		{
			eax = MoveWindow(pMem->hlock, edi, pMem->rc.bottom, BTNWT, SBWT, TRUE);
			edi += BTNWT;
		}
		else
		{
			eax = MoveWindow(pMem->hlock, 0, 0, 0, 0, TRUE);
		} // endif
		eax = pMem->fstyle;
		eax &= STYLE_NOLINENUMBER | STYLE_NOCOLLAPSE | STYLE_NOHSCROLL;
		edx = pMem->fstyleex;
		edx &= STYLEEX_LOCK;
		if(eax!=(STYLE_NOLINENUMBER | STYLE_NOCOLLAPSE | STYLE_NOHSCROLL) || edx==STYLEEX_LOCK)
		{
			ecx = pMem->rc.right;
			ecx -= edi;
			eax = pMem->fstyle;
			eax &= STYLE_NOSPLITT | STYLE_NOVSCROLL;
			if(eax!=(STYLE_NOSPLITT | STYLE_NOVSCROLL))
			{
				ecx -= SBWT;
				eax = MoveWindow(pMem->hhscroll, edi, pMem->rc.bottom, ecx, SBWT, TRUE);
				if(pMem->fstyle&STYLE_NOSIZEGRIP)
				{
					goto anon_9;
				} // endif
				eax = GetWindowLong(pMem->hpar, GWL_STYLE);
				if(eax&WS_MAXIMIZE)
				{
					goto anon_9;
				} // endif
				eax = GetClientRect(pMem->hpar, &rect);
				eax = ClientToScreen(pMem->hpar, &rect.right);
				eax = pMem->rc.bottom;
				eax += SBWT;
				rect.top = eax;

				eax = ClientToScreen(hWin, &rect);
				eax = rect.bottom;
				eax -= rect.top;
				if(eax<5)
				{
					eax = pMem->rc.right;
					eax -= SBWT;
					eax = MoveWindow(pMem->hgrip, eax, pMem->rc.bottom, SBWT, SBWT, TRUE);
					eax = MoveWindow(pMem->hnogrip, 0, 0, 0, 0, TRUE);
				}
				else
				{
anon_9:
					eax = pMem->rc.right;
					eax -= SBWT;
					eax = MoveWindow(pMem->hnogrip, eax, pMem->rc.bottom, SBWT, SBWT, TRUE);
					eax = MoveWindow(pMem->hgrip, 0, 0, 0, 0, TRUE);
				} // endif
			}
			else
			{
				eax = MoveWindow(pMem->hhscroll, edi, pMem->rc.bottom, ecx, SBWT, TRUE);
				eax = MoveWindow(pMem->hnogrip, 0, 0, 0, 0, TRUE);
				eax = MoveWindow(pMem->hgrip, 0, 0, 0, 0, TRUE);
			} // endif
		}
		else
		{
			eax = MoveWindow(pMem->hhscroll, 0, 0, 0, 0, TRUE);
			eax = MoveWindow(pMem->hgrip, 0, 0, 0, 0, TRUE);
			eax = MoveWindow(pMem->hnogrip, 0, 0, 0, 0, TRUE);
		} // endif
		eax = GetTopFromYp(pMem, pMem->edta.hwnd, pMem->edta.cpy);
		eax = GetTopFromYp(pMem, pMem->edtb.hwnd, pMem->edtb.cpy);
		eax = 0;
		goto Ex;
	}
	else if(eax==WM_HSCROLL)
	{
		temp1 = pMem->cpx;
		sinf.cbSize = sizeof(sinf);
		sinf.fMask = SIF_ALL;
		eax = GetScrollInfo(lParam, SB_CTL, &sinf);
		eax = wParam;
		eax &= 0x0FFFF;
		if(eax==SB_THUMBTRACK || eax==SB_THUMBPOSITION)
		{
			eax = sinf.nTrackPos;
			pMem->cpx = eax;
		}
		else if(eax==SB_LINELEFT)
		{
			eax = pMem->fntinfo.fntwt;
			if(pMem->cpx>eax)
			{
				pMem->cpx -= eax;
			}
			else
			{
				pMem->cpx = 0;
			} // endif
		}
		else if(eax==SB_LINERIGHT)
		{
			eax = pMem->fntinfo.fntwt;
			pMem->cpx += eax;
		}
		else if(eax==SB_PAGELEFT)
		{
			eax = sinf.nPage;
			if(pMem->cpx > eax)
			{
				pMem->cpx -= eax;
			}
			else
			{
				pMem->cpx = 0;
			} // endif
		}
		else if(eax==SB_PAGERIGHT)
		{
			eax = sinf.nPage;
			pMem->cpx += eax;
		} // endif
		edi = temp1;
		edi -= pMem->cpx;
		eax = ScrollEdit(pMem, pMem->edta.hwnd, edi, 0);
		eax = ScrollEdit(pMem, pMem->edtb.hwnd, edi, 0);
	}
	else if(eax==WM_SETFONT)
	{
		eax = wParam;
		pMem->fnt.hFont = eax;
		pMem->fnt.hIFont = eax;
		pMem->fnt.hLnrFont = eax;
		eax = SetFont(pMem, & pMem->fnt);
	}
	else if(eax==WM_GETFONT)
	{
		eax = pMem->fnt.hFont;
		return eax;
	}
	else if(eax==WM_STYLECHANGED)
	{
		eax = GetWindowLong(hWin, GWL_STYLE);
		pMem->fstyle = eax;
	}
	else if(eax==WM_PAINT)
	{
		eax = UpdateWindow(pMem->hsta);
		eax = UpdateWindow(pMem->edta.hwnd);
		eax = UpdateWindow(pMem->edtb.hwnd);
		goto ExDef;
	}
	else if(eax==REM_RAINIT)
	{
		// WM_USER+9999 (=REM_RAINIT) is sendt to a custom control by RadASM (1.2.0.5)
		// to let the custom control fill in default design time values.
		eax = SendMessage(hWin, WM_SETTEXT, 0, szToolTip);
		eax = 0;
		goto Ex;
	} // endif
ExDef:
	eax = DefWindowProc(hWin, uMsg, wParam, lParam);
Ex:
	return eax;

ErrBeep:
	eax = MessageBeep(MB_ICONHAND);
	eax = 0;
	return eax;

	void SetToolTip(void)
	{
		ti.cbSize = sizeof(TOOLINFO);
		ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
		ti.hwnd = 0;
		ti.uId = eax;
		ti.hinst = 0;
		ti.lpszText = edx;
		eax = SendMessage(pMem->htt, TTM_DELTOOL, NULL, &ti);
		eax = SendMessage(pMem->htt, TTM_ADDTOOL, NULL, &ti);
		return;

	}

	void SizeIt(void)
	{
		eax = pMem->fsplitt;
		if(eax)
		{
			ecx = eax;
			eax = pMem->rc.bottom;
			eax *= ecx;
			eax >>= 10;
		} // endif
		pMem->nsplitt = eax;
		if(pMem->nsplitt)
		{
			eax = MoveWindow(pMem->edta.hwnd, 0, 0, pMem->rc.right, pMem->nsplitt, TRUE);
			ecx = pMem->nsplitt;
			ecx += BTNHT;
			eax = pMem->rc.bottom;
			eax -= ecx;
			eax = MoveWindow(pMem->edtb.hwnd, 0, ecx, pMem->rc.right, eax, TRUE);
			eax = MoveWindow(pMem->hsbtn, 0, pMem->nsplitt, pMem->rc.right, BTNHT, TRUE);
		}
		else
		{
			eax = MoveWindow(pMem->edta.hwnd, 0, 0, 0, 0, TRUE);
			ecx = pMem->rc.right;
			if(R_SIGNED(ecx) < 0)
			{
				ecx = 0;
			} // endif

			edx = pMem->rc.bottom;
			if(R_SIGNED(edx) < 0)
			{
				edx = 0;
			} // endif
			eax = MoveWindow(pMem->edtb.hwnd, 0, 0, ecx, edx, TRUE);
			eax = pMem->rc.right;
			eax -= SBWT;
			if(!(pMem->fstyle&STYLE_NOSPLITT))
			{
				eax = MoveWindow(pMem->hsbtn, eax, 0, SBWT, BTNHT, TRUE);
			}
			else
			{
				eax = MoveWindow(pMem->hsbtn, 0, 0, 0, 0, TRUE);
			} // endif
			if(!(pMem->fstyle&STYLE_NOSTATE))
			{
				eax = MoveWindow(pMem->hsta, 0, 0, 4, 4, TRUE);
			}
			else
			{
				eax = MoveWindow(pMem->hsta, 0, 0, 0, 0, TRUE);
			} // endif
		} // endif
		return;

	}

	void AllocMem(void)
	{
		eax = HeapCreate(HEAP_GENERATE_EXCEPTIONS, 256*1024, 0);
		pMem->hHeap = eax;
		// Line
		eax = xHeapAlloc(pMem->hHeap, HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY, MAXLINEMEM*8);
		pMem->hLine = eax;
		pMem->cbLine = MAXLINEMEM*8;
		pMem->rpLine = 0;
		pMem->rpLineFree = sizeof(LINE);
		// Chars
		eax = xHeapAlloc(pMem->hHeap, HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY, MAXCHARMEM*8);
		pMem->hChars = eax;
		pMem->cbChars = MAXCHARMEM*8;
		pMem->rpChars = 0;
		pMem->rpCharsFree = MAXFREE+sizeof(CHARS);
		edx = pMem->hLine;
		((LINE *)edx)->rpChars = 0;
		edx = pMem->hChars;
		((CHARS *)edx)->max = MAXFREE;
		((CHARS *)edx)->len = 0;
		((CHARS *)edx)->state = 0;
		// Undo
		eax = xHeapAlloc(pMem->hHeap, HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY, MAXUNDOMEM);
		pMem->hUndo = eax;
		pMem->cbUndo = MAXUNDOMEM;
		pMem->rpUndo = 0;
		// Misc
		eax = 0;
		pMem->edta.cpy = eax;
		pMem->edta.cpxmax = eax;
		pMem->edtb.cpy = eax;
		pMem->edtb.cpxmax = eax;
		pMem->cpx = eax;
		pMem->cpMin = eax;
		pMem->cpMax = eax;
		pMem->blrg.lnMin = eax;
		pMem->blrg.clMin = eax;
		pMem->blrg.lnMax = eax;
		pMem->blrg.clMax = eax;
		pMem->fChanged = FALSE;
		pMem->cpbrst = -1;
		pMem->cpbren = -1;
		pMem->cpselbar = -1;
		pMem->fLock = FALSE;

		return;

	}

	void RelMem(void)
	{
		// Free memory
		if(pMem->hHeap)
		{
			eax = HeapDestroy(pMem->hHeap);
			pMem->hHeap = 0;
		} // endif
		return;

	}

} // RAWndProc

