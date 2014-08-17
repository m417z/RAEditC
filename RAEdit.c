#include <windows.h>
#include "Data.h"

REG_T TimerProc(DWORD hWin, DWORD uMsg, DWORD idEvent, DWORD dwTime)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

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
REG_T InstallRAEdit(HINSTANCE hInst, DWORD fGlobal)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	WNDCLASSEX wc;
	DWORD hBmp;

	temp1 = hInst;
	hInstance = temp1;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	if(fGlobal)
	{
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_GLOBALCLASS;
	} // endif
	wc.lpfnWndProc = &RAWndProc;
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = 4; // Holds memory handle
	temp1 = hInst;
	wc.hInstance = temp1;
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = &szRAEditClass;
	eax = NULL;
	wc.hIcon = eax;
	wc.hIconSm = eax;
	eax = LoadCursor(NULL, IDC_ARROW);
	wc.hCursor = eax;
	eax = RegisterClassEx(&wc);

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = &RAEditProc;
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = 4; // Holds memory handle
	temp1 = hInst;
	wc.hInstance = temp1;
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = &szEditClassName;
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
	temp1 = ebx;
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
	ebx = temp1;
	return eax;

} // InstallRAEdit

REG_T UnInstallRAEdit(void)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	eax = DestroyCursor(hHSCur);
	eax = DestroyCursor(hSelCur);
	eax = ImageList_Destroy(hIml);
	eax = DeleteObject(hBmpLnr);
	eax = DeleteObject(hBmpCol);
	eax = DeleteObject(hBmpExp);
	eax = DeleteObject(hBmpLck);
	eax = DeleteObject(hBrTlt);
	eax = GetProcessHeap();
	eax = HeapFree(eax, 0, hWrdMem);
	return eax;

} // UnInstallRAEdit

// Prefix ^		Word is case sensitive
// Prefix ~		Word is case converted
// Suffix +		Hilites rest of line with comment color
// Suffix -		Hilites rest of line with text color
// 
// nColor			gggg0sff cccccccc cccccccc cccccccc
// g=Word group, s=Case sensitive, f=Font style, c=color
REG_T SetHiliteWords(DWORD nColor, DWORD lpWords)
{
	REG_T eax = 0, ecx, edx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
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
		if((BYTE)nColor[3] & 4)
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
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
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
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
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
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	eax = CharTab;
	return eax;

} // GetCharTabPtr

REG_T GetCharTabVal(DWORD nChar)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	edx = nChar;
	edx &= 0x0FF;
	eax = *(BYTE *)(edx+CharTab);
	return eax;

} // GetCharTabVal

REG_T SetCharTabVal(DWORD nChar, DWORD nValue)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	edx = nChar;
	edx &= 0x0FF;
	eax = nValue;
	*(BYTE *)(edx+CharTab) = RBYTE_LOW(eax);
	return eax;

} // SetCharTabVal

REG_T SetBlockDef(DWORD lpRABLOCKDEF)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	auto void TestString(void);

	if(!lpRABLOCKDEF)
	{
		ecx = sizeof(blockdefs)/4;
		edi = &blockdefs;
		eax = 0;
		while(ecx > 0)
		{
			*(DWORD *)edi = eax;
			edi += 4;
			ecx--;
		}
	}
	else
	{
		ebx = lpRABLOCKDEF;
		esi = &blockdefs;
		edi = esi+32*4;
		while(*(DWORD *)esi)
		{
			eax = IsBlockDefEqual(ebx, edi);
			if(eax)
			{
				goto Ex;
			} // endif
			edi = *(DWORD *)esi;
			esi += 4;
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
		*(DWORD *)esi = ecx;
	} // endif
Ex:
	return eax;

	void TestString(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
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
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	eax = uMsg;
	if(eax==WM_MOUSEMOVE)
	{
		eax = GetParent(hWin);
		eax = GetWindowLong(eax, 0);
		ebx = eax;
		eax = SetCursor(hHSCur);
	}
	else if(eax==WM_LBUTTONDOWN)
	{
		eax = GetParent(hWin);
		eax = GetWindowLong(eax, 0);
		ebx = eax;
		((EDIT *)ebx)->fresize = 1;
		eax = SetCapture(((EDIT *)ebx)->hwnd);
		eax = SetCursor(hHSCur);
	}
	else if(eax==WM_LBUTTONDBLCLK)
	{
		eax = GetParent(hWin);
		temp1 = eax;
		eax = GetWindowLong(eax, 0);
		ebx = eax;
		eax = 511;
		if(((EDIT *)ebx)->fsplitt)
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
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	PAINTSTRUCT ps;
	DWORD hBr;

	if(uMsg==WM_PAINT)
	{
		eax = BeginPaint(hWin, &ps);
		eax = GetWindowLong(hWin, GWL_USERDATA);
		ebx = eax;
		if(((EDIT *)ebx)->fstyle&STYLE_READONLY)
		{
			eax = CreateSolidBrush(0x0FF);
			hBr = eax;
		}
		else if(((EDIT *)ebx)->fChanged)
		{
			eax = CreateSolidBrush(0x0B000);
			hBr = eax;
		}
		else
		{
			hBr = 0;
			eax = ((EDIT *)ebx)->br.hBrSelBar;
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
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	POINT pt;
	DWORD cpOldMin;
	DWORD cpOldMax;
	RECT oldrects[2];
	DWORD nOldLine;
	DWORD fSel;

	eax = GetWindowLong(hWin, 0);
	ebx = eax;
	eax = hWin;
	if(eax==((EDIT *)ebx)->edta.hwnd)
	{
		esi = &((EDIT *)ebx)->edta;
	}
	else
	{
		esi = &((EDIT *)ebx)->edtb;
	} // endif
	eax = ((EDIT *)ebx)->cpMin;
	cpOldMin = eax;
	edx = ((EDIT *)ebx)->cpMax;
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
				if(((EDIT *)ebx)->fsplitt)
				{
					if(((EDIT *)ebx)->fsplitt>10)
					{
						((EDIT *)ebx)->fsplitt -= 10;
					}
					else
					{
						((EDIT *)ebx)->fsplitt = 0;
					} // endif
					eax = SendMessage(((EDIT *)ebx)->hwnd, WM_SIZE, 0, 0);
					eax = UpdateWindow(((EDIT *)ebx)->edta.hwnd);
					eax = UpdateWindow(((EDIT *)ebx)->edtb.hwnd);
					eax = UpdateWindow(((EDIT *)ebx)->hsbtn);
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
				if(!(((EDIT *)ebx)->nMode&MODE_BLOCK))
				{
					eax = ((EDIT *)ebx)->cpMin;
					if(eax>((EDIT *)ebx)->cpMax && !fShift)
					{
						temp1 = eax;
						eax = ((EDIT *)ebx)->cpMax;
						((EDIT *)ebx)->cpMax = temp1;
						((EDIT *)ebx)->cpMin = eax;
						eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
					} // endif
					temp1 = ((EDIT *)ebx)->cpx;
					eax = GetCaretPos(&pt);
					eax = ((EDIT *)ebx)->fntinfo.fntht;
					pt.y -= eax;
					eax = ((RAEDT *)esi)->cpxmax;
					eax += ((EDIT *)ebx)->selbarwt;
					eax += ((EDIT *)ebx)->linenrwt;
					eax -= ((EDIT *)ebx)->cpx;
					eax = GetCharFromPos(ebx, ((RAEDT *)esi)->cpy, eax, pt.y);
					if(eax!=((EDIT *)ebx)->cpLine)
					{
						temp2 = eax;
						eax--;
						eax = IsCharLeadByte(ebx, eax);
						edx = temp2;
						eax += edx;
					} // endif
					((EDIT *)ebx)->cpMin = eax;
					if(!fShift)
					{
						((EDIT *)ebx)->cpMax = eax;
					} // endif
					((EDIT *)ebx)->cpx = temp1;
					eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
					eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
					if(!fShift && fSel)
					{
						eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
						eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
					}
					else if(fShift)
					{
						eax = InvalidateLine(ebx, ((EDIT *)ebx)->edta.hwnd, ((EDIT *)ebx)->line);
						eax = InvalidateLine(ebx, ((EDIT *)ebx)->edtb.hwnd, ((EDIT *)ebx)->line);
						temp1 = ((EDIT *)ebx)->line;
						eax = GetCharPtr(ebx, cpOldMin);
						eax = temp1;
						if(eax!=((EDIT *)ebx)->line)
						{
							eax = InvalidateLine(ebx, ((EDIT *)ebx)->edta.hwnd, ((EDIT *)ebx)->line);
							eax = InvalidateLine(ebx, ((EDIT *)ebx)->edtb.hwnd, ((EDIT *)ebx)->line);
						} // endif
						eax = UpdateWindow(((EDIT *)ebx)->edta.hwnd);
						eax = UpdateWindow(((EDIT *)ebx)->edtb.hwnd);
					} // endif
				}
				else
				{
					eax = GetBlockRects(ebx, &oldrects);
					eax = GetCaretPos(&pt);
					eax = pt.y;
					eax -= ((EDIT *)ebx)->fntinfo.fntht;
					eax = GetCharFromPos(ebx, ((RAEDT *)esi)->cpy, pt.x, eax);
					((EDIT *)ebx)->cpMin = eax;
					((EDIT *)ebx)->cpMax = eax;
					eax = pt.x;
					eax += ((EDIT *)ebx)->cpx;
					eax -= ((EDIT *)ebx)->linenrwt;
					eax -= ((EDIT *)ebx)->selbarwt;
					ecx = ((EDIT *)ebx)->fntinfo.fntwt;
					// xor		edx,edx
					eax /= ecx;
					edx = ((EDIT *)ebx)->line;
					((EDIT *)ebx)->blrg.lnMin = edx;
					((EDIT *)ebx)->blrg.clMin = eax;
					if(!fShift)
					{
						((EDIT *)ebx)->blrg.lnMax = edx;
						((EDIT *)ebx)->blrg.clMax = eax;
					} // endif
					eax = InvalidateBlock(ebx, &oldrects);
					eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
					eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
				} // endif
				eax = SelChange(ebx, SEL_TEXT);
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
				if(((EDIT *)ebx)->fsplitt<960)
				{
					((EDIT *)ebx)->fsplitt += 10;
					if(((EDIT *)ebx)->fsplitt>960)
					{
						((EDIT *)ebx)->fsplitt = 960;
					} // endif
					eax = SendMessage(((EDIT *)ebx)->hwnd, WM_SIZE, 0, 0);
					eax = UpdateWindow(((EDIT *)ebx)->edta.hwnd);
					eax = UpdateWindow(((EDIT *)ebx)->edtb.hwnd);
					eax = UpdateWindow(((EDIT *)ebx)->hsbtn);
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
				if(!(((EDIT *)ebx)->nMode&MODE_BLOCK))
				{
					eax = GetCharPtr(ebx, ((EDIT *)ebx)->cpMin);
					nOldLine = edx;
					eax = ((EDIT *)ebx)->cpMin;
					if(eax<((EDIT *)ebx)->cpMax && !fShift)
					{
						temp1 = eax;
						eax = ((EDIT *)ebx)->cpMax;
						((EDIT *)ebx)->cpMax = temp1;
						((EDIT *)ebx)->cpMin = eax;
						eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
					} // endif
					temp1 = ((EDIT *)ebx)->cpx;
					eax = GetCaretPos(&pt);
					eax = ((EDIT *)ebx)->fntinfo.fntht;
					pt.y += eax;
					eax = ((RAEDT *)esi)->cpxmax;
					eax += ((EDIT *)ebx)->selbarwt;
					eax += ((EDIT *)ebx)->linenrwt;
					eax -= ((EDIT *)ebx)->cpx;
					eax = GetCharFromPos(ebx, ((RAEDT *)esi)->cpy, eax, pt.y);
					temp2 = eax;
					if(eax)
					{
						eax--;
						eax = IsCharLeadByte(ebx, eax);
					} // endif
					edx = temp2;
					edx += eax;
					eax = edx;
					((EDIT *)ebx)->cpMin = eax;
					if(!fShift)
					{
						((EDIT *)ebx)->cpMax = eax;
					} // endif
					((EDIT *)ebx)->cpx = temp1;
					eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
					eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
					if(!fShift && fSel)
					{
						eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
						eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
					}
					else if(fShift)
					{
						eax = InvalidateLine(ebx, ((EDIT *)ebx)->edta.hwnd, ((EDIT *)ebx)->line);
						eax = InvalidateLine(ebx, ((EDIT *)ebx)->edtb.hwnd, ((EDIT *)ebx)->line);
						eax = nOldLine;
						if(eax!=((EDIT *)ebx)->line)
						{
							eax = InvalidateLine(ebx, ((EDIT *)ebx)->edta.hwnd, nOldLine);
							eax = InvalidateLine(ebx, ((EDIT *)ebx)->edtb.hwnd, nOldLine);
						} // endif
						eax = UpdateWindow(((EDIT *)ebx)->edta.hwnd);
						eax = UpdateWindow(((EDIT *)ebx)->edtb.hwnd);
					} // endif
				}
				else
				{
					eax = GetBlockRects(ebx, &oldrects);
					eax = GetCaretPos(&pt);
					eax = pt.y;
					eax += ((EDIT *)ebx)->fntinfo.fntht;
					eax = GetCharFromPos(ebx, ((RAEDT *)esi)->cpy, pt.x, eax);
					((EDIT *)ebx)->cpMin = eax;
					((EDIT *)ebx)->cpMax = eax;
					eax = pt.x;
					eax += ((EDIT *)ebx)->cpx;
					eax -= ((EDIT *)ebx)->linenrwt;
					eax -= ((EDIT *)ebx)->selbarwt;
					ecx = ((EDIT *)ebx)->fntinfo.fntwt;
					// xor		edx,edx
					eax /= ecx;
					edx = ((EDIT *)ebx)->line;
					((EDIT *)ebx)->blrg.lnMin = edx;
					((EDIT *)ebx)->blrg.clMin = eax;
					if(!fShift)
					{
						((EDIT *)ebx)->blrg.lnMax = edx;
						((EDIT *)ebx)->blrg.clMax = eax;
					} // endif
					eax = InvalidateBlock(ebx, &oldrects);
					eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
					eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
				} // endif
				eax = SelChange(ebx, SEL_TEXT);
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
				if(((EDIT *)ebx)->cpx>20)
				{
					((EDIT *)ebx)->cpx -= 20;
				}
				else
				{
					((EDIT *)ebx)->cpx = 0;
				} // endif
				eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
				eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
				eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
			} // endif
		}
		else
		{
			if(!(((EDIT *)ebx)->nMode&MODE_BLOCK))
			{
anon_3:
				eax = ((EDIT *)ebx)->cpMin;
				if(eax>((EDIT *)ebx)->cpMax && !fShift)
				{
					eax = ((EDIT *)ebx)->cpMax;
				}
				else if(eax>=((EDIT *)ebx)->cpMax || fShift)
				{
					if(eax)
					{
						if(eax>=2)
						{
							temp1 = eax;
							eax -= 2;
							eax = IsCharLeadByte(ebx, eax);
							edx = temp1;
							edx -= eax;
							eax = edx;
						} // endif
						eax--;
					} // endif
				} // endif
				if(fControl)
				{
					eax = SkipWhiteSpace(ebx, eax, TRUE);
					eax = GetWordStart(ebx, eax, 0);
				} // endif
				eax = GetCharPtr(ebx, eax);
				((EDIT *)ebx)->cpMin = ecx;
				if(!fShift)
				{
					((EDIT *)ebx)->cpMax = ecx;
				} // endif
				eax = IsLineHidden(ebx, edx);
				if(eax!=0)
				{
					goto anon_3;
				} // endif
				eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
				eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
				if(!fShift && fSel)
				{
					eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
					eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
				}
				else if(fShift)
				{
					eax = InvalidateLine(ebx, ((EDIT *)ebx)->edta.hwnd, ((EDIT *)ebx)->line);
					eax = InvalidateLine(ebx, ((EDIT *)ebx)->edtb.hwnd, ((EDIT *)ebx)->line);
					temp1 = ((EDIT *)ebx)->line;
					eax = GetCharPtr(ebx, cpOldMin);
					eax = temp1;
					if(eax!=((EDIT *)ebx)->line)
					{
						eax = InvalidateLine(ebx, ((EDIT *)ebx)->edta.hwnd, ((EDIT *)ebx)->line);
						eax = InvalidateLine(ebx, ((EDIT *)ebx)->edtb.hwnd, ((EDIT *)ebx)->line);
					} // endif
					eax = UpdateWindow(((EDIT *)ebx)->edta.hwnd);
					eax = UpdateWindow(((EDIT *)ebx)->edtb.hwnd);
				} // endif
			}
			else
			{
				eax = GetBlockRects(ebx, &oldrects);
				eax = GetCaretPos(&pt);
				eax = pt.x;
				eax -= ((EDIT *)ebx)->fntinfo.fntwt;
				eax = GetCharFromPos(ebx, ((RAEDT *)esi)->cpy, eax, pt.y);
				((EDIT *)ebx)->cpMin = eax;
				((EDIT *)ebx)->cpMax = eax;
				eax = pt.x;
				eax -= ((EDIT *)ebx)->fntinfo.fntwt;
				eax += ((EDIT *)ebx)->cpx;
				eax -= ((EDIT *)ebx)->linenrwt;
				eax -= ((EDIT *)ebx)->selbarwt;
				ecx = ((EDIT *)ebx)->fntinfo.fntwt;
				// cdq
				R_SIGNED(eax) /= TO_R_SIGNED(ecx);
				if(R_SIGNED(eax) < 0)
				{
					eax = 0;
				} // endif
				edx = ((EDIT *)ebx)->line;
				((EDIT *)ebx)->blrg.lnMin = edx;
				((EDIT *)ebx)->blrg.clMin = eax;
				if(!fShift)
				{
					((EDIT *)ebx)->blrg.lnMax = edx;
					((EDIT *)ebx)->blrg.clMax = eax;
				} // endif
				eax = InvalidateBlock(ebx, &oldrects);
				eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
				eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
			} // endif
			eax = SetCpxMax(ebx, hWin);
			eax = SelChange(ebx, SEL_TEXT);
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
				((EDIT *)ebx)->cpx += 20;
				eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
				eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
				eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
			} // endif
		}
		else
		{
			if(!(((EDIT *)ebx)->nMode&MODE_BLOCK))
			{
anon_4:
				eax = ((EDIT *)ebx)->cpMin;
				if(eax<((EDIT *)ebx)->cpMax && !fShift)
				{
					eax = ((EDIT *)ebx)->cpMax;
				}
				else if(eax<=((EDIT *)ebx)->cpMax || fShift)
				{
					temp1 = eax;
					eax = IsCharLeadByte(ebx, eax);
					edx = temp1;
					edx++;
					edx += eax;
					eax = edx;
				} // endif
				if(fControl)
				{
					eax = SkipWhiteSpace(ebx, eax, FALSE);
					eax = GetWordEnd(ebx, eax, 0);
				} // endif
				eax = GetCharPtr(ebx, eax);
				eax = ((EDIT *)ebx)->cpMin;
				((EDIT *)ebx)->cpMin = ecx;
				if(!fShift)
				{
					((EDIT *)ebx)->cpMax = ecx;
				} // endif
				eax = edx;
				eax++;
				eax *= 4;
				if(eax<((EDIT *)ebx)->rpLineFree)
				{
					eax = IsLineHidden(ebx, edx);
					if(eax!=0)
					{
						goto anon_4;
					} // endif
				} // endif
				eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
				eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
				if(!fShift && fSel)
				{
					eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
					eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
				}
				else if(fShift)
				{
					eax = InvalidateLine(ebx, ((EDIT *)ebx)->edta.hwnd, ((EDIT *)ebx)->line);
					eax = InvalidateLine(ebx, ((EDIT *)ebx)->edtb.hwnd, ((EDIT *)ebx)->line);
					temp1 = ((EDIT *)ebx)->line;
					eax = GetCharPtr(ebx, cpOldMin);
					eax = temp1;
					if(eax!=((EDIT *)ebx)->line)
					{
						eax = InvalidateLine(ebx, ((EDIT *)ebx)->edta.hwnd, ((EDIT *)ebx)->line);
						eax = InvalidateLine(ebx, ((EDIT *)ebx)->edtb.hwnd, ((EDIT *)ebx)->line);
					} // endif
					eax = UpdateWindow(((EDIT *)ebx)->edta.hwnd);
					eax = UpdateWindow(((EDIT *)ebx)->edtb.hwnd);
				} // endif
			}
			else
			{
				eax = GetBlockRects(ebx, &oldrects);
				eax = GetCaretPos(&pt);
				eax = pt.x;
				eax += ((EDIT *)ebx)->fntinfo.fntwt;
				eax = GetCharFromPos(ebx, ((RAEDT *)esi)->cpy, eax, pt.y);
				((EDIT *)ebx)->cpMin = eax;
				((EDIT *)ebx)->cpMax = eax;
				eax = pt.x;
				eax += ((EDIT *)ebx)->fntinfo.fntwt;
				eax += ((EDIT *)ebx)->cpx;
				eax -= ((EDIT *)ebx)->linenrwt;
				eax -= ((EDIT *)ebx)->selbarwt;
				ecx = ((EDIT *)ebx)->fntinfo.fntwt;
				// cdq
				R_SIGNED(eax) /= TO_R_SIGNED(ecx);
				if(R_SIGNED(eax) < 0)
				{
					eax = 0;
				} // endif
				edx = ((EDIT *)ebx)->line;
				((EDIT *)ebx)->blrg.lnMin = edx;
				((EDIT *)ebx)->blrg.clMin = eax;
				if(!fShift)
				{
					((EDIT *)ebx)->blrg.lnMax = edx;
					((EDIT *)ebx)->blrg.clMax = eax;
				} // endif
				eax = InvalidateBlock(ebx, &oldrects);
				eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
				eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
			} // endif
			eax = SetCpxMax(ebx, hWin);
			eax = SelChange(ebx, SEL_TEXT);
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
				eax = SendMessage(((EDIT *)ebx)->hpar, WM_COMMAND, eax, ((EDIT *)ebx)->hcol);
				eax = SetFocus(((EDIT *)ebx)->focus);
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
				if(!(((EDIT *)ebx)->nMode&MODE_BLOCK))
				{
					eax = ((EDIT *)ebx)->cpMin;
					temp1 = eax;
					if(eax>((EDIT *)ebx)->cpMax && !fShift)
					{
						temp2 = eax;
						eax = ((EDIT *)ebx)->cpMax;
						((EDIT *)ebx)->cpMax = temp2;
						((EDIT *)ebx)->cpMin = eax;
						eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
					} // endif
					eax = GetCaretPos(&pt);
					eax = ((RAEDT *)esi)->rc.bottom;
					ecx = ((EDIT *)ebx)->fntinfo.fntht;
					// xor		edx,edx
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
					eax += ((EDIT *)ebx)->selbarwt;
					eax += ((EDIT *)ebx)->linenrwt;
					eax -= ((EDIT *)ebx)->cpx;
					eax = GetCharFromPos(ebx, ((RAEDT *)esi)->cpy, eax, pt.y);
					temp2 = eax;
					eax++;
					eax = IsCharLeadByte(ebx, eax);
					edx = temp2;
					edx += eax;
					eax = edx;
					((EDIT *)ebx)->cpMin = eax;
					if(!fShift)
					{
						((EDIT *)ebx)->cpMax = eax;
					} // endif
					eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
					eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
					eax = temp1;
					if(eax!=((EDIT *)ebx)->cpMin)
					{
						eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
						eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
						eax = SelChange(ebx, SEL_TEXT);
						nUndoid++;
					} // endif
				}
				else
				{
					eax = GetBlockRects(ebx, &oldrects);
					eax = ((RAEDT *)esi)->rc.bottom;
					eax -= ((RAEDT *)esi)->rc.top;
					ecx = ((EDIT *)ebx)->fntinfo.fntht;
					// xor		edx,edx
					eax /= ecx;
					edx = ((EDIT *)ebx)->blrg.lnMin;
					if(edx>eax)
					{
						edx -= eax;
					}
					else
					{
						edx = 0;
					} // endif
					eax = ((EDIT *)ebx)->blrg.clMin;
					((EDIT *)ebx)->blrg.lnMin = edx;
					if(!fShift)
					{
						((EDIT *)ebx)->blrg.clMax = eax;
						((EDIT *)ebx)->blrg.lnMax = edx;
					} // endif
					eax = GetBlockCp(ebx, edx, eax);
					((EDIT *)ebx)->cpMin = eax;
					((EDIT *)ebx)->cpMax = eax;
					eax = GetCharPtr(ebx, eax);
					eax = InvalidateBlock(ebx, &oldrects);
					eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
					eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
					eax = SelChange(ebx, SEL_TEXT);
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
				eax = SendMessage(((EDIT *)ebx)->hpar, WM_COMMAND, -2, 0);
				RWORD(eax) = -3;
				eax = RWORD(eax);
				ecx = BN_CLICKED;
				ecx <<= 16;
				eax |= ecx;
				eax = SendMessage(((EDIT *)ebx)->hpar, WM_COMMAND, eax, ((EDIT *)ebx)->hexp);
				eax = SetFocus(((EDIT *)ebx)->focus);
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
				if(!(((EDIT *)ebx)->nMode&MODE_BLOCK))
				{
					eax = ((EDIT *)ebx)->cpMin;
					temp1 = eax;
					if(eax<((EDIT *)ebx)->cpMax && !fShift)
					{
						temp2 = eax;
						eax = ((EDIT *)ebx)->cpMax;
						((EDIT *)ebx)->cpMax = temp2;
						((EDIT *)ebx)->cpMin = eax;
						eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
					} // endif
					eax = GetCaretPos(&pt);
					eax = ((RAEDT *)esi)->rc.bottom;
					ecx = ((EDIT *)ebx)->fntinfo.fntht;
					// xor		edx,edx
					eax /= ecx;
					eax *= ecx;
					temp2 = eax;
					eax = ((EDIT *)ebx)->rpLineFree;
					eax /= 4;
					eax -= ((EDIT *)ebx)->nHidden;
					ecx = ((EDIT *)ebx)->fntinfo.fntht;
					eax *= ecx;
					ecx = temp2;
					eax -= ecx;
					ecx += ((RAEDT *)esi)->cpy;
					if(R_SIGNED(eax) >= R_SIGNED(ecx))
					{
						((RAEDT *)esi)->cpy = ecx;
						eax = ((RAEDT *)esi)->cpxmax;
						eax += ((EDIT *)ebx)->selbarwt;
						eax += ((EDIT *)ebx)->linenrwt;
						eax -= ((EDIT *)ebx)->cpx;
						eax = GetCharFromPos(ebx, ((RAEDT *)esi)->cpy, eax, pt.y);
						temp2 = eax;
						eax++;
						eax = IsCharLeadByte(ebx, eax);
						edx = temp2;
						edx += eax;
						eax = edx;
						((EDIT *)ebx)->cpMin = eax;
						if(!fShift)
						{
							((EDIT *)ebx)->cpMax = eax;
						} // endif
						eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
						eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
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
						eax += ((EDIT *)ebx)->selbarwt;
						eax += ((EDIT *)ebx)->linenrwt;
						eax -= ((EDIT *)ebx)->cpx;
						eax = GetCharFromPos(ebx, ((RAEDT *)esi)->cpy, eax, pt.y);
						temp2 = eax;
						eax++;
						eax = IsCharLeadByte(ebx, eax);
						edx = temp2;
						edx += eax;
						eax = edx;
						((EDIT *)ebx)->cpMin = eax;
						if(!fShift)
						{
							((EDIT *)ebx)->cpMax = eax;
						} // endif
						eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
					} // endif
					eax = temp1;
					if(eax!=((EDIT *)ebx)->cpMin)
					{
						eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
						eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
						eax = SelChange(ebx, SEL_TEXT);
						nUndoid++;
					} // endif
				}
				else
				{
					eax = GetBlockRects(ebx, &oldrects);
					eax = ((RAEDT *)esi)->rc.bottom;
					eax -= ((RAEDT *)esi)->rc.top;
					ecx = ((EDIT *)ebx)->fntinfo.fntht;
					// xor		edx,edx
					eax /= ecx;
					edx = ((EDIT *)ebx)->blrg.lnMin;
					edx += eax;
					eax = ((EDIT *)ebx)->rpLineFree;
					eax /= 4;
					if(edx>eax)
					{
						edx = eax;
					} // endif
					eax = ((EDIT *)ebx)->blrg.clMin;
					((EDIT *)ebx)->blrg.lnMin = edx;
					if(!fShift)
					{
						((EDIT *)ebx)->blrg.clMax = eax;
						((EDIT *)ebx)->blrg.lnMax = edx;
					} // endif
					eax = GetBlockCp(ebx, edx, eax);
					((EDIT *)ebx)->cpMin = eax;
					((EDIT *)ebx)->cpMax = eax;
					eax = GetCharPtr(ebx, eax);
					eax = InvalidateBlock(ebx, &oldrects);
					eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
					eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
					eax = SelChange(ebx, SEL_TEXT);
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
				if(((EDIT *)ebx)->nsplitt)
				{
					eax = ((EDIT *)ebx)->cpMin;
					((RAEDT *)esi)->cp = eax;
					eax = hWin;
					if(eax==((EDIT *)ebx)->edtb.hwnd)
					{
						eax = ((EDIT *)ebx)->edta.cp;
						((EDIT *)ebx)->cpMin = eax;
						((EDIT *)ebx)->cpMax = eax;
						eax = SetFocus(((EDIT *)ebx)->edta.hwnd);
					}
					else
					{
						eax = ((EDIT *)ebx)->edtb.cp;
						((EDIT *)ebx)->cpMin = eax;
						((EDIT *)ebx)->cpMax = eax;
						eax = SetFocus(((EDIT *)ebx)->edtb.hwnd);
					} // endif
					eax = SelChange(ebx, SEL_TEXT);
					nUndoid++;
				} // endif
			} // endif
		}
		else
		{
			if(!(((EDIT *)ebx)->nMode&MODE_BLOCK))
			{
				((RAEDT *)esi)->cpxmax = 0;
				((EDIT *)ebx)->cpx = 0;
				if(fControl)
				{
					eax = 0;
					((RAEDT *)esi)->cpy = eax;
				}
				else
				{
					eax = ((EDIT *)ebx)->cpMin;
					if(eax>((EDIT *)ebx)->cpMax && !fShift)
					{
						temp1 = eax;
						eax = ((EDIT *)ebx)->cpMax;
						((EDIT *)ebx)->cpMax = temp1;
						((EDIT *)ebx)->cpMin = eax;
						eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
						eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
					} // endif
					eax = GetCaretPos(&pt);
					eax = GetCharFromPos(ebx, ((RAEDT *)esi)->cpy, 0, pt.y);
					temp1 = eax;
					eax = SkipSpace(ebx, eax, FALSE);
					edx = temp1;
					if(eax==((EDIT *)ebx)->cpMin)
					{
						eax = edx;
					} // endif
				} // endif
				((EDIT *)ebx)->cpMin = eax;
				if(!fShift)
				{
					((EDIT *)ebx)->cpMax = eax;
				} // endif
				eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
				eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
				eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
			}
			else
			{
				eax = GetBlockRects(ebx, &oldrects);
				if(fControl)
				{
					eax = GetCharFromPos(ebx, 0, ((EDIT *)ebx)->cpx, 0);
				}
				else
				{
					eax = GetCaretPos(&pt);
					eax = GetCharFromPos(ebx, ((RAEDT *)esi)->cpy, ((EDIT *)ebx)->cpx, pt.y);
				} // endif
				eax = SetBlockFromCp(ebx, eax, fShift);
				eax = InvalidateBlock(ebx, &oldrects);
				eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
				eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
			} // endif
			eax = SelChange(ebx, SEL_TEXT);
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
				eax = 1ffH;
				if(((EDIT *)ebx)->fsplitt)
				{
					eax = 0;
				} // endif
				eax = SendMessage(((EDIT *)ebx)->hwnd, REM_SETSPLIT, eax, 0);
			} // endif
		}
		else
		{
			if(!(((EDIT *)ebx)->nMode&MODE_BLOCK))
			{
				eax = ((EDIT *)ebx)->cpMin;
				if(eax<((EDIT *)ebx)->cpMax && !fShift)
				{
					temp1 = eax;
					eax = ((EDIT *)ebx)->cpMax;
					((EDIT *)ebx)->cpMax = temp1;
					eax--;
					((EDIT *)ebx)->cpMin = eax;
					eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
				}
				else if(eax>((EDIT *)ebx)->cpMax && !fShift)
				{
					((EDIT *)ebx)->cpMin--;
					eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
				} // endif
				((RAEDT *)esi)->cpxmax = 999999999;
				if(fControl)
				{
					eax = GetCharFromPos(ebx, ((RAEDT *)esi)->cpy, 999999999, 999999999);
				}
				else
				{
					eax = GetCaretPos(&pt);
					eax = GetCharFromPos(ebx, ((RAEDT *)esi)->cpy, 999999999, pt.y);
					temp1 = eax;
					eax = SkipSpace(ebx, eax, TRUE);
					edx = temp1;
					if(eax==((EDIT *)ebx)->cpMin)
					{
						eax = edx;
					} // endif
				} // endif
				((EDIT *)ebx)->cpMin = eax;
				if(!fShift)
				{
					((EDIT *)ebx)->cpMax = eax;
				} // endif
				eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
				eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
				eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
				eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
				eax = SetCpxMax(ebx, hWin);
			}
			else
			{
				eax = GetBlockRects(ebx, &oldrects);
				if(fControl)
				{
					eax = GetCharFromPos(ebx, ((RAEDT *)esi)->cpy, 999999999, 999999999);
				}
				else
				{
					eax = GetCaretPos(&pt);
					eax = GetCharFromPos(ebx, ((RAEDT *)esi)->cpy, 999999999, pt.y);
				} // endif
				eax = SetBlockFromCp(ebx, eax, fShift);
				eax = InvalidateBlock(ebx, &oldrects);
				eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
				eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
			} // endif
			eax = SelChange(ebx, SEL_TEXT);
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
				eax = IsDlgButtonChecked(((EDIT *)ebx)->hwnd, -2);
				if(eax)
				{
					eax = BST_UNCHECKED;
				}
				else
				{
					eax = BST_CHECKED;
				} // endif
				eax = CheckDlgButton(((EDIT *)ebx)->hwnd, -2, eax);
				RWORD(eax) = -2;
				eax = RWORD(eax);
				ecx = BN_CLICKED;
				ecx <<= 16;
				eax |= ecx;
				eax = SendMessage(((EDIT *)ebx)->hwnd, WM_COMMAND, eax, ((EDIT *)ebx)->hlin);
				eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
				eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
			} // endif
		}
		else
		{
			if(!fShift && !fControl)
			{
				// Insert
				((EDIT *)ebx)->fOvr ^= 1;
				if(((EDIT *)ebx)->fOvr)
				{
					((EDIT *)ebx)->nMode |= MODE_OVERWRITE;
				}
				else
				{
					((EDIT *)ebx)->nMode &= -1 ^ MODE_OVERWRITE;
				} // endif
				eax = SelChange(ebx, SEL_TEXT);
			}
			else if(fShift && !fControl)
			{
				// Shift+Insert, Paste
				nUndoid++;
				eax = IsSelectionLocked(ebx, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
				if(eax!=0)
				{
					goto ErrBeep;
				} // endif
				eax = Paste(ebx, hWin, NULL);
				nUndoid++;
			}
			else if(!fShift && fControl)
			{
				// Ctrl+Insert, Copy
				eax = Copy(ebx);
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
				if(!(((EDIT *)ebx)->nMode&MODE_BLOCK))
				{
					eax = ((EDIT *)ebx)->cpMin;
					if(fControl)
					{
						if(eax>((EDIT *)ebx)->cpMax)
						{
							temp1 = ((EDIT *)ebx)->cpMax;
							((EDIT *)ebx)->cpMax = eax;
							eax = temp1;
							((EDIT *)ebx)->cpMin = eax;
						} // endif
						if(fShift)
						{
							eax = GetLineEnd(ebx, ((EDIT *)ebx)->cpMax);
						}
						else
						{
							eax = GetWordEnd(ebx, ((EDIT *)ebx)->cpMax, 0);
						} // endif
						((EDIT *)ebx)->cpMax = eax;
					} // endif
					eax = ((EDIT *)ebx)->cpMin;
					if(eax!=((EDIT *)ebx)->cpMax)
					{
						// Selection
						eax = IsSelectionLocked(ebx, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
						if(eax!=0)
						{
							goto ErrBeep;
						} // endif
						nUndoid++;
						eax = DeleteSelection(ebx, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
						((EDIT *)ebx)->cpMin = eax;
						((EDIT *)ebx)->cpMax = eax;
						if(eax<((EDIT *)ebx)->edta.cp)
						{
							((EDIT *)ebx)->edta.cp = eax;
						} // endif
						if(eax<((EDIT *)ebx)->edtb.cp)
						{
							((EDIT *)ebx)->edtb.cp = eax;
						} // endif
						eax = GetCharPtr(ebx, ((EDIT *)ebx)->cpMin);
						eax = GetYpFromLine(ebx, edx);
						if(eax<((EDIT *)ebx)->edta.cpy)
						{
							((EDIT *)ebx)->edta.cpy = eax;
						} // endif
						if(eax<((EDIT *)ebx)->edtb.cpy)
						{
							((EDIT *)ebx)->edtb.cpy = eax;
						} // endif
						eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
						eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
						eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
						eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
						nUndoid++;
					}
					else
					{
						// Single char
						eax++;
						eax = IsSelectionLocked(ebx, ((EDIT *)ebx)->cpMin, eax);
						if(eax!=0)
						{
							goto ErrBeep;
						} // endif
						eax = GetChar(ebx, ((EDIT *)ebx)->cpMin);
						if(RBYTE_LOW(eax)==VK_RETURN)
						{
							eax = GetBookMark(ebx, ((EDIT *)ebx)->line);
							if(eax==2 || eax==8)
							{
								eax = Expand(ebx, ((EDIT *)ebx)->line);
							} // endif
						}
						else
						{
							eax = IsCharLeadByte(ebx, ((EDIT *)ebx)->cpMin);
							if(eax)
							{
								eax = DeleteChar(ebx, ((EDIT *)ebx)->cpMin);
								eax = SaveUndo(ebx, UNDO_DELETE, ((EDIT *)ebx)->cpMin, eax, 1);
							} // endif
						} // endif
						eax = DeleteChar(ebx, ((EDIT *)ebx)->cpMin);
						temp1 = eax;
						eax = SaveUndo(ebx, UNDO_DELETE, ((EDIT *)ebx)->cpMin, eax, 1);
						eax = temp1;
						if(eax==VK_RETURN)
						{
							eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
							eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
						}
						else
						{
							eax = InvalidateLine(ebx, hWin, ((EDIT *)ebx)->line);
						} // endif
					} // endif
				}
				else
				{
					// Block
					eax = IsSelectionLocked(ebx, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
					if(eax!=0)
					{
						goto ErrBeep;
					} // endif
					nUndoid++;
					eax = ((EDIT *)ebx)->blrg.clMin;
					if(eax==((EDIT *)ebx)->blrg.clMax)
					{
						((EDIT *)ebx)->blrg.clMax++;
					} // endif
					eax = DeleteSelectionBlock(ebx, ((EDIT *)ebx)->blrg.lnMin, ((EDIT *)ebx)->blrg.clMin, ((EDIT *)ebx)->blrg.lnMax, ((EDIT *)ebx)->blrg.clMax);
					eax = ((EDIT *)ebx)->blrg.clMin;
					if(eax>((EDIT *)ebx)->blrg.clMax)
					{
						eax = ((EDIT *)ebx)->blrg.clMax;
					} // endif
					edx = ((EDIT *)ebx)->blrg.lnMin;
					if(edx>((EDIT *)ebx)->blrg.lnMax)
					{
						edx = ((EDIT *)ebx)->blrg.lnMax;
					} // endif
					((EDIT *)ebx)->blrg.clMin = eax;
					((EDIT *)ebx)->blrg.lnMin = edx;
					((EDIT *)ebx)->blrg.clMax = eax;
					((EDIT *)ebx)->blrg.lnMax = edx;
					eax = GetBlockCp(ebx, edx, eax);
					((EDIT *)ebx)->cpMin = eax;
					((EDIT *)ebx)->cpMax = eax;
					eax = GetCharPtr(ebx, ((EDIT *)ebx)->cpMin);
					eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
					eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
					eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
					eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
					nUndoid++;
				} // endif
				eax = SetCpxMax(ebx, hWin);
				eax = SelChange(ebx, SEL_TEXT);
			}
			else if(fShift && !fControl)
			{
				// Shift+Delete, Cut
				eax = IsSelectionLocked(ebx, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
				if(eax!=0)
				{
					goto ErrBeep;
				} // endif
				nUndoid++;
				eax = Cut(ebx, hWin);
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
			if(!(((EDIT *)ebx)->nMode&MODE_BLOCK))
			{
				eax = ((EDIT *)ebx)->cpMin;
				if(eax>((EDIT *)ebx)->cpMax)
				{
					temp1 = ((EDIT *)ebx)->cpMax;
					((EDIT *)ebx)->cpMax = eax;
					eax = temp1;
					((EDIT *)ebx)->cpMin = eax;
				} // endif
				if(fShift)
				{
					eax = GetLineStart(ebx, ((EDIT *)ebx)->cpMin);
				}
				else
				{
					eax = GetWordStart(ebx, ((EDIT *)ebx)->cpMin, 0);
				} // endif
				if(eax && eax==((EDIT *)ebx)->cpMin)
				{
					eax--;
				} // endif
				((EDIT *)ebx)->cpMin = eax;
				if(eax!=((EDIT *)ebx)->cpMax)
				{
					eax = IsSelectionLocked(ebx, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
					if(eax!=0)
					{
						goto ErrBeep;
					} // endif
					nUndoid++;
					eax = DeleteSelection(ebx, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
					((EDIT *)ebx)->cpMin = eax;
					((EDIT *)ebx)->cpMax = eax;
					eax = GetCharPtr(ebx, ((EDIT *)ebx)->cpMin);
					eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
					eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
					eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
					eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
					eax = SetCpxMax(ebx, hWin);
					eax = SelChange(ebx, SEL_TEXT);
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
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
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
	DWORD hCur;
	RECT rect;
	RECT oldrects[2];

	auto void SetBlock(void);
	auto void SetScroll(void);

	// Get memory pointers
	eax = GetWindowLong(hWin, 0);
	ebx = eax;
	if(eax)
	{
		eax = hWin;
		if(eax==((EDIT *)ebx)->edta.hwnd)
		{
			esi = &((EDIT *)ebx)->edta;
		}
		else
		{
			esi = &((EDIT *)ebx)->edtb;
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
				if(!(((EDIT *)ebx)->fstyle&STYLE_NOBACKBUFFER))
				{
					eax = RAEditPaint(hWin);
				}
				else
				{
					eax = GetUpdateRect(hWin, &rect, FALSE);
					eax = rect.bottom;
					eax -= rect.top;
					edx = ((EDIT *)ebx)->fntinfo.fntht;
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
		eax = ((EDIT *)ebx)->cpMin;
		cpOldMin = eax;
		edx = ((EDIT *)ebx)->cpMax;
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
			eax = Copy(ebx);
		}
		else if(edx==0x58 && fControl && !fShift && !fAlt)
		{
			// Ctrl+X, Cut
			eax = IsSelectionLocked(ebx, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
			if(eax!=0)
			{
				goto ErrBeep;
			} // endif
			nUndoid++;
			eax = Cut(ebx, hWin);
			nUndoid++;
		}
		else if(edx==0x56 && fControl && !fShift && !fAlt)
		{
			// Ctrl+V, Paste
			eax = IsSelectionLocked(ebx, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
			if(eax!=0)
			{
				goto ErrBeep;
			} // endif
			nUndoid++;
			eax = Paste(ebx, hWin, NULL);
			nUndoid++;
		}
		else if(edx==0x41 && fControl && !fShift && !fAlt)
		{
			// Ctrl+A, Select all
			if(!(((EDIT *)ebx)->nMode&MODE_BLOCK))
			{
				eax = GetCharPtr(ebx, -1);
				((EDIT *)ebx)->cpMax = ecx;
				eax = GetCharPtr(ebx, 0);
				((EDIT *)ebx)->cpMin = ecx;
				eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
				eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
				eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
				eax = SelChange(ebx, SEL_TEXT);
				nUndoid++;
			} // endif
		}
		else if(edx==0x5A && fControl && !fShift && !fAlt)
		{
			// Ctrl+Z, Undo
			nUndoid++;
			eax = Undo(ebx, hWin);
			nUndoid++;
		}
		else if(edx==0x59 && fControl && !fShift && !fAlt)
		{
			// Ctrl+Y, Redo
			nUndoid++;
			eax = Redo(ebx, hWin);
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
		((EDIT *)ebx)->cpselbar = -1;
		if(((EDIT *)ebx)->cpbrst!=-1 || ((EDIT *)ebx)->cpbren!=-1)
		{
			((EDIT *)ebx)->cpbrst = -1;
			((EDIT *)ebx)->cpbren = -1;
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
		} // endif
		if(!(((EDIT *)ebx)->nMode&MODE_BLOCK))
		{
			eax = wParam;
			if((eax>=VK_SPACE || eax==VK_RETURN || eax==VK_TAB) && eax!=0x7F)
			{
				eax = IsSelectionLocked(ebx, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
				if(eax!=0)
				{
					goto ErrBeep;
				} // endif
				eax = ((EDIT *)ebx)->cpMin;
				eax -= ((EDIT *)ebx)->cpMax;
				temp1 = eax;
				eax = DeleteSelection(ebx, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
				((EDIT *)ebx)->cpMin = eax;
				((EDIT *)ebx)->cpMax = eax;
				ecx = 1;
				if(wParam==VK_TAB && ((EDIT *)ebx)->fExpandTab)
				{
					wParam = VK_SPACE;
					eax = GetTabPos(ebx, ((EDIT *)ebx)->cpMin);
					ecx = ((EDIT *)ebx)->nTab;
					ecx -= eax;
				} // endif
anon_5:
				temp2 = ecx;
				temp3 = ((EDIT *)ebx)->cpMin;
				eax = GetCharPtr(ebx, ((EDIT *)ebx)->cpMin);
				eax = InsertChar(ebx, ((EDIT *)ebx)->cpMin, wParam);
				ecx = temp3;
				if(!eax)
				{
					eax = SaveUndo(ebx, UNDO_INSERT, ecx, wParam, 1);
				}
				else
				{
					eax = SaveUndo(ebx, UNDO_OVERWRITE, ecx, eax, 1);
				} // endif
				eax = ((EDIT *)ebx)->cpMin;
				eax++;
				((EDIT *)ebx)->cpMin = eax;
				((EDIT *)ebx)->cpMax = eax;
				ecx = temp2;
				ecx--;
				if(ecx!=0)
				{
					goto anon_5;
				} // endif
				if(wParam==VK_RETURN && ((EDIT *)ebx)->fIndent)
				{
					eax = AutoIndent(ebx);
				} // endif
				eax = GetCharPtr(ebx, ((EDIT *)ebx)->cpMin);
				eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
				temp2 = eax;
				if(!eax)
				{
					eax = InvalidateLine(ebx, ((EDIT *)ebx)->edta.hwnd, ((EDIT *)ebx)->line);
					eax = InvalidateLine(ebx, ((EDIT *)ebx)->edtb.hwnd, ((EDIT *)ebx)->line);
				} // endif
				eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
				eax = temp2;
				edx = temp1;
				if(eax && !edx)
				{
					if(wParam==VK_RETURN)
					{
						eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
						eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
					}
					else
					{
						eax = InvalidateLine(ebx, ((EDIT *)ebx)->edta.hwnd, ((EDIT *)ebx)->line);
						eax = InvalidateLine(ebx, ((EDIT *)ebx)->edtb.hwnd, ((EDIT *)ebx)->line);
					} // endif
				}
				else if((eax && edx) || wParam==VK_RETURN)
				{
					eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
					eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
				} // endif
				eax = SetCpxMax(ebx, hWin);
				eax = SelChange(ebx, SEL_TEXT);
				eax = ((EDIT *)ebx)->cpMin;
				eax--;
				eax = BracketMatch(ebx, wParam, eax);
			}
			else if(eax==0x08)
			{
				eax = ((EDIT *)ebx)->cpMin;
				if(eax!=((EDIT *)ebx)->cpMax || eax)
				{
					eax = IsSelectionLocked(ebx, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
					if(eax!=0)
					{
						goto ErrBeep;
					} // endif
					eax = ((EDIT *)ebx)->cpMin;
					if(eax!=((EDIT *)ebx)->cpMax)
					{
						eax = DeleteSelection(ebx, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
						((EDIT *)ebx)->cpMin = eax;
						((EDIT *)ebx)->cpMax = eax;
						eax = GetCharPtr(ebx, ((EDIT *)ebx)->cpMin);
						eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
						eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
						eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
						eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
					}
					else if(eax)
					{
						eax--;
						eax = IsSelectionLocked(ebx, eax, eax);
						if(eax!=0)
						{
							goto ErrBeep;
						} // endif
						((EDIT *)ebx)->cpMin--;
						((EDIT *)ebx)->cpMax--;
						eax = GetChar(ebx, ((EDIT *)ebx)->cpMin);
						if(RBYTE_LOW(eax)==VK_RETURN)
						{
							eax = IsLineHidden(ebx, ((EDIT *)ebx)->line);
							if(eax)
							{
								eax = PreviousBookMark(ebx, ((EDIT *)ebx)->line, 2);
								if(eax!=-1)
								{
									eax = Expand(ebx, eax);
								} // endif
							} // endif
							eax = DeleteChar(ebx, ((EDIT *)ebx)->cpMin);
							wParam = eax;
							eax = SaveUndo(ebx, UNDO_BACKDELETE, ((EDIT *)ebx)->cpMin, eax, 1);
						}
						else
						{
							eax = ((EDIT *)ebx)->cpMin;
							if(eax)
							{
								eax--;
								eax = IsCharLeadByte(ebx, eax);
								if(eax)
								{
									eax = DeleteChar(ebx, ((EDIT *)ebx)->cpMin);
									wParam = eax;
									eax = SaveUndo(ebx, UNDO_BACKDELETE, ((EDIT *)ebx)->cpMin, eax, 1);
									((EDIT *)ebx)->cpMin--;
									((EDIT *)ebx)->cpMax--;
								} // endif
							} // endif
							eax = DeleteChar(ebx, ((EDIT *)ebx)->cpMin);
							wParam = eax;
							eax = SaveUndo(ebx, UNDO_BACKDELETE, ((EDIT *)ebx)->cpMin, eax, 1);
						} // endif
						eax = GetCharPtr(ebx, ((EDIT *)ebx)->cpMin);
						eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
						if(wParam==VK_RETURN)
						{
							if(R_SIGNED(eax) < 0)
							{
								((RAEDT *)esi)->cpy += eax;
								eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
							} // endif
							eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
							eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
							eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
						}
						else
						{
							eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
							eax = InvalidateLine(ebx, ((EDIT *)ebx)->edta.hwnd, ((EDIT *)ebx)->line);
							eax = InvalidateLine(ebx, ((EDIT *)ebx)->edtb.hwnd, ((EDIT *)ebx)->line);
						} // endif
					} // endif
					eax = SetCpxMax(ebx, hWin);
					eax = SelChange(ebx, SEL_TEXT);
				} // endif
			} // endif
		}
		else
		{
			eax = wParam;
			if((eax>=VK_SPACE || eax==VK_TAB) && eax!=0x7F)
			{
				eax = IsSelectionLocked(ebx, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
				if(eax!=0)
				{
					goto ErrBeep;
				} // endif
				temp1 = esi;
				nUndoid++;
				eax = ((EDIT *)ebx)->blrg.lnMin;
				edx = ((EDIT *)ebx)->blrg.lnMax;
				if(eax<edx)
				{
					temp2 = eax;
					eax = edx;
					edx = temp2;
				} // endif
				eax -= edx;
				eax++;
				edi = eax;
				eax = ((EDIT *)ebx)->blrg.clMin;
				edx = ((EDIT *)ebx)->blrg.clMax;
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
						eax = ((EDIT *)ebx)->blrg.clMin;
						ecx = ((EDIT *)ebx)->nTab;
						// xor		edx,edx
						eax /= ecx;
						eax++;
						eax *= ecx;
						eax -= ((EDIT *)ebx)->blrg.clMin;
						cp = eax;
						if(((EDIT *)ebx)->fExpandTab)
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
				eax = Paste(ebx, hWin, eax);
				eax = ((EDIT *)ebx)->blrg.clMin;
				if(eax==((EDIT *)ebx)->blrg.clMax)
				{
					eax += cp;
					((EDIT *)ebx)->blrg.clMin = eax;
					((EDIT *)ebx)->blrg.clMax = eax;
					eax = GetBlockCp(ebx, ((EDIT *)ebx)->blrg.lnMin, eax);
					((EDIT *)ebx)->cpMin = eax;
					((EDIT *)ebx)->cpMax = eax;
				}
				else
				{
					nUndoid++;
				} // endif
				eax = temp2;
				eax = GlobalFree(eax);
				esi = temp1;
				eax = GetCharPtr(ebx, ((EDIT *)ebx)->cpMin);
				eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
				eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
				eax = SelChange(ebx, SEL_TEXT);
			}
			else if(eax==0x08)
			{
				eax = IsSelectionLocked(ebx, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
				if(eax!=0)
				{
					goto ErrBeep;
				} // endif
				eax = ((EDIT *)ebx)->blrg.clMin;
				if(eax==((EDIT *)ebx)->blrg.clMax && eax)
				{
					((EDIT *)ebx)->blrg.clMin--;
				} // endif
				eax = DeleteSelectionBlock(ebx, ((EDIT *)ebx)->blrg.lnMin, ((EDIT *)ebx)->blrg.clMin, ((EDIT *)ebx)->blrg.lnMax, ((EDIT *)ebx)->blrg.clMax);
				eax = ((EDIT *)ebx)->blrg.clMin;
				if(eax>((EDIT *)ebx)->blrg.clMax)
				{
					eax = ((EDIT *)ebx)->blrg.clMax;
				} // endif
				((EDIT *)ebx)->blrg.clMin = eax;
				((EDIT *)ebx)->blrg.clMax = eax;
				eax = GetBlockCp(ebx, ((EDIT *)ebx)->blrg.lnMin, eax);
				((EDIT *)ebx)->cpMin = eax;
				((EDIT *)ebx)->cpMax = eax;
				eax = GetCharPtr(ebx, ((EDIT *)ebx)->cpMin);
				eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
				eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
				eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
				eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
				eax = SelChange(ebx, SEL_TEXT);
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
		eax = SendMessage(((EDIT *)ebx)->htt, TTM_ACTIVATE, FALSE, 0);
		eax = SendMessage(((EDIT *)ebx)->htt, TTM_ACTIVATE, TRUE, 0);
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
					ecx = ((EDIT *)ebx)->selbarwt;
					ecx += ((EDIT *)ebx)->linenrwt;
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
							eax = ((EDIT *)ebx)->hwnd;
							tmr1.hwnd = eax;
							tmr1.umsg = WM_HSCROLL;
							tmr1.wparam = SB_LINEDOWN;
							eax = ((EDIT *)ebx)->hhscroll;
							tmr1.lparam = eax;
						}
						else if(R_SIGNED(eax) < R_SIGNED(ecx))
						{
							edx = ecx;
							edx -= eax;
							eax = ((EDIT *)ebx)->hwnd;
							tmr1.hwnd = eax;
							tmr1.umsg = WM_HSCROLL;
							tmr1.wparam = SB_LINEUP;
							eax = ((EDIT *)ebx)->hhscroll;
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
						eax = SetTimer(NULL, 0, eax, &TimerProc);
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
				ecx = ((EDIT *)ebx)->fntinfo.fntht;
				if(R_SIGNED(eax) < 0)
				{
					eax = -eax;
					// xor		edx,edx
					eax /= ecx;
					eax *= ecx;
					eax = -eax;
				}
				else
				{
					// xor		edx,edx
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
				eax = SetCpxMax(ebx, hWin);
				eax = temp1;
				eax = GetCharFromPos(ebx, ((RAEDT *)esi)->cpy, eax, edi);
				temp1 = eax;
				if(eax)
				{
					eax--;
					eax = IsCharLeadByte(ebx, eax);
				} // endif
				edx = temp1;
				edx += eax;
				eax = edx;
				if(eax!=((EDIT *)ebx)->cpMin)
				{
					temp1 = ((EDIT *)ebx)->cpMin;
					cpOldMin = temp1;
					((EDIT *)ebx)->cpMin = eax;
					if(!((EDIT *)ebx)->fCaretHide)
					{
						eax = HideCaret(hWin);
						((EDIT *)ebx)->fCaretHide = TRUE;
					} // endif
					eax = InvalidateSelection(ebx, ((EDIT *)ebx)->edta.hwnd, cpOldMin, ((EDIT *)ebx)->cpMin);
					eax = InvalidateSelection(ebx, ((EDIT *)ebx)->edtb.hwnd, cpOldMin, ((EDIT *)ebx)->cpMin);
					eax = SelChange(ebx, SEL_TEXT);
				} // endif
			}
			else
			{
				// Block Selection, not on selbar
				fShift = TRUE;
				eax = 0;
				SetBlock();
				eax = SelChange(ebx, SEL_TEXT);
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
				hDragSourceMem = ebx;
				eax = hWin;
				hDragWin = eax;
				eax = ((EDIT *)ebx)->cpMin;
				edx = ((EDIT *)ebx)->cpMax;
				if(eax>edx)
				{
					temp1 = eax;
					eax = edx;
					edx = temp1;
				} // endif
				cpDragSource.cpMin = eax;
				cpDragSource.cpMax = edx;
				eax = DoDragDrop(&pIDataObject, &pIDropSource, DROPEFFECT_COPY | DROPEFFECT_MOVE, &peff);
				eax = peff;
				if(eax==DROPEFFECT_MOVE && !(((EDIT *)ebx)->fstyle & STYLE_READONLY))
				{
					eax = IsSelectionLocked(ebx, cpDragSource.cpMin, cpDragSource.cpMax);
					if(!eax)
					{
						eax = ((EDIT *)ebx)->cpMin;
						if(eax>((EDIT *)ebx)->cpMax)
						{
							eax = ((EDIT *)ebx)->cpMax;
						} // endif
						temp1 = eax;
						eax = DeleteSelection(ebx, cpDragSource.cpMin, cpDragSource.cpMax);
						eax = temp1;
						if(eax>cpDragSource.cpMin)
						{
							eax -= cpDragSource.cpMax;
							eax += cpDragSource.cpMin;
						} // endif
						((EDIT *)ebx)->cpMin = eax;
						((EDIT *)ebx)->cpMax = eax;
						eax = GetCharPtr(ebx, ((EDIT *)ebx)->cpMin);
						eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
						eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
						eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
						eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
						eax = SetCpxMax(ebx, hWin);
						eax = SelChange(ebx, SEL_TEXT);
					} // endif
				} // endif
				eax = GetFocus();
				if(eax==((EDIT *)ebx)->edta.hwnd || eax==((EDIT *)ebx)->edtb.hwnd)
				{
					eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
					eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
				} // endif
				hDragSourceMem = 0;
			} // endif
		}
		else
		{
			fOnBM = FALSE;
			fOnSel = 0;
			ecx = ((EDIT *)ebx)->selbarwt;
			ecx += ((EDIT *)ebx)->linenrwt;
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
					temp1 = ((EDIT *)ebx)->cpMin;
					cpOldMin = temp1;
					temp1 = ((EDIT *)ebx)->cpMax;
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
							eax = GetCharFromPos(ebx, 0, 0, iYp);
							((EDIT *)ebx)->cpMin = eax;
							eax = GetCharFromPos(ebx, 0, 0, edi);
							((EDIT *)ebx)->cpMax = eax;
							edi += ((EDIT *)ebx)->fntinfo.fntht;
							eax = GetCharFromPos(ebx, 0, 0, edi);
							if(eax==((EDIT *)ebx)->cpMax)
							{
								eax = GetCharFromPos(ebx, 0, 9999, edi);
							} // endif
							((EDIT *)ebx)->cpMax = eax;
						}
						else
						{
							temp1 = eax;
							eax = iYp;
							eax += ((EDIT *)ebx)->fntinfo.fntht;
							eax = GetCharFromPos(ebx, 0, 0, eax);
							((EDIT *)ebx)->cpMax = eax;
							eax = temp1;
							eax = GetCharFromPos(ebx, 0, 0, eax);
							((EDIT *)ebx)->cpMin = eax;
						} // endif
					} // endif
					eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
					eax = ((EDIT *)ebx)->cpMin;
					if(eax!=cpOldMin)
					{
						eax = InvalidateSelection(ebx, ((EDIT *)ebx)->edta.hwnd, cpOldMin, ((EDIT *)ebx)->cpMin);
						eax = InvalidateSelection(ebx, ((EDIT *)ebx)->edtb.hwnd, cpOldMin, ((EDIT *)ebx)->cpMin);
					} // endif
					eax = ((EDIT *)ebx)->cpMax;
					if(eax!=cpOldMax)
					{
						eax = InvalidateSelection(ebx, ((EDIT *)ebx)->edta.hwnd, cpOldMax, ((EDIT *)ebx)->cpMax);
						eax = InvalidateSelection(ebx, ((EDIT *)ebx)->edtb.hwnd, cpOldMax, ((EDIT *)ebx)->cpMax);
					} // endif
					eax = SelChange(ebx, SEL_TEXT);
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
								eax = SetTimer(NULL, 0, eax, &TimerProc);
								TimerID = eax;
							} // endif
						} // endif
					} // endif
				}
				else
				{
					edx = ((EDIT *)ebx)->cpx;
					edx = -edx;
					eax = GetCharFromPos(ebx, ((RAEDT *)esi)->cpy, edx, eax);
					cp = eax;
					eax = GetCharPtr(ebx, eax);
					edi = ((EDIT *)ebx)->rpChars;
					edi += ((EDIT *)ebx)->hChars;
					ecx = ((CHARS *)edi)->len;
					if(((CHARS *)edi)->state&STATE_BMMASK)
					{
						eax = GetPosFromChar(ebx, cp, &pt);
						edx = pt.y;
						edx -= ((RAEDT *)esi)->cpy;
						eax = lParam;
						eax >>= 16;
						R_SIGNED(eax) = RWORD_SIGNED(eax);
						eax -= edx;
						edx = ((EDIT *)ebx)->fntinfo.fntht;
						edx -= 7;
						edx /= 1;
						eax -= edx;
						if(eax<12)
						{
							eax = lParam;
							eax &= 0x0FFFF;
							R_SIGNED(eax) = RWORD_SIGNED(eax);
							eax -= ((EDIT *)ebx)->linenrwt;
							eax -= ((EDIT *)ebx)->selbarwt;
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
				eax = ((EDIT *)ebx)->cpMin;
				if(eax!=((EDIT *)ebx)->cpMax)
				{
					eax = GetCharFromPos(ebx, ((RAEDT *)esi)->cpy, pt.x, pt.y);
					cp = eax;
					ecx = ((EDIT *)ebx)->cpMin;
					edx = ((EDIT *)ebx)->cpMax;
					if(ecx>edx)
					{
						temp1 = ecx;
						ecx = edx;
						edx = temp1;
					} // endif
					if(eax>=ecx && eax<edx)
					{
						eax = GetChar(ebx, cp);
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
			eax = ((EDIT *)ebx)->cpMax;
			eax -= ((EDIT *)ebx)->cpMin;
			fSel = eax;
			// Get mouse x position
			eax = lParam;
			edx = eax;
			edx >>= 16;
			ptDrag.y = edx;
			R_SIGNED(eax) = RWORD_SIGNED(eax);
			ptDrag.x = eax;
			ecx = ((EDIT *)ebx)->selbarwt;
			ecx += ((EDIT *)ebx)->linenrwt;
			if(eax<=ecx)
			{
				// On selection bar
				eax = GetTopFromYp(ebx, hWin, ((RAEDT *)esi)->cpy);
				eax = SetCapture(hWin);
				if(!fOnBM)
				{
					if(!(((EDIT *)ebx)->nMode&MODE_BLOCK))
					{
						if(((EDIT *)ebx)->cpx)
						{
							((EDIT *)ebx)->cpx = 0;
							eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
							eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
						} // endif
						edx = lParam;
						edx >>= 16;
						edx += ((RAEDT *)esi)->cpy;
						iYp = edx;
						fSelState = 3;
						if(fShift)
						{
							if(((EDIT *)ebx)->cpselbar==-1)
							{
								eax = ((EDIT *)ebx)->cpMin;
								((EDIT *)ebx)->cpselbar = eax;
							} // endif
							eax = GetCharFromPos(ebx, 0, 0, iYp);
							((EDIT *)ebx)->cpMin = eax;
							eax = GetCharFromPos(ebx, 0, 9999, iYp);
							((EDIT *)ebx)->cpMax = eax;
							eax = iYp;
							eax += ((EDIT *)ebx)->fntinfo.fntht;
							eax = GetCharFromPos(ebx, 0, 0, eax);
							if(eax>((EDIT *)ebx)->cpMax)
							{
								((EDIT *)ebx)->cpMax = eax;
							} // endif
							eax = ((EDIT *)ebx)->cpselbar;
							eax = SendMessage(((EDIT *)ebx)->hwnd, EM_EXLINEFROMCHAR, 0, ((EDIT *)ebx)->cpselbar);
							nOldLine = eax;
							eax = SendMessage(((EDIT *)ebx)->hwnd, EM_LINEINDEX, eax, 0);
							if(eax<((EDIT *)ebx)->cpMax)
							{
								((EDIT *)ebx)->cpMin = eax;
							}
							else
							{
								eax = ((EDIT *)ebx)->cpMin;
								((EDIT *)ebx)->cpMax = eax;
								nOldLine++;
								eax = SendMessage(((EDIT *)ebx)->hwnd, EM_LINEINDEX, nOldLine, 0);
								((EDIT *)ebx)->cpMin = eax;
							} // endif
							eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
							eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
						}
						else
						{
							eax = SendMessage(hWin, WM_MOUSEMOVE, wParam, lParam);
							((EDIT *)ebx)->cpselbar = -1;
						} // endif
					} // endif
				}
				else
				{
					// On bookmark
					eax = SetFocus(hWin);
					eax = ((EDIT *)ebx)->cpx;
					eax = -eax;
					edx = lParam;
					edx >>= 16;
					eax = GetCharFromPos(ebx, ((RAEDT *)esi)->cpy, eax, edx);
					if(fSel)
					{
						temp1 = eax;
						eax = InvalidateSelection(ebx, ((EDIT *)ebx)->edta.hwnd, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
						eax = InvalidateSelection(ebx, ((EDIT *)ebx)->edtb.hwnd, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
						eax = temp1;
					} // endif
					((EDIT *)ebx)->cpMin = eax;
					((EDIT *)ebx)->cpMax = eax;
					eax = GetCharPtr(ebx, eax);
					eax = LoadCursor(0, IDC_ARROW);
					eax = SetCursor(eax);
					eax = SetCaretVisible(((RAEDT *)esi)->hwnd, ((RAEDT *)esi)->cpy);
					eax = SelChange(ebx, SEL_OBJECT);
					eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
					eax = InvalidateSelection(ebx, ((EDIT *)ebx)->edta.hwnd, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
					eax = InvalidateSelection(ebx, ((EDIT *)ebx)->edtb.hwnd, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
				} // endif
			}
			else
			{
				((EDIT *)ebx)->cpselbar = -1;
				if(!(((EDIT *)ebx)->nMode&MODE_BLOCK))
				{
					if(fSel)
					{
						eax = InvalidateSelection(ebx, ((EDIT *)ebx)->edta.hwnd, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
						eax = InvalidateSelection(ebx, ((EDIT *)ebx)->edtb.hwnd, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
					} // endif
					eax = lParam;
					edx = eax;
					R_SIGNED(eax) = RWORD_SIGNED(eax);
					edx >>= 16;
					eax += ((EDIT *)ebx)->cpx;
					((RAEDT *)esi)->cpxmax = eax;
					eax -= ((EDIT *)ebx)->cpx;
					eax = GetCharFromPos(ebx, ((RAEDT *)esi)->cpy, eax, edx);
					temp1 = eax;
					eax--;
					eax = IsCharLeadByte(ebx, eax);
					edx = temp1;
					edx += eax;
					eax = edx;
					((EDIT *)ebx)->cpMin = eax;
					if(!fShift)
					{
						((EDIT *)ebx)->cpMax = eax;
					} // endif
					eax = SetFocus(hWin);
					eax = SendMessage(hWin, WM_SETFOCUS, 0, 0);
					eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
					if(fShift)
					{
						eax = InvalidateSelection(ebx, ((EDIT *)ebx)->edta.hwnd, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
						eax = InvalidateSelection(ebx, ((EDIT *)ebx)->edtb.hwnd, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
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
				eax = SetCpxMax(ebx, hWin);
				eax = SelChange(ebx, SEL_TEXT);
			} // endif
		}
		else if(((EDIT *)ebx)->fstyle & STYLE_DRAGDROP)
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
		eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
		nUndoid++;
		eax = 0;
		fSelState = eax;
		return eax;
	}
	else if(eax==WM_LBUTTONDBLCLK)
	{
		eax = ((EDIT *)ebx)->fstyle;
		eax &= STYLE_NODBLCLICK;
		edx = ((EDIT *)ebx)->nMode;
		edx &= MODE_BLOCK;
		if(fOnBM)
		{
			eax = LoadCursor(0, IDC_ARROW);
			eax = SetCursor(eax);
			eax = SetCaretVisible(((RAEDT *)esi)->hwnd, ((RAEDT *)esi)->cpy);
			eax = SelChange(ebx, SEL_OBJECT);
		}
		else if(!eax && !edx)
		{
			eax = ((EDIT *)ebx)->cpMin;
			cpOldMin = eax;
			edx = ((EDIT *)ebx)->cpMax;
			cpOldMax = edx;
			eax = lParam;
			edx = eax;
			R_SIGNED(eax) = RWORD_SIGNED(eax);
			edx >>= 16;
			eax += ((EDIT *)ebx)->cpx;
			((RAEDT *)esi)->cpxmax = eax;
			eax -= ((EDIT *)ebx)->cpx;
			eax = GetCharFromPos(ebx, ((RAEDT *)esi)->cpy, eax, edx);
			temp1 = eax;
			eax--;
			eax = IsCharLeadByte(ebx, eax);
			edx = temp1;
			edx += eax;
			eax = edx;
			((EDIT *)ebx)->cpMin = eax;
			((EDIT *)ebx)->cpMax = eax;
			eax = InvalidateSelection(ebx, ((EDIT *)ebx)->edta.hwnd, cpOldMin, cpOldMax);
			eax = InvalidateSelection(ebx, ((EDIT *)ebx)->edtb.hwnd, cpOldMin, cpOldMax);
			eax = GetWordStart(ebx, ((EDIT *)ebx)->cpMin, 0);
			((EDIT *)ebx)->cpMin = eax;
			eax = GetWordEnd(ebx, eax, 0);
			((EDIT *)ebx)->cpMax = eax;
			eax = InvalidateSelection(ebx, ((EDIT *)ebx)->edta.hwnd, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
			eax = InvalidateSelection(ebx, ((EDIT *)ebx)->edtb.hwnd, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
			eax = SelChange(ebx, SEL_TEXT);
			eax = ((EDIT *)ebx)->cpMin;
			if(eax!=((EDIT *)ebx)->cpMax)
			{
				eax = LoadCursor(0, IDC_ARROW);
				eax = SetCursor(eax);
				fOnSel = 1;
			} // endif
		}
		else
		{
			eax = ReleaseCapture();
			eax = SendMessage(((EDIT *)ebx)->hwnd, uMsg, wParam, lParam);
		} // endif
		nUndoid++;
		eax = 0;
		return eax;
	}
	else if(eax==WM_RBUTTONDOWN)
	{
		eax = ((EDIT *)ebx)->cpMin;
		if(eax==((EDIT *)ebx)->cpMax)
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
		if(((EDIT *)ebx)->nScroll)
		{
			if(!eax)
			{
				temp1 = ((RAEDT *)esi)->cpy;
				sinf.cbSize = sizeof(sinf);
				sinf.fMask = SIF_ALL;
				eax = GetScrollInfo(((RAEDT *)esi)->hvscroll, SB_CTL, &sinf);
				eax = ((EDIT *)ebx)->nScroll;
				edx = ((EDIT *)ebx)->fntinfo.fntht;
				eax *= edx;
				edx = eax;
				eax = wParam;
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
anon_6:
				} // endif
				eax = temp1;
				eax -= ((RAEDT *)esi)->cpy;
				if(eax)
				{
					temp1 = eax;
					eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
					eax = temp1;
					eax = ScrollEdit(ebx, hWin, 0, eax);
				} // endif
			}
			else if(eax==MK_CONTROL | MK_SHIFT)
			{
				eax = wParam;
				if(R_SIGNED(eax) > 0)
				{
					eax = SB_LINELEFT;
				}
				else
				{
					eax = SB_LINERIGHT;
				} // endif
				ecx = ((EDIT *)ebx)->nScroll;
				while(ecx)
				{
					temp1 = eax;

					temp2 = ecx;
					eax = PostMessage(((EDIT *)ebx)->hwnd, WM_HSCROLL, eax, ((EDIT *)ebx)->hhscroll);
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
				ecx = ((EDIT *)ebx)->fntinfo.fntht;
				// xor		edx,edx
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
				ecx = ((EDIT *)ebx)->fntinfo.fntht;
				// xor		edx,edx
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
			eax = ScrollEdit(ebx, hWin, 0, edx);
		} // endif
		if(((EDIT *)ebx)->fstyle&STYLE_SCROLLTIP)
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
					eax = MoveWindow(((EDIT *)ebx)->htlt, MpX, pt.y, 60, 15, TRUE);
					// Get the top line number
					eax = ((RAEDT *)esi)->topln;
					eax++;
					eax = DwToAscii(eax, &szLine+4);
					eax = SetWindowText(((EDIT *)ebx)->htlt, &szLine);
					eax = ShowWindow(((EDIT *)ebx)->htlt, SW_SHOWNOACTIVATE);
					eax = InvalidateRect(((EDIT *)ebx)->htlt, NULL, TRUE);
					eax = UpdateWindow(((EDIT *)ebx)->htlt);
				} // endif
			}
			else if(eax==SB_ENDSCROLL)
			{
				MpY = 0;
				fTlln = FALSE;
				eax = ShowWindow(((EDIT *)ebx)->htlt, SW_HIDE);
			} // endif
		} // endif
		eax = 0;
		return eax;
	}
	else if(eax==WM_HSCROLL)
	{
		eax = PostMessage(((EDIT *)ebx)->hwnd, uMsg, wParam, lParam);
		eax = 0;
		return eax;
	}
	else if(eax==WM_SETFOCUS)
	{
		eax = DefWindowProc(hWin, uMsg, wParam, lParam);
		eax = hWin;
		((EDIT *)ebx)->focus = eax;
		if(!(((EDIT *)ebx)->nMode&MODE_BLOCK))
		{
			eax = 2;
		}
		else
		{
			eax = 3;
		} // endif
		edx = ((EDIT *)ebx)->fntinfo.fntht;
		eax = CreateCaret(hWin, NULL, eax, edx);
		eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
		eax = SelChange(ebx, SEL_TEXT);
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
		eax = GetWindowLong(eax, 0);
		eax = SetWindowLong(hWin, 0, eax);
	}
	else if(eax==WM_WINDOWPOSCHANGED)
	{
		fSize = TRUE;
		eax = ((EDIT *)ebx)->edta.hwnd;
		if(eax==hWin)
		{
			eax = GetClientRect(hWin, & ((EDIT *)ebx)->edta.rc);
			eax = ((EDIT *)ebx)->fstyle;
			eax &= STYLE_NOSPLITT | STYLE_NOVSCROLL;
			if(eax!=STYLE_NOSPLITT | STYLE_NOVSCROLL)
			{
				eax = SBWT;
				((EDIT *)ebx)->edta.rc.right -= eax;
				eax = MoveWindow(((EDIT *)ebx)->edta.hvscroll, ((EDIT *)ebx)->edta.rc.right, 0, SBWT, ((EDIT *)ebx)->edta.rc.bottom, TRUE);
			}
			else
			{
				eax = MoveWindow(((EDIT *)ebx)->edta.hvscroll, 0, 0, 0, 0, TRUE);
			} // endif
		}
		else
		{
			eax = GetClientRect(hWin, & ((EDIT *)ebx)->edtb.rc);
			eax = ((EDIT *)ebx)->fstyle;
			eax &= STYLE_NOSPLITT | STYLE_NOVSCROLL;
			if(eax!=STYLE_NOSPLITT | STYLE_NOVSCROLL)
			{
				eax = SBWT;
				((EDIT *)ebx)->edtb.rc.right -= eax;
				ecx = ((EDIT *)ebx)->edtb.rc.bottom;
				edx = 0;
				if(!((EDIT *)ebx)->nsplitt)
				{
					if(!(((EDIT *)ebx)->fstyle&STYLE_NOSPLITT))
					{
						edx = BTNHT;
						ecx -= edx;
					} // endif
				} // endif
				eax = MoveWindow(((EDIT *)ebx)->edtb.hvscroll, ((EDIT *)ebx)->edtb.rc.right, edx, SBWT, ecx, TRUE);
			}
			else
			{
				eax = MoveWindow(((EDIT *)ebx)->edtb.hvscroll, 0, 0, 0, 0, TRUE);
			} // endif
		} // endif
		eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
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

	void SetBlock(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		eax = GetBlockRects(ebx, &oldrects);
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
		eax = GetLineFromYp(ebx, eax);
		((EDIT *)ebx)->blrg.lnMin = eax;
		if(!fShift)
		{
			((EDIT *)ebx)->blrg.lnMax = eax;
		} // endif
		edx = ((EDIT *)ebx)->linenrwt;
		edx += ((EDIT *)ebx)->selbarwt;
		eax = pt.x;
		if(R_SIGNED(eax) < R_SIGNED(edx))
		{
			eax = edx;
		}
		else if(eax>((RAEDT *)esi)->rc.right)
		{
			eax = ((RAEDT *)esi)->rc.right;
		} // endif
		eax -= ((EDIT *)ebx)->linenrwt;
		eax -= ((EDIT *)ebx)->selbarwt;
		eax += ((EDIT *)ebx)->cpx;
		ecx = ((EDIT *)ebx)->fntinfo.fntwt;
		// cdq
		R_SIGNED(eax) /= TO_R_SIGNED(ecx);
		((EDIT *)ebx)->blrg.clMin = eax;
		if(!fShift)
		{
			((EDIT *)ebx)->blrg.clMax = eax;
		} // endif
		eax = GetBlockCp(ebx, ((EDIT *)ebx)->blrg.lnMin, ((EDIT *)ebx)->blrg.clMin);
		((EDIT *)ebx)->cpMin = eax;
		((EDIT *)ebx)->cpMax = eax;
		eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
		eax = InvalidateBlock(ebx, &oldrects);
		return;

ErrBeep:
		eax = MessageBeep(MB_ICONHAND);
		eax = 0;
		return;

	}

	void SetScroll(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		sinf.cbSize = sizeof(sinf);
		sinf.fMask = SIF_ALL;
		sinf.nMin = 0;
		eax = ((EDIT *)ebx)->rpLineFree;
		eax /= 4;
		eax -= ((EDIT *)ebx)->nHidden;
		ecx = ((EDIT *)ebx)->fntinfo.fntht;
		eax *= ecx;
		if(eax<((RAEDT *)esi)->rc.bottom)
		{
			eax = ((RAEDT *)esi)->rc.bottom;
			// xor		edx,edx
			eax /= ecx;
			eax *= ecx;
		} // endif
		sinf.nMax = eax;
		eax = ((RAEDT *)esi)->rc.bottom;
		// xor		edx,edx
		eax /= ecx;
		eax *= ecx;
		sinf.nPage = eax;
		eax = ((RAEDT *)esi)->cpy;
		sinf.nPos = eax;
		eax = SetScrollInfo(((RAEDT *)esi)->hvscroll, SB_CTL, &sinf, TRUE);
		eax = ((EDIT *)ebx)->fntinfo.fntwt;
		eax <<= 4;
		sinf.nPage = eax;
		eax <<= 5;
		sinf.nMax = eax;
		eax = ((EDIT *)ebx)->cpx;
		sinf.nPos = eax;
		eax = SetScrollInfo(((EDIT *)ebx)->hhscroll, SB_CTL, &sinf, TRUE);
		return;

	}

} // RAEditProc

REG_T GetText(DWORD hMem, DWORD cpMin, DWORD cpMax, DWORD lpText, DWORD fLf)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD nLf;

	nLf = 0;
	ebx = hMem;
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
	eax = GetCharPtr(ebx, cpMax);
	cpMax = ecx;
	eax = GetCharPtr(ebx, cpMin);
	cpMin = ecx;
	ecx = eax;
	edx = cpMin;
	esi = ((EDIT *)ebx)->hLine;
	esi += ((EDIT *)ebx)->rpLine;
	while(edx<cpMax)
	{
		eax = ((EDIT *)ebx)->hChars;
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
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	PAINTSTRUCT ps;
	BYTE buffer[16];
	DWORD hFnt;

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
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	if(fHorz)
	{
		eax = GetDeviceCaps(hDC, LOGPIXELSX);
	}
	else
	{
		eax = GetDeviceCaps(hDC, LOGPIXELSY);
	} // endif
	// mov		ecx,lSize
	// mul		ecx
	// mov		ecx,1440
	// div		ecx
	eax = MulDiv(lSize, 1440, eax);
	return eax;

} // ConvTwipsToPixels

// The edit controls callback (WndProc).
REG_T RAWndProc(HWND hWin, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
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

	auto void SetToolTip(void);
	auto void SizeIt(void);
	auto void AllocMem(void);
	auto void RelMem(void);

	// Get memory pointer
	eax = GetWindowLong(hWin, 0);
	if(eax)
	{
		ebx = eax;
		eax = ((EDIT *)ebx)->focus;
		if(eax==((EDIT *)ebx)->edta.hwnd)
		{
			esi = &((EDIT *)ebx)->edta;
		}
		else
		{
			esi = &((EDIT *)ebx)->edtb;
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
			ecx = ((EDIT *)ebx)->fntinfo.fntht;
			if(ecx)
			{
				eax = ((EDIT *)ebx)->edta.cpy;
				// xor		edx,edx
				eax /= ecx;
				temp1 = eax;
				eax = ((EDIT *)ebx)->edtb.cpy;
				// xor		edx,edx
				eax /= ecx;
				temp2 = eax;
			}
			else
			{
				temp1 = 0;
				temp2 = 0;
			} // endif
			eax = wParam;
			((EDIT *)ebx)->fntinfo.linespace = eax;
			eax = SetFont(ebx, lParam);
			ecx = ((EDIT *)ebx)->fntinfo.fntht;
			eax = temp2;
			eax *= ecx;
			((EDIT *)ebx)->edtb.cpy = eax;
			eax = temp1;
			eax *= ecx;
			((EDIT *)ebx)->edta.cpy = eax;
			eax = 0;
			((EDIT *)ebx)->edta.topyp = eax;
			((EDIT *)ebx)->edta.topln = eax;
			((EDIT *)ebx)->edta.topcp = eax;
			((EDIT *)ebx)->edtb.topyp = eax;
			((EDIT *)ebx)->edtb.topln = eax;
			((EDIT *)ebx)->edtb.topcp = eax;
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
			if(!((EDIT *)ebx)->fntinfo.monospace)
			{
				eax = ((EDIT *)ebx)->nMode;
				if(eax&MODE_BLOCK)
				{
					eax ^= MODE_BLOCK;
					eax = SendMessage(hWin, REM_SETMODE, eax, 0);
				} // endif
			} // endif
			eax = GetFocus();
			if(eax==((EDIT *)ebx)->focus && eax)
			{
				eax = SetFocus(hWin);
			} // endif
			eax = 0;
			return eax;

		case REM_GETFONT:
			// wParam=0
			// lParam=lpRAFONT
			edx = lParam;
			eax = ((EDIT *)ebx)->fnt.hFont;
			((RAFONT *)edx)->hFont = eax;
			eax = ((EDIT *)ebx)->fnt.hIFont;
			((RAFONT *)edx)->hIFont = eax;
			eax = ((EDIT *)ebx)->fnt.hLnrFont;
			((RAFONT *)edx)->hLnrFont = eax;
			eax = ((EDIT *)ebx)->fntinfo.linespace;
			return eax;

		case REM_SETCOLOR:
			// wParam=0
			// lParam=lpRACOLOR
			edx = lParam;
			eax = ((RACOLOR *)edx)->bckcol;
			((EDIT *)ebx)->clr.bckcol = eax;
			eax = ((RACOLOR *)edx)->txtcol;
			((EDIT *)ebx)->clr.txtcol = eax;
			eax = ((RACOLOR *)edx)->selbckcol;
			if(eax==((RACOLOR *)edx)->bckcol)
			{
				eax ^= 0x03F3F3F;
			} // endif
			((EDIT *)ebx)->clr.selbckcol = eax;
			eax = ((RACOLOR *)edx)->seltxtcol;
			((EDIT *)ebx)->clr.seltxtcol = eax;
			eax = ((RACOLOR *)edx)->cmntcol;
			((EDIT *)ebx)->clr.cmntcol = eax;
			eax = ((RACOLOR *)edx)->strcol;
			((EDIT *)ebx)->clr.strcol = eax;
			eax = ((RACOLOR *)edx)->oprcol;
			((EDIT *)ebx)->clr.oprcol = eax;
			eax = ((RACOLOR *)edx)->hicol1;
			((EDIT *)ebx)->clr.hicol1 = eax;
			eax = ((RACOLOR *)edx)->hicol2;
			((EDIT *)ebx)->clr.hicol2 = eax;
			eax = ((RACOLOR *)edx)->hicol3;
			((EDIT *)ebx)->clr.hicol3 = eax;
			eax = ((RACOLOR *)edx)->selbarbck;
			((EDIT *)ebx)->clr.selbarbck = eax;
			eax = ((RACOLOR *)edx)->selbarpen;
			((EDIT *)ebx)->clr.selbarpen = eax;
			eax = ((RACOLOR *)edx)->lnrcol;
			((EDIT *)ebx)->clr.lnrcol = eax;
			eax = ((RACOLOR *)edx)->numcol;
			((EDIT *)ebx)->clr.numcol = eax;
			eax = ((RACOLOR *)edx)->cmntback;
			((EDIT *)ebx)->clr.cmntback = eax;
			eax = ((RACOLOR *)edx)->strback;
			((EDIT *)ebx)->clr.strback = eax;
			eax = ((RACOLOR *)edx)->numback;
			((EDIT *)ebx)->clr.numback = eax;
			eax = ((RACOLOR *)edx)->oprback;
			((EDIT *)ebx)->clr.oprback = eax;
			eax = ((RACOLOR *)edx)->changed;
			((EDIT *)ebx)->clr.changed = eax;
			eax = ((RACOLOR *)edx)->changesaved;
			((EDIT *)ebx)->clr.changesaved = eax;
			eax = CreateBrushes(ebx);
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
			eax = InvalidateRect(((EDIT *)ebx)->hsta, NULL, FALSE);
			eax = 0;
			return eax;

		case REM_GETCOLOR:
			// wParam=0
			// lParam=lpRACOLOR
			edx = lParam;
			eax = ((EDIT *)ebx)->clr.bckcol;
			((RACOLOR *)edx)->bckcol = eax;
			eax = ((EDIT *)ebx)->clr.txtcol;
			((RACOLOR *)edx)->txtcol = eax;
			eax = ((EDIT *)ebx)->clr.selbckcol;
			((RACOLOR *)edx)->selbckcol = eax;
			eax = ((EDIT *)ebx)->clr.seltxtcol;
			((RACOLOR *)edx)->seltxtcol = eax;
			eax = ((EDIT *)ebx)->clr.cmntcol;
			((RACOLOR *)edx)->cmntcol = eax;
			eax = ((EDIT *)ebx)->clr.strcol;
			((RACOLOR *)edx)->strcol = eax;
			eax = ((EDIT *)ebx)->clr.oprcol;
			((RACOLOR *)edx)->oprcol = eax;
			eax = ((EDIT *)ebx)->clr.hicol1;
			((RACOLOR *)edx)->hicol1 = eax;
			eax = ((EDIT *)ebx)->clr.hicol2;
			((RACOLOR *)edx)->hicol2 = eax;
			eax = ((EDIT *)ebx)->clr.hicol3;
			((RACOLOR *)edx)->hicol3 = eax;
			eax = ((EDIT *)ebx)->clr.selbarbck;
			((RACOLOR *)edx)->selbarbck = eax;
			eax = ((EDIT *)ebx)->clr.selbarpen;
			((RACOLOR *)edx)->selbarpen = eax;
			eax = ((EDIT *)ebx)->clr.lnrcol;
			((RACOLOR *)edx)->lnrcol = eax;
			eax = ((EDIT *)ebx)->clr.numcol;
			((RACOLOR *)edx)->numcol = eax;
			eax = ((EDIT *)ebx)->clr.cmntback;
			((RACOLOR *)edx)->cmntback = eax;
			eax = ((EDIT *)ebx)->clr.strback;
			((RACOLOR *)edx)->strback = eax;
			eax = ((EDIT *)ebx)->clr.numback;
			((RACOLOR *)edx)->numback = eax;
			eax = ((EDIT *)ebx)->clr.oprback;
			((RACOLOR *)edx)->oprback = eax;
			eax = ((EDIT *)ebx)->clr.changed;
			((RACOLOR *)edx)->changed = eax;
			eax = ((EDIT *)ebx)->clr.changesaved;
			((RACOLOR *)edx)->changesaved = eax;
			eax = 0;
			return eax;

		case REM_SETHILITELINE:
			// wParam=Line
			// lParam=nColor
			eax = HiliteLine(ebx, wParam, lParam);
			return eax;

		case REM_GETHILITELINE:
			// wParam=Line
			// lParam=0
			eax = 0;
			eax--;
			edx = wParam;
			edx *= 4;
			if(edx<((EDIT *)ebx)->rpLineFree)
			{
				edx += ((EDIT *)ebx)->hLine;
				edx = ((LINE *)edx)->rpChars;
				edx += ((EDIT *)ebx)->hChars;
				eax = ((CHARS *)edx)->state;
				eax &= STATE_HILITEMASK;
			} // endif
			return eax;

		case REM_SETBOOKMARK:
			// wParam=Line
			// lParam=nType
			eax = SetBookMark(ebx, wParam, lParam);
			temp1 = eax;
			eax = InvalidateLine(ebx, ((EDIT *)ebx)->edta.hwnd, wParam);
			eax = InvalidateLine(ebx, ((EDIT *)ebx)->edtb.hwnd, wParam);
			eax = temp1;
			return eax;

		case REM_GETBOOKMARK:
			// wParam=Line
			// lParam=0
			eax = GetBookMark(ebx, wParam);
			return eax;

		case REM_CLRBOOKMARKS:
			// wParam=0
			// lParam=nType
			eax = ClearBookMarks(ebx, lParam);
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
			eax = 0;
			return eax;

		case REM_NXTBOOKMARK:
			// wParam=Line
			// lParam=nType
			eax = NextBookMark(ebx, wParam, lParam);
			return eax;

		case REM_PRVBOOKMARK:
			// wParam=Line
			// lParam=nType
			eax = PreviousBookMark(ebx, wParam, lParam);
			return eax;

		case REM_FINDBOOKMARK:
			// wParam=BmID
			// lParam=0
			eax = 0;
			eax--;
			ecx = wParam;
			edi = 0;
			while(edi<((EDIT *)ebx)->rpLineFree)
			{
				edx = edi;
				edx += ((EDIT *)ebx)->hLine;
				edx = ((LINE *)edx)->rpChars;
				edx += ((EDIT *)ebx)->hChars;
				if(ecx==((CHARS *)edx)->bmid)
				{
					eax = edi;
					eax /= 4;
					break;
				} // endif
				edi += sizeof(LINE);
			} // endw
			return eax;

		case REM_SETBLOCKS:
			// wParam=[lpLINERANGE]
			// lParam=0
			temp1 = nBmid;
			esi = &blockdefs;
			edi = esi+32*4;
			while(*(DWORD *)esi)
			{
				eax = ((RABLOCKDEF *)edi)->flag;
				eax >>= 16;
				if(eax==((EDIT *)ebx)->nWordGroup)
				{
					eax = SetBlocks(ebx, wParam, edi);
				} // endif
				edi = *(DWORD *)esi;
				esi += 4;
			} // endw
			eax = temp1;
			if(eax!=nBmid)
			{
				eax = InvalidateRect(((EDIT *)ebx)->edta.hwnd, NULL, FALSE);
				eax = InvalidateRect(((EDIT *)ebx)->edtb.hwnd, NULL, FALSE);
			} // endif
			eax = 0;
			return eax;

		case REM_ISLINE:
			// wParam=Line
			// lParam=lpszDef
			eax = IsLine(ebx, wParam, lParam);
			return eax;

		case REM_GETWORD:
			// wParam=BuffSize
			// lParam=lpBuff
			eax = GetWordStart(ebx, ((EDIT *)ebx)->cpMin, 0);
			esi = ((EDIT *)ebx)->rpChars;
			ecx = eax;
			ecx -= ((EDIT *)ebx)->cpLine;
			temp1 = ecx;
			temp2 = eax;
			eax = GetWordEnd(ebx, eax, 0);
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
			esi += ((EDIT *)ebx)->hChars;
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
			eax = Collapse(ebx, wParam);
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
			return eax;

		case REM_COLLAPSEALL:
			// wParam=0
			// lParam=0
			eax = CollapseAll(ebx);
			if(eax)
			{
				temp1 = esi;
				temp2 = edi;
				eax = GetLineFromCp(ebx, ((EDIT *)ebx)->cpMin);
				esi = eax;
				edi = eax;
anon_7:
				eax = IsLineHidden(ebx, esi);
				if(eax)
				{
					esi--;
					goto anon_7;
				} // endif
				if(esi!=edi)
				{
					eax = GetCpFromLine(ebx, esi);
					chrg.cpMin = eax;
					chrg.cpMax = eax;
					eax = SendMessage(hWin, EM_EXSETSEL, 0, &chrg);
				} // endif
				esi = temp2;
				edi = temp1;
				if(((EDIT *)ebx)->fsplitt)
				{
					eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
					eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
				} // endif
				eax = SendMessage(hWin, REM_VCENTER, 0, 0);
			} // endif
			return eax;

		case REM_EXPAND:
			// wParam=Line
			// lParam=0
			eax = Expand(ebx, wParam);
			temp1 = eax;
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
			eax = temp1;
			return eax;

		case REM_EXPANDALL:
			// wParam=0
			// lParam=0
			eax = ExpandAll(ebx);
			if(eax)
			{
				if(((EDIT *)ebx)->fsplitt)
				{
					eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
					eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
				} // endif
				eax = SendMessage(hWin, REM_VCENTER, 0, 0);
			} // endif
			return eax;

		case REM_LOCKLINE:
			// wParam=Line
			// lParam=TRUE/FALSE
			eax = LockLine(ebx, wParam, lParam);
			return eax;

		case REM_ISLINELOCKED:
			// wParam=Line
			// lParam=0
			eax = IsLineLocked(ebx, wParam);
			if(eax)
			{
				eax = TRUE;
			} // endif
			return eax;

		case REM_HIDELINE:
			// wParam=Line
			// lParam=TRUE/FALSE
			eax = HideLine(ebx, wParam, lParam);
			temp1 = eax;
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
			eax = temp1;
			return eax;

		case REM_ISLINEHIDDEN:
			// wParam=Line
			// lParam=0
			eax = IsLineHidden(ebx, wParam);
			if(eax)
			{
				eax = TRUE;
			} // endif
			return eax;

		case REM_AUTOINDENT:
			// wParam=0
			// lParam=TRUE/FALSE
			eax = lParam;
			((EDIT *)ebx)->fIndent = eax;
			return eax;

		case REM_TABWIDTH:
			// wParam=nChars
			// lParam=0
			eax = wParam;
			((EDIT *)ebx)->nTab = eax;
			eax = lParam;
			((EDIT *)ebx)->fExpandTab = eax;
			eax = SetFont(ebx, & ((EDIT *)ebx)->fnt);
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
			eax = 0;
			return eax;

		case REM_SELBARWIDTH:
			// wParam=nWidth
			// lParam=0
			eax = wParam;
			((EDIT *)ebx)->selbarwt = eax;
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
			eax = 0;
			return eax;

		case REM_LINENUMBERWIDTH:
			// wParam=nWidth
			// lParam=0
			eax = wParam;
			((EDIT *)ebx)->nlinenrwt = eax;
			if(((EDIT *)ebx)->linenrwt)
			{
				((EDIT *)ebx)->linenrwt = eax;
				eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
				eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
			} // endif
			eax = 0;
			return eax;

		case REM_MOUSEWHEEL:
			// wParam=nLines
			// lParam=0
			eax = wParam;
			((EDIT *)ebx)->nScroll = eax;
			return eax;

		case REM_SUBCLASS:
			// wParam=0
			// lParam=lpWndProc
			eax = SetWindowLong(((EDIT *)ebx)->edta.hwnd, GWL_WNDPROC, lParam);
			eax = SetWindowLong(((EDIT *)ebx)->edtb.hwnd, GWL_WNDPROC, lParam);
			return eax;

		case REM_SETSPLIT:
			// wParam=nSplit
			// lParam=0
			eax = wParam;
			eax &= 0x1FF;
			((EDIT *)ebx)->fsplitt = eax;
			if(!eax)
			{
				eax = ((EDIT *)ebx)->focus;
				if(eax==((EDIT *)ebx)->edta.hwnd)
				{
					eax = ((EDIT *)ebx)->edta.cpxmax;
					((EDIT *)ebx)->edtb.cpxmax = eax;
					eax = ((EDIT *)ebx)->edta.cpy;
					((EDIT *)ebx)->edtb.cpy = eax;
				} // endif
			} // endif
			SizeIt();
			eax = SetFocus(((EDIT *)ebx)->edtb.hwnd);
			eax = SetCaretVisible(((EDIT *)ebx)->edtb.hwnd, ((EDIT *)ebx)->edtb.cpy);
			return eax;

		case REM_GETSPLIT:
			// wParam=0
			// lParam=0
			eax = ((EDIT *)ebx)->fsplitt;
			return eax;

		case REM_VCENTER:
			// wParam=0
			// lParam=0
			eax = ((RAEDT *)esi)->rc.bottom;
			eax /= 1;
			ecx = ((EDIT *)ebx)->fntinfo.fntht;
			// xor		edx,edx
			eax /= ecx;
			eax *= ecx;
			temp1 = eax;
			eax = GetLineFromCp(ebx, ((EDIT *)ebx)->cpMin);
			eax = GetYpFromLine(ebx, eax);
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
			eax = InvalidateEdit(ebx, ((RAEDT *)esi)->hwnd);
			eax = 0;
			return eax;

		case REM_REPAINT:
			// wParam=0
			// lParam=TRUE/FALSE (Paint Now)
			eax = InvalidateRect(((EDIT *)ebx)->edta.hwnd, NULL, FALSE);
			eax = InvalidateRect(((EDIT *)ebx)->edta.hvscroll, NULL, TRUE);
			eax = InvalidateRect(((EDIT *)ebx)->edtb.hwnd, NULL, FALSE);
			eax = InvalidateRect(((EDIT *)ebx)->edtb.hvscroll, NULL, TRUE);
			eax = InvalidateRect(((EDIT *)ebx)->hhscroll, NULL, TRUE);
			eax = InvalidateRect(((EDIT *)ebx)->hgrip, NULL, TRUE);
			eax = InvalidateRect(((EDIT *)ebx)->hnogrip, NULL, TRUE);
			eax = InvalidateRect(((EDIT *)ebx)->hsbtn, NULL, TRUE);
			eax = InvalidateRect(((EDIT *)ebx)->hlin, NULL, TRUE);
			eax = InvalidateRect(((EDIT *)ebx)->hexp, NULL, TRUE);
			eax = InvalidateRect(((EDIT *)ebx)->hcol, NULL, TRUE);
			eax = InvalidateRect(((EDIT *)ebx)->hlock, NULL, TRUE);
			eax = InvalidateRect(((EDIT *)ebx)->hsta, NULL, TRUE);
			if(lParam)
			{
				eax = UpdateWindow(((EDIT *)ebx)->edta.hwnd);
				eax = UpdateWindow(((EDIT *)ebx)->edtb.hwnd);
			} // endif
			eax = 0;
			return eax;

		case REM_BMCALLBACK:
			// wParam=0
			// lParam=lpBmProc
			eax = lParam;
			((EDIT *)ebx)->lpBmCB = eax;
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
			((EDIT *)ebx)->fstyle = eax;
			eax = SetWindowLong(hWin, GWL_STYLE, eax);
			eax = InvalidateRect(((EDIT *)ebx)->hsta, NULL, TRUE);
			eax = 0;
			return eax;

		case REM_INVALIDATELINE:
			// wParam=nLine
			// lParam=0
			eax = InvalidateLine(ebx, ((EDIT *)ebx)->edta.hwnd, wParam);
			eax = InvalidateLine(ebx, ((EDIT *)ebx)->edtb.hwnd, wParam);
			eax = 0;
			return eax;

		case REM_SETPAGESIZE:
			// wParam=nLines
			// lParam=0
			eax = wParam;
			((EDIT *)ebx)->nPageBreak = eax;
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
			eax = 0;
			return eax;

		case REM_GETPAGESIZE:
			// wParam=0
			// lParam=0
			eax = ((EDIT *)ebx)->nPageBreak;
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
			eax = SetCommentBlocks(ebx, wParam, lParam);
			return eax;

		case REM_SETWORDGROUP:
			// wParam=0
			// lParam=nGroup (0-15)
			eax = lParam;
			eax &= 0x0F;
			((EDIT *)ebx)->nWordGroup = eax;
			return eax;

		case REM_GETWORDGROUP:
			// wParam=0
			// lParam=0
			eax = ((EDIT *)ebx)->nWordGroup;
			return eax;

		case REM_SETBMID:
			// wParam=nLine
			// lParam=nBmID
			edx = wParam;
			edx *= 4;
			if(edx<((EDIT *)ebx)->rpLineFree)
			{
				edx += ((EDIT *)ebx)->hLine;
				edx = ((LINE *)edx)->rpChars;
				edx += ((EDIT *)ebx)->hChars;
				eax = lParam;
				((CHARS *)edx)->bmid = eax;
			} // endif
			return eax;

		case REM_GETBMID:
			// wParam=nLine
			// lParam=0
			eax = 0;
			edx = wParam;
			edx *= 4;
			if(edx<((EDIT *)ebx)->rpLineFree)
			{
				edx += ((EDIT *)ebx)->hLine;
				edx = ((LINE *)edx)->rpChars;
				edx += ((EDIT *)ebx)->hChars;
				eax = ((CHARS *)edx)->bmid;
			} // endif
			return eax;

		case REM_ISCHARPOS:
			// wParam=CP
			// lParam=0
			eax = IsCharPos(ebx, wParam);
			return eax;

		case REM_HIDELINES:
			// wParam=nLine
			// lParam=nLines
			eax = 0;
			if(lParam>1)
			{
				eax = GetBookMark(ebx, wParam);
				if(!eax)
				{
					temp1 = ((EDIT *)ebx)->nHidden;
					ecx = lParam;
					edx = wParam;
					ecx--;
					while(ecx)
					{
						edx++;
						temp2 = ecx;
						temp3 = edx;
						eax = HideLine(ebx, edx, TRUE);
						if(eax)
						{
							edx = temp3;
							temp3 = edx;
							edx *= 4;
							if(edx<((EDIT *)ebx)->rpLineFree)
							{
								edx += ((EDIT *)ebx)->hLine;
								edx = ((LINE *)edx)->rpChars;
								edx += ((EDIT *)ebx)->hChars;
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
					eax = ((EDIT *)ebx)->nHidden;
					eax -= edx;
					if(eax)
					{
						temp1 = eax;
						eax = SetBookMark(ebx, wParam, 8);
						eax = ((EDIT *)ebx)->cpMin;
						if(eax>((EDIT *)ebx)->cpMax)
						{
							eax = ((EDIT *)ebx)->cpMax;
						} // endif
						((EDIT *)ebx)->cpMin = eax;
						((EDIT *)ebx)->cpMax = eax;
						eax = ((EDIT *)ebx)->rpLineFree;
						eax /= 4;
						eax -= ((EDIT *)ebx)->nHidden;
						ecx = ((EDIT *)ebx)->fntinfo.fntht;
						eax *= ecx;
						ecx = 0;
						if(eax<((EDIT *)ebx)->edta.cpy)
						{
							((EDIT *)ebx)->edta.cpy = eax;
							((EDIT *)ebx)->edta.topyp = ecx;
							((EDIT *)ebx)->edta.topln = ecx;
							((EDIT *)ebx)->edta.topcp = ecx;
						} // endif
						if(eax<((EDIT *)ebx)->edtb.cpy)
						{
							((EDIT *)ebx)->edtb.cpy = eax;
							((EDIT *)ebx)->edtb.topyp = ecx;
							((EDIT *)ebx)->edtb.topln = ecx;
							((EDIT *)ebx)->edtb.topcp = ecx;
						} // endif
						eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
						eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
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
			edx *= 4;
			if(edx<((EDIT *)ebx)->rpLineFree)
			{
				edx += ((EDIT *)ebx)->hLine;
				edx = ((LINE *)edx)->rpChars;
				edx += ((EDIT *)ebx)->hChars;
				if(lParam)
				{
					((CHARS *)edx)->state |= STATE_DIVIDERLINE;
				}
				else
				{
					((CHARS *)edx)->state &= -1 ^ STATE_DIVIDERLINE;
				} // endif
				eax = InvalidateLine(ebx, ((EDIT *)ebx)->edta.hwnd, wParam);
				eax = InvalidateLine(ebx, ((EDIT *)ebx)->edtb.hwnd, wParam);
			} // endif
			eax = 0;
			return eax;

		case REM_ISINBLOCK:
			// wParam=nLine
			// lParam=lpRABLOCKDEF
			eax = IsInBlock(ebx, wParam, lParam);
			return eax;

		case REM_TRIMSPACE:
			// wParam=nLine
			// lParam=fLeft
			eax = TrimSpace(ebx, wParam, lParam);
			if(eax)
			{
				temp1 = eax;
				eax = SelChange(ebx, SEL_TEXT);
				eax = temp1;
			} // endif
			return eax;

		case REM_SAVESEL:
			// wParam=0
			// lParam=0
			eax = ((EDIT *)ebx)->cpMin;
			((EDIT *)ebx)->savesel.cpMin = eax;
			eax = ((EDIT *)ebx)->cpMax;
			((EDIT *)ebx)->savesel.cpMax = eax;
			eax = 0;
			return eax;

		case REM_RESTORESEL:
			// wParam=0
			// lParam=0
			if(!((EDIT *)ebx)->fHideSel)
			{
				eax = ((EDIT *)ebx)->cpMin;
				if(eax!=((EDIT *)ebx)->cpMax)
				{
					eax = InvalidateSelection(ebx, ((EDIT *)ebx)->edta.hwnd, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
					eax = InvalidateSelection(ebx, ((EDIT *)ebx)->edtb.hwnd, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
				} // endif
			} // endif
			eax = GetCharPtr(ebx, ((EDIT *)ebx)->savesel.cpMax);
			((EDIT *)ebx)->cpMax = ecx;
			eax = GetCharPtr(ebx, ((EDIT *)ebx)->savesel.cpMin);
			((EDIT *)ebx)->cpMin = ecx;
			if(!((EDIT *)ebx)->fHideSel)
			{
				eax = TestExpand(ebx, ((EDIT *)ebx)->line);
				eax = ((EDIT *)ebx)->cpMin;
				if(eax!=((EDIT *)ebx)->cpMax)
				{
					eax = InvalidateSelection(ebx, ((EDIT *)ebx)->edta.hwnd, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
					eax = InvalidateSelection(ebx, ((EDIT *)ebx)->edtb.hwnd, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
					eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
					eax = SetCpxMax(ebx, ((RAEDT *)esi)->hwnd);
				} // endif
			} // endif
			eax = SelChange(ebx, SEL_TEXT);
			eax = 0;
			return eax;

		case REM_GETCURSORWORD:
			// wParam=BuffSize
			// lParam=lpBuff
			edi = lParam;
			*(BYTE *)edi = 0;
			eax = GetCursorPos(&pt);
			eax = ScreenToClient(hWin, &pt);
			eax = ((EDIT *)ebx)->selbarwt;
			eax += ((EDIT *)ebx)->linenrwt;
			if(eax<=pt.x)
			{
				eax = ChildWindowFromPoint(hWin, pt.x, pt.y);
				if(eax==((EDIT *)ebx)->edta.hwnd)
				{
					esi = &((EDIT *)ebx)->edta;
				}
				else
				{
					esi = &((EDIT *)ebx)->edtb;
				} // endif
				eax = ClientToScreen(hWin, &pt);
				eax = ScreenToClient(((RAEDT *)esi)->hwnd, &pt);
				eax = GetCharFromPos(ebx, ((RAEDT *)esi)->cpy, pt.x, pt.y);
				temp1 = eax;
				edx = eax;
				temp2 = pt.x;
				eax = GetPosFromChar(ebx, edx, &pt);
				edx = temp2;
				eax = temp1;
				edx -= ((EDIT *)ebx)->fntinfo.fntwt;
				if(edx<=pt.x)
				{
					eax = GetWordStart(ebx, eax, ((EDIT *)ebx)->nCursorWordType);
					esi = ((EDIT *)ebx)->rpChars;
					ecx = eax;
					ecx -= ((EDIT *)ebx)->cpLine;
					temp1 = ecx;
					temp2 = eax;
					eax = GetWordEnd(ebx, eax, ((EDIT *)ebx)->nCursorWordType);
					ecx = temp2;
					edx = temp1;
					eax -= ecx;
					ecx = eax;
					if(ecx>=wParam)
					{
						ecx = wParam;
						ecx--;
					} // endif
					esi += ((EDIT *)ebx)->hChars;
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
					eax = ((EDIT *)ebx)->line;
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
			edx *= 4;
			if(edx<((EDIT *)ebx)->rpLineFree)
			{
				edx += ((EDIT *)ebx)->hLine;
				edx = ((LINE *)edx)->rpChars;
				edx += ((EDIT *)ebx)->hChars;
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
			eax = ((EDIT *)ebx)->nMode;
			// .if ((EDIT *)ebx)->fOvr
			// or		eax,MODE_OVERWRITE
			// .endif
			return eax;

		case REM_SETMODE:
			// wParam=nMode
			// lParam=0
			eax = wParam;
			if(!(eax&MODE_OVERWRITE))
			{
				((EDIT *)ebx)->fOvr = FALSE;
			}
			else
			{
				((EDIT *)ebx)->fOvr = TRUE;
			} // endif
			if(!((EDIT *)ebx)->fntinfo.monospace)
			{
				eax &= -1 ^ MODE_BLOCK;
			} // endif
			edx = ((EDIT *)ebx)->nMode;
			((EDIT *)ebx)->nMode = eax;
			eax ^= edx;
			if(eax&MODE_BLOCK)
			{
				if(!((EDIT *)ebx)->fntinfo.monospace)
				{
					((EDIT *)ebx)->nMode &= -1 ^ MODE_BLOCK;
				} // endif
				if(!(((EDIT *)ebx)->nMode&MODE_BLOCK))
				{
					eax = 2;
				}
				else
				{
					eax = 3;
				} // endif
				edx = ((EDIT *)ebx)->fntinfo.fntht;
				eax = CreateCaret(((EDIT *)ebx)->focus, NULL, eax, edx);
				eax = GetCaretPos(&pt);
				eax = GetCharFromPos(ebx, ((RAEDT *)esi)->cpy, pt.x, pt.y);
				((EDIT *)ebx)->cpMin = eax;
				((EDIT *)ebx)->cpMax = eax;
				if(!(((EDIT *)ebx)->nMode&MODE_BLOCK))
				{
					eax = 0;
					((EDIT *)ebx)->blrg.lnMin = eax;
					((EDIT *)ebx)->blrg.clMin = eax;
					((EDIT *)ebx)->blrg.lnMax = eax;
					((EDIT *)ebx)->blrg.clMax = eax;
				}
				else
				{
					eax = SetBlockFromCp(ebx, ((EDIT *)ebx)->cpMin, FALSE);
				} // endif
				eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
				eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
				eax = InvalidateRect(((EDIT *)ebx)->edta.hwnd, NULL, TRUE);
				eax = InvalidateRect(((EDIT *)ebx)->edtb.hwnd, NULL, TRUE);
				eax = SelChange(ebx, SEL_TEXT);
			} // endif
			eax = 0;
			return eax;

		case REM_GETBLOCK:
			// wParam=0
			// lParam=lpBLOCKRANGE
			edx = lParam;
			eax = ((EDIT *)ebx)->blrg.lnMin;
			((BLOCKRANGE *)edx)->lnMin = eax;
			eax = ((EDIT *)ebx)->blrg.clMin;
			((BLOCKRANGE *)edx)->clMin = eax;
			eax = ((EDIT *)ebx)->blrg.lnMax;
			((BLOCKRANGE *)edx)->lnMax = eax;
			eax = ((EDIT *)ebx)->blrg.clMax;
			((BLOCKRANGE *)edx)->clMax = eax;
			eax = 0;
			return eax;

		case REM_SETBLOCK:
			// wParam=0
			// lParam=lpBLOCKRANGE
			if(((EDIT *)ebx)->nMode&MODE_BLOCK)
			{
				eax = GetBlockRects(ebx, &oldrects);
				edx = lParam;
				eax = ((BLOCKRANGE *)edx)->lnMin;
				((EDIT *)ebx)->blrg.lnMin = eax;
				eax = ((BLOCKRANGE *)edx)->clMin;
				((EDIT *)ebx)->blrg.clMin = eax;
				eax = ((BLOCKRANGE *)edx)->lnMax;
				((EDIT *)ebx)->blrg.lnMax = eax;
				eax = ((BLOCKRANGE *)edx)->clMax;
				((EDIT *)ebx)->blrg.clMax = eax;
				eax = GetBlockCp(ebx, ((EDIT *)ebx)->blrg.lnMin, ((EDIT *)ebx)->blrg.clMin);
				((EDIT *)ebx)->cpMin = eax;
				((EDIT *)ebx)->cpMax = eax;
				eax = InvalidateBlock(ebx, &oldrects);
				eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
				eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
				eax = SelChange(ebx, SEL_TEXT);
			} // endif
			eax = 0;
			return eax;

		case REM_BLOCKINSERT:
			if(((EDIT *)ebx)->nMode&MODE_BLOCK)
			{
				eax = IsSelectionLocked(ebx, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
				if(eax!=0)
				{
					goto ErrBeep;
				} // endif
				nUndoid++;
				eax = ((EDIT *)ebx)->blrg.lnMin;
				edx = ((EDIT *)ebx)->blrg.lnMax;
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
				eax = Paste(ebx, ((EDIT *)ebx)->focus, eax);
				eax = temp1;
				eax = GlobalFree(eax);
				nUndoid++;
				eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
				eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
			} // endif
			eax = 0;
			return eax;

		case REM_LOCKUNDOID:
			if(wParam)
			{
				eax = nUndoid;
				eax++;
				((EDIT *)ebx)->lockundoid = eax;
				eax++;
				nUndoid = eax;
			}
			else
			{
				((EDIT *)ebx)->lockundoid = 0;
			} // endif
			eax = 0;
			return eax;

		case REM_ADDBLOCKDEF:
			eax = SetBlockDef(lParam);
			eax = 0;
			return eax;

		case REM_CONVERT:
			if(!(((EDIT *)ebx)->nMode&MODE_BLOCK))
			{
				eax = IsSelectionLocked(ebx, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
				if(eax!=0)
				{
					goto ErrBeep;
				} // endif
				if(wParam==CONVERT_TABTOSPACE || wParam==CONVERT_SPACETOTAB)
				{
					eax = ConvertIndent(ebx, wParam);
				}
				else
				{
					eax = ConvertCase(ebx, wParam);
				} // endif
				eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
				eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
				eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
			} // endif
			eax = 0;
			return eax;

		case REM_BRACKETMATCH:
			if(lParam)
			{
				edx = lParam;
				ecx = &bracketstart;
				while(*(BYTE *)edx && *(BYTE *)edx!=',')
				{
					RBYTE_LOW(eax) = *(BYTE *)edx;
					*(BYTE *)ecx = RBYTE_LOW(eax);
					edx++;
					ecx++;
				} // endw
				*(BYTE *)ecx = 0;
				ecx = &bracketend;
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
				ecx = &bracketcont;
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
				eax = ((EDIT *)ebx)->cpMin;
				if(eax==((EDIT *)ebx)->cpMax)
				{
					eax = GetChar(ebx, ((EDIT *)ebx)->cpMin);
					eax = BracketMatch(ebx, eax, ((EDIT *)ebx)->cpMin);
				} // endif
			} // endif
			return eax;

		case REM_COMMAND:
			eax = GetFocus();
			if(eax==((EDIT *)ebx)->edta.hwnd || eax==((EDIT *)ebx)->edtb.hwnd)
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
			eax = GetWordStart(ebx, edx, 0);
			esi = ((EDIT *)ebx)->rpChars;
			eax -= ((EDIT *)ebx)->cpLine;
			esi += ((EDIT *)ebx)->hChars;
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
			eax = CollapseGetEnd(ebx, wParam);
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
			((EDIT *)ebx)->fLock = eax;
			eax = CheckDlgButton(hWin, -5, eax);
			eax = 0;
			return eax;

		case REM_GETLOCK:
			// wParam=0
			// lParam=0
			eax = ((EDIT *)ebx)->fLock;
			return eax;

		case REM_GETWORDFROMPOS:
			// wParam=cp
			// lParam=lpBuff
			eax = GetWordStart(ebx, wParam, 0);
			esi = ((EDIT *)ebx)->rpChars;
			ecx = eax;
			ecx -= ((EDIT *)ebx)->cpLine;
			temp1 = ecx;
			temp2 = eax;
			eax = GetWordEnd(ebx, eax, 0);
			ecx = temp2;
			edx = temp1;
			eax -= ecx;
			ecx = eax;
			edi = lParam;
			esi += ((EDIT *)ebx)->hChars;
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
			eax = NoBlockLine(ebx, wParam, lParam);
			return eax;

		case REM_ISLINENOBLOCK:
			// wParam=Line
			// lParam=0
			eax = IsLineNoBlock(ebx, wParam);
			if(eax)
			{
				eax = TRUE;
			} // endif
			return eax;

		case REM_SETALTHILITELINE:
			// wParam=nLine
			// lParam=TRUE/FALSE
			eax = AltHiliteLine(ebx, wParam, lParam);
			return eax;

		case REM_ISLINEALTHILITE:
			// wParam=nLine
			// lParam=0
			eax = IsLineAltHilite(ebx, wParam);
			if(eax)
			{
				eax = TRUE;
			} // endif
			return eax;

		case REM_SETCURSORWORDTYPE:
			// wParam=Type
			// lParam=0
			eax = wParam;
			((EDIT *)ebx)->nCursorWordType = eax;
			return eax;

		case REM_SETBREAKPOINT:
			// wParam=nLine
			// lParam=TRUE/FALSE
			eax = SetBreakpoint(ebx, wParam, lParam);
			eax = InvalidateLine(ebx, ((EDIT *)ebx)->edta.hwnd, wParam);
			eax = InvalidateLine(ebx, ((EDIT *)ebx)->edtb.hwnd, wParam);
			return eax;

		case REM_NEXTBREAKPOINT:
			// wParam=nLine
			// lParam=0
			eax = NextBreakpoint(ebx, wParam);
			return eax;

		case REM_GETLINESTATE:
			// wParam=nLine
			// lParam=0
			eax = GetLineState(ebx, wParam);
			return eax;

		case REM_SETERROR:
			// wParam=nLine
			// lParam=nErrID
			eax = SetError(ebx, wParam, lParam);
			eax = InvalidateLine(ebx, ((EDIT *)ebx)->edta.hwnd, wParam);
			eax = InvalidateLine(ebx, ((EDIT *)ebx)->edtb.hwnd, wParam);
			return eax;
		case REM_GETERROR:
			// wParam=nLine
			// lParam=0
			eax = GetError(ebx, wParam);
			return eax;

		case REM_NEXTERROR:
			// wParam=nLine
			// lParam=0
			eax = NextError(ebx, wParam);
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
			eax = SetRedText(ebx, wParam, lParam);
			eax = InvalidateLine(ebx, ((EDIT *)ebx)->edta.hwnd, wParam);
			eax = InvalidateLine(ebx, ((EDIT *)ebx)->edtb.hwnd, wParam);
			return eax;

		case REM_SETSTYLEEX:
			eax = wParam;
			((EDIT *)ebx)->fstyleex = eax;
			eax = InvalidateRect(hWin, NULL, TRUE);
			return eax;

		case REM_GETUNICODE:
			eax = ((EDIT *)ebx)->funicode;
			return eax;

		case REM_SETUNICODE:
			eax = wParam;
			((EDIT *)ebx)->funicode = eax;
			return eax;

		case REM_SETCHANGEDSTATE:
			eax = SetChangedState(ebx, wParam);
			return eax;

		case REM_SETTOOLTIP:
			eax = wParam;
			if(eax==1)
			{
				eax = ((EDIT *)ebx)->hsta;
			}
			else if(eax==2)
			{
				eax = ((EDIT *)ebx)->hsbtn;
			}
			else if(eax==3)
			{
				eax = ((EDIT *)ebx)->hlin;
			}
			else if(eax==4)
			{
				eax = ((EDIT *)ebx)->hexp;
			}
			else if(eax==5)
			{
				eax = ((EDIT *)ebx)->hcol;
			}
			else if(eax==6)
			{
				eax = ((EDIT *)ebx)->hlock;
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
			((EDIT *)ebx)->fhilite = eax;
			eax = HiliteLine(ebx, ((EDIT *)ebx)->line, ((EDIT *)ebx)->fhilite);
			return eax;

		case REM_GETUNDO:
			eax = GetUndo(ebx, wParam, lParam);
			return eax;

		case REM_SETUNDO:
			eax = SetUndo(ebx, wParam, lParam);
			return eax;

		case REM_GETLINEBEGIN:
			eax = GetLineBegin(ebx, wParam);
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
			eax = GetCharPtr(ebx, lParam);
			eax = edx;
			return eax;

		case EM_EXSETSEL:
			// wParam=0
			// lParam=lpCHARRANGE
			if(!(((EDIT *)ebx)->nMode&MODE_BLOCK))
			{
				if(!((EDIT *)ebx)->fHideSel)
				{
					eax = ((EDIT *)ebx)->cpMin;
					if(eax!=((EDIT *)ebx)->cpMax)
					{
						eax = InvalidateSelection(ebx, ((EDIT *)ebx)->edta.hwnd, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
						eax = InvalidateSelection(ebx, ((EDIT *)ebx)->edtb.hwnd, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
					} // endif
				} // endif
				edi = lParam;
				eax = GetCharPtr(ebx, ((CHARRANGE *)edi)->cpMax);
				((EDIT *)ebx)->cpMax = ecx;
				eax = GetCharPtr(ebx, ((CHARRANGE *)edi)->cpMin);
				((EDIT *)ebx)->cpMin = ecx;
				temp1 = edx;
				if(!((EDIT *)ebx)->fHideSel)
				{
					eax = TestExpand(ebx, ((EDIT *)ebx)->line);
					eax = ((EDIT *)ebx)->cpMin;
					if(eax!=((EDIT *)ebx)->cpMax)
					{
						eax = InvalidateSelection(ebx, ((EDIT *)ebx)->edta.hwnd, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
						eax = InvalidateSelection(ebx, ((EDIT *)ebx)->edtb.hwnd, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
						eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
						eax = SetCpxMax(ebx, ((RAEDT *)esi)->hwnd);
					} // endif
				} // endif
				eax = SelChange(ebx, SEL_TEXT);
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
			eax = ((EDIT *)ebx)->cpMin;
			ecx = ((EDIT *)ebx)->cpMax;
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
			if(!(((EDIT *)ebx)->nMode&MODE_BLOCK))
			{
				eax = FindTextEx(ebx, wParam, lParam);
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
			eax = GetText(ebx, ((TEXTRANGE *)edx)->chrg.cpMin, ((TEXTRANGE *)edx)->chrg.cpMax, ((TEXTRANGE *)edx)->lpstrText, FALSE);
			return eax;

		case EM_FINDWORDBREAK:
			// wParam=uFlags
			// lParam=cp
			eax = wParam;
			if(eax==WB_MOVEWORDLEFT)
			{
				eax = GetWordStart(ebx, lParam, 0);
			}
			else if(eax==WB_MOVEWORDRIGHT)
			{
				eax = GetWordEnd(ebx, lParam, 0);
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
			if(!(((EDIT *)ebx)->nMode&MODE_BLOCK))
			{
				edx = ((EDIT *)ebx)->hUndo;
				edx += ((EDIT *)ebx)->rpUndo;
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
			eax = Redo(ebx, ((RAEDT *)esi)->hwnd);
			nUndoid++;
			return eax;

		case EM_HIDESELECTION:
			// wParam=TRUE/FALSE
			// lParam=0
			eax = wParam;
			((EDIT *)ebx)->fHideSel = eax;
			return eax;

		case EM_GETSELTEXT:
			// wParam=0
			// lParam=lpBuff
			eax = GetText(ebx, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax, lParam, FALSE);
			return eax;

		case EM_CANPASTE:
			// wParam=CF_TEXT
			// lParam=0
			eax = IsClipboardFormatAvailable(wParam);
			return eax;

		case EM_STREAMIN:
			// wParam=SF_TEXT
			// lParam=lpStream
			eax = StreamIn(ebx, lParam);
			eax = 0;
			((EDIT *)ebx)->edta.cpy = eax;
			((EDIT *)ebx)->edta.cpxmax = eax;
			((EDIT *)ebx)->edta.topyp = eax;
			((EDIT *)ebx)->edta.topln = eax;
			((EDIT *)ebx)->edta.topcp = eax;
			((EDIT *)ebx)->edtb.cpy = eax;
			((EDIT *)ebx)->edtb.cpxmax = eax;
			((EDIT *)ebx)->edtb.topyp = eax;
			((EDIT *)ebx)->edtb.topln = eax;
			((EDIT *)ebx)->edtb.topcp = eax;
			((EDIT *)ebx)->cpMin = eax;
			((EDIT *)ebx)->cpMax = eax;
			((EDIT *)ebx)->blrg.lnMin = eax;
			((EDIT *)ebx)->blrg.clMin = eax;
			((EDIT *)ebx)->blrg.lnMax = eax;
			((EDIT *)ebx)->blrg.clMax = eax;
			((EDIT *)ebx)->line = eax;
			((EDIT *)ebx)->cpx = eax;
			((EDIT *)ebx)->cpLine = eax;
			((EDIT *)ebx)->rpLine = eax;
			((EDIT *)ebx)->rpChars = eax;
			eax = GetCharPtr(ebx, 0);
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
			eax = SetCaret(ebx, 0);
			eax = SelChange(ebx, SEL_TEXT);
			nUndoid++;
			eax = 0;
			return eax;

		case EM_STREAMOUT:
			// wParam=SF_TEXT
			// lParam=lpStream
			eax = StreamOut(ebx, lParam);
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
				// xor		edx,edx
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
				// xor		edx,edx
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
				ecx = ((EDIT *)ebx)->nTab;
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
			eax = GetCharFromPos(ebx, ((RAEDT *)esi)->cpy, ((POINT *)edx)->x, ((POINT *)edx)->y);
			return eax;

		case EM_POSFROMCHAR:
			// wParam=lpPoint
			// lParam=cp
			eax = GetPosFromChar(ebx, lParam, wParam);
			edx = wParam;
			eax = ((EDIT *)ebx)->cpx;
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
				eax = ((EDIT *)ebx)->cpMin;
			} // endif
			eax = GetCharPtr(ebx, eax);
			eax = edx;
			return eax;

		case EM_LINEINDEX:
			// wParam=line
			// lParam=0
			eax = wParam;
			if(eax==-1)
			{
				eax = ((EDIT *)ebx)->line;
			} // endif
			eax = GetCpFromLine(ebx, eax);
			return eax;

		case EM_GETLINE:
			// wParam=line
			// lParam=lpBuff
			edx = wParam;
			edx *= 4;
			if(edx<((EDIT *)ebx)->rpLineFree)
			{
				edx += ((EDIT *)ebx)->hLine;
				edx = ((LINE *)edx)->rpChars;
				edx += ((EDIT *)ebx)->hChars;
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
			eax = GetLineFromCp(ebx, wParam);
			edx = eax;
			edx *= 4;
			edx += ((EDIT *)ebx)->hLine;
			edx = ((LINE *)edx)->rpChars;
			edx += ((EDIT *)ebx)->hChars;
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
			edx = ((EDIT *)ebx)->fntinfo.fntwt;
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
			eax += ((EDIT *)ebx)->cpx;
			((EDIT *)ebx)->cpx = eax;
			eax = lParam;
			edx = ((EDIT *)ebx)->fntinfo.fntht;
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
			eax = InvalidateEdit(ebx, ((RAEDT *)esi)->hwnd);
			eax = TRUE;
			return eax;

		case EM_SCROLLCARET:
			// wParam=0
			// lParam=0
			eax = SetCaretVisible(((RAEDT *)esi)->hwnd, ((RAEDT *)esi)->cpy);
			eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
			eax = TRUE;
			return eax;

		case EM_SETSEL:
			// wParam=cpMin
			// lParam=cpMax
			if(!(((EDIT *)ebx)->nMode&MODE_BLOCK))
			{
				if(!((EDIT *)ebx)->fHideSel)
				{
					eax = ((EDIT *)ebx)->cpMin;
					if(eax!=((EDIT *)ebx)->cpMax)
					{
						eax = InvalidateSelection(ebx, ((EDIT *)ebx)->edta.hwnd, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
						eax = InvalidateSelection(ebx, ((EDIT *)ebx)->edtb.hwnd, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
					} // endif
				} // endif
				eax = GetCharPtr(ebx, lParam);
				((EDIT *)ebx)->cpMax = ecx;
				eax = GetCharPtr(ebx, wParam);
				((EDIT *)ebx)->cpMin = ecx;
				if(!((EDIT *)ebx)->fHideSel)
				{
					eax = TestExpand(ebx, ((EDIT *)ebx)->line);
					eax = ((EDIT *)ebx)->cpMin;
					if(eax!=((EDIT *)ebx)->cpMax)
					{
						eax = InvalidateSelection(ebx, ((EDIT *)ebx)->edta.hwnd, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
						eax = InvalidateSelection(ebx, ((EDIT *)ebx)->edtb.hwnd, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
						eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
						eax = SetCpxMax(ebx, ((RAEDT *)esi)->hwnd);
					} // endif
				} // endif
				eax = SelChange(ebx, SEL_TEXT);
			} // endif
			eax = 0;
			return eax;

		case EM_GETSEL:
			// wParam=lpcpMin
			// lParam=lpcpMax
			eax = ((EDIT *)ebx)->cpMin;
			ecx = ((EDIT *)ebx)->cpMax;
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
			eax = ((EDIT *)ebx)->fChanged;
			return eax;

		case EM_SETMODIFY:
			// wParam=TRUE/FALSE
			// lParam=0
			eax = wParam;
			((EDIT *)ebx)->fChanged = eax;
			eax = InvalidateRect(((EDIT *)ebx)->hsta, NULL, TRUE);
			return eax;

		case EM_REPLACESEL:
			// wParam=TRUE/FALSE
			// lParam=lpText
			if(!wParam)
			{
				fNoSaveUndo++;
			} // endif
			eax = IsSelectionLocked(ebx, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
			if(eax!=0)
			{
				goto ErrBeep;
			} // endif
			nUndoid++;
			eax = DeleteSelection(ebx, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
			((EDIT *)ebx)->cpMin = eax;
			((EDIT *)ebx)->cpMax = eax;
			temp1 = eax;
			temp2 = ((EDIT *)ebx)->fOvr;
			((EDIT *)ebx)->fOvr = 0;
			eax = EditInsert(ebx, ((EDIT *)ebx)->cpMin, lParam);
			((EDIT *)ebx)->fOvr = temp2;
			ecx = temp1;
			((EDIT *)ebx)->cpMin += eax;
			((EDIT *)ebx)->cpMax += eax;
			if(wParam && eax)
			{
				eax = SaveUndo(ebx, UNDO_INSERTBLOCK, ecx, lParam, eax);
			} // endif
			eax = GetCharPtr(ebx, ((EDIT *)ebx)->cpMin);
			eax = SetCaretVisible(((RAEDT *)esi)->hwnd, ((RAEDT *)esi)->cpy);
			eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
			eax = SetCpxMax(ebx, ((RAEDT *)esi)->hwnd);
			eax = SelChange(ebx, SEL_TEXT);
			nUndoid++;
			eax = 0;
			fNoSaveUndo = eax;
			return eax;

		case EM_GETLINECOUNT:
			// wParam=0
			// lParam=0
			eax = ((EDIT *)ebx)->rpLineFree;
			eax /= 4;
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
			eax = ((EDIT *)ebx)->focus;
			if(eax==((EDIT *)ebx)->edtb.hwnd && ((EDIT *)ebx)->nsplitt)
			{
				eax = ((EDIT *)ebx)->nsplitt;
				eax += BTNHT;
				((RECT *)edx)->top += eax;
				((RECT *)edx)->bottom += eax;
			} // endif
			return eax;

		case EM_CANUNDO:
			// wParam=0
			// lParam=0
			eax = 0;
			if(!(((EDIT *)ebx)->nMode&MODE_BLOCK))
			{
				eax = ((EDIT *)ebx)->rpUndo;
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
			eax = Undo(ebx, ((RAEDT *)esi)->hwnd);
			nUndoid++;
			return eax;

		case EM_EMPTYUNDOBUFFER:
			// wParam=0
			// lParam=0
			edi = ((EDIT *)ebx)->hUndo;
			ecx = ((EDIT *)ebx)->cbUndo;
			eax = 0;
			((EDIT *)ebx)->rpUndo = eax;
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
		eax = Copy(ebx);
		eax = 0;
		goto Ex;
	}
	else if(eax==WM_CUT)
	{
		// wParam=0
		// lParam=0
		nUndoid++;
		eax = Cut(ebx, ((RAEDT *)esi)->hwnd);
		nUndoid++;
		goto Ex;
	}
	else if(eax==WM_PASTE)
	{
		// wParam=0
		// lParam=0
		eax = IsSelectionLocked(ebx, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
		if(eax!=0)
		{
			goto ErrBeep;
		} // endif
		nUndoid++;
		eax = Paste(ebx, ((RAEDT *)esi)->hwnd, NULL);
		nUndoid++;
		eax = 0;
		goto Ex;
	}
	else if(eax==WM_CLEAR)
	{
		// wParam=0
		// lParam=0
		eax = IsSelectionLocked(ebx, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
		if(eax!=0)
		{
			goto ErrBeep;
		} // endif
		nUndoid++;
		if(!(((EDIT *)ebx)->nMode&MODE_BLOCK))
		{
			eax = DeleteSelection(ebx, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
			((EDIT *)ebx)->cpMin = eax;
			((EDIT *)ebx)->cpMax = eax;
			eax = GetCharPtr(ebx, ((EDIT *)ebx)->cpMin);
			eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
			eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
			eax = InvalidateRect(((EDIT *)ebx)->edta.hwnd, NULL, FALSE);
			eax = InvalidateRect(((EDIT *)ebx)->edtb.hwnd, NULL, FALSE);
		}
		else
		{
			eax = DeleteSelectionBlock(ebx, ((EDIT *)ebx)->blrg.lnMin, ((EDIT *)ebx)->blrg.clMin, ((EDIT *)ebx)->blrg.lnMax, ((EDIT *)ebx)->blrg.clMax);
			eax = ((EDIT *)ebx)->blrg.clMin;
			if(eax>((EDIT *)ebx)->blrg.clMax)
			{
				eax = ((EDIT *)ebx)->blrg.clMax;
			} // endif
			edx = ((EDIT *)ebx)->blrg.lnMin;
			if(edx>((EDIT *)ebx)->blrg.lnMax)
			{
				edx = ((EDIT *)ebx)->blrg.lnMax;
			} // endif
			((EDIT *)ebx)->blrg.clMin = eax;
			((EDIT *)ebx)->blrg.lnMin = edx;
			((EDIT *)ebx)->blrg.clMax = eax;
			((EDIT *)ebx)->blrg.lnMax = edx;
			eax = GetBlockCp(ebx, edx, eax);
			((EDIT *)ebx)->cpMin = eax;
			((EDIT *)ebx)->cpMax = eax;
			eax = GetCharPtr(ebx, ((EDIT *)ebx)->cpMin);
			eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
			eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
		} // endif
		eax = SetCpxMax(ebx, ((RAEDT *)esi)->hwnd);
		eax = SelChange(ebx, SEL_TEXT);
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
		eax = EditInsert(ebx, 0, lParam);
		eax = 0;
		((EDIT *)ebx)->edta.cpy = eax;
		((EDIT *)ebx)->edta.cpxmax = eax;
		((EDIT *)ebx)->edta.topyp = eax;
		((EDIT *)ebx)->edta.topln = eax;
		((EDIT *)ebx)->edta.topcp = eax;
		((EDIT *)ebx)->edtb.cpy = eax;
		((EDIT *)ebx)->edtb.cpxmax = eax;
		((EDIT *)ebx)->edtb.topyp = eax;
		((EDIT *)ebx)->edtb.topln = eax;
		((EDIT *)ebx)->edtb.topcp = eax;
		((EDIT *)ebx)->cpMin = eax;
		((EDIT *)ebx)->cpMax = eax;
		((EDIT *)ebx)->blrg.lnMin = eax;
		((EDIT *)ebx)->blrg.clMin = eax;
		((EDIT *)ebx)->blrg.lnMax = eax;
		((EDIT *)ebx)->blrg.clMax = eax;
		((EDIT *)ebx)->line = eax;
		((EDIT *)ebx)->cpx = eax;
		((EDIT *)ebx)->cpLine = eax;
		((EDIT *)ebx)->rpLine = eax;
		((EDIT *)ebx)->rpChars = eax;
		eax = GetCharPtr(ebx, 0);
		eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
		eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
		eax = SetCaret(ebx, 0);
		eax = SelChange(ebx, SEL_TEXT);
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
			edx = ((EDIT *)ebx)->hLine;
			eax = edx;
			eax += ((EDIT *)ebx)->rpLineFree;
			while(edx<eax)
			{
				esi = ((LINE *)edx)->rpChars;
				esi += ((EDIT *)ebx)->hChars;
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
		edx = ((EDIT *)ebx)->hLine;
		edi = edx;
		edi += ((EDIT *)ebx)->rpLineFree;
		eax = ((RAEDT *)esi)->topln;
		eax *= 4;
		edx += eax;
		eax = ((RAEDT *)esi)->topcp;
		while(edx<edi)
		{
			ecx = ((LINE *)edx)->rpChars;
			ecx += ((EDIT *)ebx)->hChars;
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
		if(((EDIT *)ebx)->fresize==1)
		{
			((EDIT *)ebx)->fresize = 2;
		}
		else if(((EDIT *)ebx)->fresize==2)
		{
			eax = lParam;
			eax >>= 16;
			R_SIGNED(eax) = RWORD_SIGNED(eax);
			eax -= 2;
			edx = ((EDIT *)ebx)->rc.bottom;
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
				ecx = ((EDIT *)ebx)->rc.bottom;
				// xor		edx,edx
				eax /= ecx;
			} // endif
			((EDIT *)ebx)->fsplitt = eax;
			SizeIt();
			eax = UpdateWindow(((EDIT *)ebx)->hsbtn);
			eax = UpdateWindow(((EDIT *)ebx)->edta.hwnd);
			eax = UpdateWindow(((EDIT *)ebx)->edtb.hwnd);
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
		if(((EDIT *)ebx)->fresize)
		{
			((EDIT *)ebx)->fresize = 0;
			eax = ReleaseCapture();
			SizeIt();
			if(((EDIT *)ebx)->fsplitt==0)
			{
				eax = SetFocus(((EDIT *)ebx)->edtb.hwnd);
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
		ebx = eax;
		// Save the pointer
		eax = SetWindowLong(hWin, 0, ebx);
		eax = hWin;
		((EDIT *)ebx)->hwnd = eax;
		eax = GetParent(eax);
		((EDIT *)ebx)->hpar = eax;
		eax = GetWindowLong(hWin, GWL_STYLE);
		((EDIT *)ebx)->fstyle = eax;
		eax = GetWindowLong(((EDIT *)ebx)->hwnd, GWL_ID);
		((EDIT *)ebx)->ID = eax;
		AllocMem();
		((EDIT *)ebx)->nlinenrwt = LNRWT;
		((EDIT *)ebx)->linenrwt = 0;
		((EDIT *)ebx)->selbarwt = SELWT;
		((EDIT *)ebx)->fIndent = TRUE;
		((EDIT *)ebx)->nTab = 4;
		((EDIT *)ebx)->nScroll = 3;
		((EDIT *)ebx)->fntinfo.fntht = 10;
		((EDIT *)ebx)->clr.bckcol = BCKCLR;
		((EDIT *)ebx)->clr.txtcol = TXTCLR;
		((EDIT *)ebx)->clr.selbckcol = SELBCKCLR;
		((EDIT *)ebx)->clr.seltxtcol = SELTXTCLR;
		((EDIT *)ebx)->clr.cmntcol = CMNTCLR;
		((EDIT *)ebx)->clr.strcol = STRCLR;
		((EDIT *)ebx)->clr.oprcol = OPRCLR;
		((EDIT *)ebx)->clr.hicol1 = HILITE1;
		((EDIT *)ebx)->clr.hicol2 = HILITE2;
		((EDIT *)ebx)->clr.hicol3 = HILITE3;
		((EDIT *)ebx)->clr.selbarbck = SELBARCLR;
		((EDIT *)ebx)->clr.selbarpen = SELBARPEN;
		((EDIT *)ebx)->clr.lnrcol = LNRCLR;
		((EDIT *)ebx)->clr.numcol = NUMCLR;
		((EDIT *)ebx)->clr.cmntback = CMNTBCK;
		((EDIT *)ebx)->clr.strback = STRBCK;
		((EDIT *)ebx)->clr.numback = NUMBCK;
		((EDIT *)ebx)->clr.oprback = OPRBCK;
		((EDIT *)ebx)->clr.changed = CHANGEDCLR;
		((EDIT *)ebx)->clr.changesaved = CHANGESAVEDCLR;
		eax = CreateBrushes(ebx);

		eax = CreateWindowEx(NULL, &szToolTips, NULL, TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
		((EDIT *)ebx)->htt = eax;

		eax = CreateWindowEx(1, &szStatic, NULL, SS_NOTIFY | WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWin, -1, hInstance, 0);
		((EDIT *)ebx)->hsbtn = eax;
		edx = &szSplitterBar;
		SetToolTip();
		eax = SetWindowLong(((EDIT *)ebx)->hsbtn, GWL_WNDPROC, &SplittBtnProc);
		OldSplittBtnProc = eax;

		eax = CreateWindowEx(NULL, &szEditClassName, NULL, WS_CLIPSIBLINGS | WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWin, NULL, hInstance, 0);
		((EDIT *)ebx)->edta.hwnd = eax;
		eax = CreateWindowEx(NULL, &szScrollBar, NULL, WS_CHILD | WS_VISIBLE | SBS_VERT, 0, 0, 0, 0, ((EDIT *)ebx)->edta.hwnd, NULL, hInstance, 0);
		((EDIT *)ebx)->edta.hvscroll = eax;

		eax = CreateWindowEx(NULL, &szEditClassName, NULL, WS_CLIPSIBLINGS | WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWin, NULL, hInstance, 0);
		((EDIT *)ebx)->edtb.hwnd = eax;
		eax = CreateWindowEx(NULL, &szScrollBar, NULL, WS_CHILD | WS_VISIBLE | SBS_VERT, 0, 0, 0, 0, ((EDIT *)ebx)->edtb.hwnd, NULL, hInstance, 0);
		((EDIT *)ebx)->edtb.hvscroll = eax;

		eax = CreateWindowEx(NULL, &szScrollBar, NULL, WS_CHILD | WS_VISIBLE | SBS_HORZ, 0, 0, 0, 0, hWin, NULL, hInstance, 0);
		((EDIT *)ebx)->hhscroll = eax;
		eax = CreateWindowEx(NULL, &szScrollBar, NULL, WS_CHILD | WS_VISIBLE | SBS_SIZEGRIP, 0, 0, 0, 0, hWin, NULL, hInstance, 0);
		((EDIT *)ebx)->hgrip = eax;

		eax = CreateWindowEx(NULL, &szStatic, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWin, NULL, hInstance, 0);
		((EDIT *)ebx)->hnogrip = eax;

		eax = CreateWindowEx(NULL, &szStatic, NULL, SS_NOTIFY | WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWin, NULL, hInstance, 0);
		((EDIT *)ebx)->hsta = eax;
		edx = &szChanged;
		SetToolTip();
		eax = SetWindowLong(((EDIT *)ebx)->hsta, GWL_USERDATA, ebx);
		eax = SetWindowLong(((EDIT *)ebx)->hsta, GWL_WNDPROC, &StateProc);
		OldStateProc = eax;

		eax = CreateWindowEx(NULL, &szStatic, NULL, WS_POPUP | WS_BORDER | SS_OWNERDRAW, 0, 0, 0, 0, hWin, NULL, hInstance, 0);
		((EDIT *)ebx)->htlt = eax;

		eax = SetWindowLong(eax, GWL_WNDPROC, &FakeToolTipProc);
		OldFakeToolTipProc = eax;
		eax = SendMessage(((EDIT *)ebx)->htt, WM_GETFONT, 0, 0);
		eax = SendMessage(((EDIT *)ebx)->htlt, WM_SETFONT, eax, FALSE);

		eax = CreateWindowEx(NULL, &szButton, NULL, BS_BITMAP | BS_PUSHLIKE | BS_AUTOCHECKBOX | WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWin, -2, hInstance, 0);
		((EDIT *)ebx)->hlin = eax;
		edx = &szLineNumber;
		SetToolTip();
		eax = SendMessage(((EDIT *)ebx)->hlin, BM_SETIMAGE, IMAGE_BITMAP, hBmpLnr);

		eax = CreateWindowEx(NULL, &szButton, NULL, BS_BITMAP | BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWin, -3, hInstance, 0);
		((EDIT *)ebx)->hexp = eax;
		edx = &szExpand;
		SetToolTip();
		eax = SendMessage(((EDIT *)ebx)->hexp, BM_SETIMAGE, IMAGE_BITMAP, hBmpExp);

		eax = CreateWindowEx(NULL, &szButton, NULL, BS_BITMAP | BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWin, -4, hInstance, 0);
		((EDIT *)ebx)->hcol = eax;
		edx = &szCollapse;
		SetToolTip();
		eax = SendMessage(((EDIT *)ebx)->hcol, BM_SETIMAGE, IMAGE_BITMAP, hBmpCol);

		eax = CreateWindowEx(NULL, &szButton, NULL, BS_BITMAP | BS_PUSHLIKE | BS_AUTOCHECKBOX | WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWin, -5, hInstance, 0);
		((EDIT *)ebx)->hlock = eax;
		edx = &szLock;
		SetToolTip();
		eax = SendMessage(((EDIT *)ebx)->hlock, BM_SETIMAGE, IMAGE_BITMAP, hBmpLck);

		eax = SetWindowPos(((EDIT *)ebx)->hsta, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOSIZE); // SWP_NOREPOSITION
		eax = SetWindowPos(((EDIT *)ebx)->hsbtn, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOSIZE); // SWP_NOREPOSITION
		if(((EDIT *)ebx)->fstyle&STYLE_DRAGDROP)
		{
			eax = RegisterDragDrop(((EDIT *)ebx)->edta.hwnd, &pIDropTarget);
			eax = RegisterDragDrop(((EDIT *)ebx)->edtb.hwnd, &pIDropTarget);
		} // endif
	}
	else if(eax==WM_DESTROY)
	{
		if(((EDIT *)ebx)->fstyle&STYLE_DRAGDROP)
		{
			eax = RevokeDragDrop(((EDIT *)ebx)->edta.hwnd);
			eax = RevokeDragDrop(((EDIT *)ebx)->edtb.hwnd);
		} // endif
		eax = SetWindowLong(hWin, 0, 0);
		eax = DestroyWindow(((EDIT *)ebx)->htt);
		eax = DestroyWindow(((EDIT *)ebx)->hsbtn);
		eax = DestroyWindow(((EDIT *)ebx)->edta.hvscroll);
		eax = DestroyWindow(((EDIT *)ebx)->edtb.hvscroll);
		eax = DestroyWindow(((EDIT *)ebx)->hhscroll);
		eax = DestroyWindow(((EDIT *)ebx)->hgrip);
		eax = DestroyWindow(((EDIT *)ebx)->hnogrip);
		eax = DestroyWindow(((EDIT *)ebx)->hsta);
		eax = DestroyWindow(((EDIT *)ebx)->hlin);
		eax = DestroyWindow(((EDIT *)ebx)->hexp);
		eax = DestroyWindow(((EDIT *)ebx)->hcol);
		eax = DestroyWindow(((EDIT *)ebx)->hlock);
		eax = DestroyWindow(((EDIT *)ebx)->edta.hwnd);
		eax = DestroyWindow(((EDIT *)ebx)->edtb.hwnd);
		eax = DestroyBrushes(ebx);
		// Free memory
		RelMem();
		eax = GetProcessHeap();
		eax = HeapFree(eax, 0, ebx);
	}
	else if(eax==WM_SETFOCUS)
	{
		eax = ((EDIT *)ebx)->edta.hwnd;
		if(eax!=((EDIT *)ebx)->focus)
		{
			eax = ((EDIT *)ebx)->edtb.hwnd;
		} // endif
		eax = SetFocus(eax);
		eax = 0;
		goto Ex;
	}
	else if(eax==WM_KEYDOWN || eax==WM_CHAR || eax==WM_KEYUP)
	{
		eax = SendMessage(((EDIT *)ebx)->focus, eax, wParam, lParam);
		goto Ex;
	}
	else if(eax==WM_COMMAND)
	{
		eax = wParam;
		eax &= 0x0FFFF;
		if(RWORD(eax)==-2)
		{
			eax = IsDlgButtonChecked(hWin, -2);
			if(eax)
			{
				eax = ((EDIT *)ebx)->nlinenrwt;
				((EDIT *)ebx)->linenrwt = eax;
			}
			else
			{
				((EDIT *)ebx)->linenrwt = 0;
			} // endif
			eax = SetFocus(((EDIT *)ebx)->focus);
			SizeIt();
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
		}
		else if(RWORD(eax)==-3)
		{
			ecx = BN_CLICKED;
			ecx <<= 16;
			eax |= ecx;
			eax = SendMessage(((EDIT *)ebx)->hpar, WM_COMMAND, eax, ((EDIT *)ebx)->hexp);
			eax = SetFocus(((EDIT *)ebx)->focus);
		}
		else if(RWORD(eax)==-4)
		{
			ecx = BN_CLICKED;
			ecx <<= 16;
			eax |= ecx;
			eax = SendMessage(((EDIT *)ebx)->hpar, WM_COMMAND, eax, ((EDIT *)ebx)->hcol);
			eax = SetFocus(((EDIT *)ebx)->focus);
		}
		else if(RWORD(eax)==-5)
		{
			ecx = ((EDIT *)ebx)->fLock;
			ecx ^= 1;
			((EDIT *)ebx)->fLock = ecx;
			ecx = BN_CLICKED;
			ecx <<= 16;
			eax |= ecx;
			eax = SendMessage(((EDIT *)ebx)->hpar, WM_COMMAND, eax, ((EDIT *)ebx)->hlock);
			eax = SetFocus(((EDIT *)ebx)->focus);
		} // endif
	}
	else if(eax==WM_SIZE)
	{
		temp1 = edi;
		eax = GetWindowLong(hWin, GWL_STYLE);
		((EDIT *)ebx)->fstyle = eax;
		eax = GetClientRect(hWin, & ((EDIT *)ebx)->rc);
		eax = ((EDIT *)ebx)->fstyle;
		eax &= STYLE_NOLINENUMBER | STYLE_NOCOLLAPSE | STYLE_NOHSCROLL;
		edx = ((EDIT *)ebx)->fstyleex;
		edx &= STYLEEX_LOCK;
		if(eax!=STYLE_NOLINENUMBER | STYLE_NOCOLLAPSE | STYLE_NOHSCROLL || edx==STYLEEX_LOCK)
		{
			eax = SBWT;
			((EDIT *)ebx)->rc.bottom -= eax;
		} // endif
		SizeIt();
		edi = 0;
		if(!(((EDIT *)ebx)->fstyle&STYLE_NOLINENUMBER))
		{
			eax = MoveWindow(((EDIT *)ebx)->hlin, 0, ((EDIT *)ebx)->rc.bottom, BTNWT, SBWT, TRUE);
			edi += BTNWT;
		}
		else
		{
			eax = MoveWindow(((EDIT *)ebx)->hlin, 0, 0, 0, 0, TRUE);
		} // endif
		if(!(((EDIT *)ebx)->fstyle&STYLE_NOCOLLAPSE))
		{
			eax = MoveWindow(((EDIT *)ebx)->hexp, edi, ((EDIT *)ebx)->rc.bottom, BTNWT, SBWT, TRUE);
			edi += BTNWT;
			eax = MoveWindow(((EDIT *)ebx)->hcol, edi, ((EDIT *)ebx)->rc.bottom, BTNWT, SBWT, TRUE);
			edi += BTNWT;
		}
		else
		{
			eax = MoveWindow(((EDIT *)ebx)->hexp, 0, 0, 0, 0, TRUE);
			eax = MoveWindow(((EDIT *)ebx)->hcol, 0, 0, 0, 0, TRUE);
		} // endif
		if(((EDIT *)ebx)->fstyleex&STYLEEX_LOCK)
		{
			eax = MoveWindow(((EDIT *)ebx)->hlock, edi, ((EDIT *)ebx)->rc.bottom, BTNWT, SBWT, TRUE);
			edi += BTNWT;
		}
		else
		{
			eax = MoveWindow(((EDIT *)ebx)->hlock, 0, 0, 0, 0, TRUE);
		} // endif
		eax = ((EDIT *)ebx)->fstyle;
		eax &= STYLE_NOLINENUMBER | STYLE_NOCOLLAPSE | STYLE_NOHSCROLL;
		edx = ((EDIT *)ebx)->fstyleex;
		edx &= STYLEEX_LOCK;
		if(eax!=STYLE_NOLINENUMBER | STYLE_NOCOLLAPSE | STYLE_NOHSCROLL || edx==STYLEEX_LOCK)
		{
			ecx = ((EDIT *)ebx)->rc.right;
			ecx -= edi;
			eax = ((EDIT *)ebx)->fstyle;
			eax &= STYLE_NOSPLITT | STYLE_NOVSCROLL;
			if(eax!=STYLE_NOSPLITT | STYLE_NOVSCROLL)
			{
				ecx -= SBWT;
				eax = MoveWindow(((EDIT *)ebx)->hhscroll, edi, ((EDIT *)ebx)->rc.bottom, ecx, SBWT, TRUE);
				if(((EDIT *)ebx)->fstyle&STYLE_NOSIZEGRIP)
				{
					goto anon_9;
				} // endif
				eax = GetWindowLong(((EDIT *)ebx)->hpar, GWL_STYLE);
				if(eax&WS_MAXIMIZE)
				{
					goto anon_9;
				} // endif
				eax = GetClientRect(((EDIT *)ebx)->hpar, &rect);
				eax = ClientToScreen(((EDIT *)ebx)->hpar, &rect.right);
				eax = ((EDIT *)ebx)->rc.bottom;
				eax += SBWT;
				rect.top = eax;

				eax = ClientToScreen(hWin, &rect);
				eax = rect.bottom;
				eax -= rect.top;
				if(eax<5)
				{
					eax = ((EDIT *)ebx)->rc.right;
					eax -= SBWT;
					eax = MoveWindow(((EDIT *)ebx)->hgrip, eax, ((EDIT *)ebx)->rc.bottom, SBWT, SBWT, TRUE);
					eax = MoveWindow(((EDIT *)ebx)->hnogrip, 0, 0, 0, 0, TRUE);
				}
				else
				{
anon_9:
					eax = ((EDIT *)ebx)->rc.right;
					eax -= SBWT;
					eax = MoveWindow(((EDIT *)ebx)->hnogrip, eax, ((EDIT *)ebx)->rc.bottom, SBWT, SBWT, TRUE);
					eax = MoveWindow(((EDIT *)ebx)->hgrip, 0, 0, 0, 0, TRUE);
				} // endif
			}
			else
			{
				eax = MoveWindow(((EDIT *)ebx)->hhscroll, edi, ((EDIT *)ebx)->rc.bottom, ecx, SBWT, TRUE);
				eax = MoveWindow(((EDIT *)ebx)->hnogrip, 0, 0, 0, 0, TRUE);
				eax = MoveWindow(((EDIT *)ebx)->hgrip, 0, 0, 0, 0, TRUE);
			} // endif
		}
		else
		{
			eax = MoveWindow(((EDIT *)ebx)->hhscroll, 0, 0, 0, 0, TRUE);
			eax = MoveWindow(((EDIT *)ebx)->hgrip, 0, 0, 0, 0, TRUE);
			eax = MoveWindow(((EDIT *)ebx)->hnogrip, 0, 0, 0, 0, TRUE);
		} // endif
		eax = GetTopFromYp(ebx, ((EDIT *)ebx)->edta.hwnd, ((EDIT *)ebx)->edta.cpy);
		eax = GetTopFromYp(ebx, ((EDIT *)ebx)->edtb.hwnd, ((EDIT *)ebx)->edtb.cpy);
		edi = temp1;
		eax = 0;
		goto Ex;
	}
	else if(eax==WM_HSCROLL)
	{
		temp1 = ((EDIT *)ebx)->cpx;
		sinf.cbSize = sizeof(sinf);
		sinf.fMask = SIF_ALL;
		eax = GetScrollInfo(lParam, SB_CTL, &sinf);
		eax = wParam;
		eax &= 0x0FFFF;
		if(eax==SB_THUMBTRACK || eax==SB_THUMBPOSITION)
		{
			eax = sinf.nTrackPos;
			((EDIT *)ebx)->cpx = eax;
		}
		else if(eax==SB_LINELEFT)
		{
			eax = ((EDIT *)ebx)->fntinfo.fntwt;
			if(((EDIT *)ebx)->cpx>eax)
			{
				((EDIT *)ebx)->cpx -= eax;
			}
			else
			{
				((EDIT *)ebx)->cpx = 0;
			} // endif
		}
		else if(eax==SB_LINERIGHT)
		{
			eax = ((EDIT *)ebx)->fntinfo.fntwt;
			((EDIT *)ebx)->cpx += eax;
		}
		else if(eax==SB_PAGELEFT)
		{
			eax = sinf.nPage;
			if(((EDIT *)ebx)->cpx > eax)
			{
				((EDIT *)ebx)->cpx -= eax;
			}
			else
			{
				((EDIT *)ebx)->cpx = 0;
			} // endif
		}
		else if(eax==SB_PAGERIGHT)
		{
			eax = sinf.nPage;
			((EDIT *)ebx)->cpx += eax;
		} // endif
		edi = temp1;
		edi -= ((EDIT *)ebx)->cpx;
		eax = ScrollEdit(ebx, ((EDIT *)ebx)->edta.hwnd, edi, 0);
		eax = ScrollEdit(ebx, ((EDIT *)ebx)->edtb.hwnd, edi, 0);
	}
	else if(eax==WM_SETFONT)
	{
		eax = wParam;
		((EDIT *)ebx)->fnt.hFont = eax;
		((EDIT *)ebx)->fnt.hIFont = eax;
		((EDIT *)ebx)->fnt.hLnrFont = eax;
		eax = SetFont(ebx, & ((EDIT *)ebx)->fnt);
	}
	else if(eax==WM_GETFONT)
	{
		eax = ((EDIT *)ebx)->fnt.hFont;
		return eax;
	}
	else if(eax==WM_STYLECHANGED)
	{
		eax = GetWindowLong(hWin, GWL_STYLE);
		((EDIT *)ebx)->fstyle = eax;
	}
	else if(eax==WM_PAINT)
	{
		eax = UpdateWindow(((EDIT *)ebx)->hsta);
		eax = UpdateWindow(((EDIT *)ebx)->edta.hwnd);
		eax = UpdateWindow(((EDIT *)ebx)->edtb.hwnd);
		goto ExDef;
	}
	else if(eax==REM_RAINIT)
	{
		// WM_USER+9999 (=REM_RAINIT) is sendt to a custom control by RadASM (1.2.0.5)
		// to let the custom control fill in default design time values.
		eax = SendMessage(hWin, WM_SETTEXT, 0, &szToolTip);
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
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		ti.cbSize = sizeof(TOOLINFO);
		ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
		ti.hWnd = 0;
		ti.uId = eax;
		ti.hInst = 0;
		ti.lpszText = edx;
		eax = SendMessage(((EDIT *)ebx)->htt, TTM_DELTOOL, NULL, &ti);
		eax = SendMessage(((EDIT *)ebx)->htt, TTM_ADDTOOL, NULL, &ti);
		return;

	}

	void SizeIt(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		eax = ((EDIT *)ebx)->fsplitt;
		if(eax)
		{
			ecx = eax;
			eax = ((EDIT *)ebx)->rc.bottom;
			eax *= ecx;
			eax >>= 10;
		} // endif
		((EDIT *)ebx)->nsplitt = eax;
		if(((EDIT *)ebx)->nsplitt)
		{
			eax = MoveWindow(((EDIT *)ebx)->edta.hwnd, 0, 0, ((EDIT *)ebx)->rc.right, ((EDIT *)ebx)->nsplitt, TRUE);
			ecx = ((EDIT *)ebx)->nsplitt;
			ecx += BTNHT;
			eax = ((EDIT *)ebx)->rc.bottom;
			eax -= ecx;
			eax = MoveWindow(((EDIT *)ebx)->edtb.hwnd, 0, ecx, ((EDIT *)ebx)->rc.right, eax, TRUE);
			eax = MoveWindow(((EDIT *)ebx)->hsbtn, 0, ((EDIT *)ebx)->nsplitt, ((EDIT *)ebx)->rc.right, BTNHT, TRUE);
		}
		else
		{
			eax = MoveWindow(((EDIT *)ebx)->edta.hwnd, 0, 0, 0, 0, TRUE);
			ecx = ((EDIT *)ebx)->rc.right;
			if(R_SIGNED(ecx) < 0)
			{
				ecx = 0;
			} // endif

			edx = ((EDIT *)ebx)->rc.bottom;
			if(R_SIGNED(edx) < 0)
			{
				edx = 0;
			} // endif
			eax = MoveWindow(((EDIT *)ebx)->edtb.hwnd, 0, 0, ecx, edx, TRUE);
			eax = ((EDIT *)ebx)->rc.right;
			eax -= SBWT;
			if(!(((EDIT *)ebx)->fstyle&STYLE_NOSPLITT))
			{
				eax = MoveWindow(((EDIT *)ebx)->hsbtn, eax, 0, SBWT, BTNHT, TRUE);
			}
			else
			{
				eax = MoveWindow(((EDIT *)ebx)->hsbtn, 0, 0, 0, 0, TRUE);
			} // endif
			if(!(((EDIT *)ebx)->fstyle&STYLE_NOSTATE))
			{
				eax = MoveWindow(((EDIT *)ebx)->hsta, 0, 0, 4, 4, TRUE);
			}
			else
			{

				eax = MoveWindow(((EDIT *)ebx)->hsta, 0, 0, 0, 0, TRUE);
			} // endif
		} // endif
		return;

	}

	void AllocMem(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		eax = HeapCreate(HEAP_GENERATE_EXCEPTIONS, 256*1024, 0);
		((EDIT *)ebx)->hHeap = eax;
		// Line
		eax = xHeapAlloc(((EDIT *)ebx)->hHeap, HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY, MAXLINEMEM*8);
		((EDIT *)ebx)->hLine = eax;
		((EDIT *)ebx)->cbLine = MAXLINEMEM;
		((EDIT *)ebx)->rpLine = 0;
		((EDIT *)ebx)->rpLineFree = sizeof(LINE);
		// Chars
		eax = xHeapAlloc(((EDIT *)ebx)->hHeap, HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY, MAXCHARMEM*8);
		((EDIT *)ebx)->hChars = eax;
		eax = 0;
		((EDIT *)ebx)->cbChars = MAXCHARMEM*8;
		((EDIT *)ebx)->rpChars = eax;
		((EDIT *)ebx)->rpCharsFree = MAXFREE+sizeof(CHARS);
		edx = ((EDIT *)ebx)->hLine;
		((LINE *)edx)->rpChars = eax;
		edx = ((EDIT *)ebx)->hChars;
		((CHARS *)edx)->max = MAXFREE;
		((CHARS *)edx)->len = eax;
		((CHARS *)edx)->state = eax;
		// Undo
		eax = xHeapAlloc(((EDIT *)ebx)->hHeap, HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY, MAXUNDOMEM);
		((EDIT *)ebx)->hUndo = eax;
		((EDIT *)ebx)->cbUndo = MAXCHARMEM;
		((EDIT *)ebx)->rpUndo = 0;
		// Misc
		eax = 0;
		((EDIT *)ebx)->edta.cpy = eax;
		((EDIT *)ebx)->edta.cpxmax = eax;
		((EDIT *)ebx)->edtb.cpy = eax;
		((EDIT *)ebx)->edtb.cpxmax = eax;
		((EDIT *)ebx)->cpx = eax;
		((EDIT *)ebx)->cpMin = eax;
		((EDIT *)ebx)->cpMax = eax;
		((EDIT *)ebx)->blrg.lnMin = eax;
		((EDIT *)ebx)->blrg.clMin = eax;
		((EDIT *)ebx)->blrg.lnMax = eax;
		((EDIT *)ebx)->blrg.clMax = eax;
		((EDIT *)ebx)->fChanged = FALSE;
		((EDIT *)ebx)->cpbrst = -1;
		((EDIT *)ebx)->cpbren = -1;
		((EDIT *)ebx)->cpselbar = -1;
		((EDIT *)ebx)->fLock = FALSE;

		return;

	}

	void RelMem(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		// Free memory
		if(((EDIT *)ebx)->hHeap)
		{
			eax = HeapDestroy(((EDIT *)ebx)->hHeap);
			((EDIT *)ebx)->hHeap = 0;
		} // endif
		return;

	}

} // RAWndProc

