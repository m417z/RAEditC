#include "Position.h"

#include "Block.h"
#include "Misc.h"

REG_T GetTopFromYp(EDIT *pMem, HWND hWin, DWORD yp)
{
	REG_T eax = 0, ecx, edx, esi, edi;
	REG_T temp1, temp2, temp3;
	DWORD cp;

	eax = hWin;
	if(eax==pMem->edta.hwnd)
	{
		edx = &pMem->edta;
	}
	else
	{
		edx = &pMem->edtb;
	} // endif
	temp1 = edx;
	esi = pMem->hLine;
	temp2 = pMem->hChars;
	temp3 = edx;
	edi = pMem->fntinfo.fntht;
	eax = yp;
	eax /= edi;
	eax *= edi;
	yp = eax;
	edx = temp3;
	ecx = ((RAEDT *)edx)->topln;
	if(eax>=((RAEDT *)edx)->topyp)
	{
		eax -= ((RAEDT *)edx)->topyp;
		eax /= edi;
		edi = temp2;
		cp = 0;
		if(eax)
		{
			edx = ((LINE *)(esi+ecx*sizeof(LINE)))->rpChars;
Nxt1:
			edx = ((CHARS *)(edi+edx))->len;
			cp += edx;
			edx = ecx*sizeof(LINE);
			if(edx<pMem->rpLineFree)
			{
				ecx++;
				edx = ((LINE *)(esi+ecx*sizeof(LINE)))->rpChars;
				if(((CHARS *)(edi+edx))->state&STATE_HIDDEN)
				{
					goto Nxt1;
				} // endif
				eax--;
				if(eax!=0)
				{
					goto Nxt1;
				} // endif
			} // endif
		} // endif
	}
	else
	{
		eax -= ((RAEDT *)edx)->topyp;
		eax = -eax;
		eax /= edi;
		edi = temp2;
		cp = 0;
		if(eax)
		{
Nxt2:
			if(!ecx)
			{
				esi = temp1;
				temp1 = esi;
				((RAEDT *)esi)->topcp = ecx;
				cp = ecx;
				yp = ecx;
				goto anon_1;
			} // endif
			ecx--;
			edx = ((LINE *)(esi+ecx*sizeof(LINE)))->rpChars;
			temp2 = eax;
			eax = ((CHARS *)(edi+edx))->len;
			cp -= eax;
			eax = temp2;
			if(((CHARS *)(edi+edx))->state&STATE_HIDDEN)
			{
				goto Nxt2;
			} // endif
			eax--;
			if(eax!=0)
			{
				goto Nxt2;
			} // endif
		} // endif
	} // endif
anon_1:
	esi = temp1;
	eax = cp;
	((RAEDT *)esi)->topcp += eax;
	((RAEDT *)esi)->topln = ecx;
	eax = yp;
	((RAEDT *)esi)->topyp = eax;
	if(!pMem->edta.rc.bottom)
	{
		edx = &pMem->edta;
		temp1 = ((RAEDT *)esi)->cpy;
		((RAEDT *)edx)->cpy = temp1;
		temp1 = ((RAEDT *)esi)->topyp;
		((RAEDT *)edx)->topyp = temp1;
		temp1 = ((RAEDT *)esi)->topln;
		((RAEDT *)edx)->topln = temp1;
		temp1 = ((RAEDT *)esi)->topcp;
		((RAEDT *)edx)->topcp = temp1;
	} // endif
	return eax;

} // GetTopFromYp

// eax=Char index in line
// ecx=Char index
// edx=Line number
REG_T GetCharPtr(EDIT *pMem, DWORD cp, REG_T *pdwCharIndex, REG_T *pdwLineNumber)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1;
	REG_T rpLineMax;

	esi = pMem->hLine;
	eax = pMem->rpLineFree;
	eax += esi;
	rpLineMax = eax;
	edi = pMem->hChars;
	ecx = 0;
	eax = cp;
	edx = pMem->cpLine;
	if(eax>=edx)
	{
		ecx = edx;
		esi += pMem->rpLine;
	}
	else
	{
		edx /= 2;
		if(eax>=edx)
		{
			ecx = pMem->cpLine;
			esi += pMem->rpLine;
			while(ecx>eax)
			{
				esi -= sizeof(LINE);
				edx = ((LINE *)esi)->rpChars;
				ecx -= ((CHARS *)(edi+edx))->len;
			} // endw
		} // endif
	} // endif
	ebx = sizeof(LINE);
anon_2:
	if(esi>=rpLineMax)
	{
		edx = 0;
		edx--;
		goto anon_3;
	} // endif
	edx = ((LINE *)esi)->rpChars;
	ecx += ((CHARS *)(edi+edx))->len;
	esi += ebx;
	if(eax>=ecx)
	{
		goto anon_2;
	} // endif
	ecx -= ((CHARS *)(edi+edx))->len;
anon_3:
	esi -= sizeof(LINE);
	edi = pMem->hChars;
	edi += ((LINE *)esi)->rpChars;
	edx++;
	if(edx!=0)
	{
		temp1 = ecx;
		pMem->cpLine = ecx;
		ecx -= eax;
		ecx = -ecx;
		eax = temp1;
		eax += ecx;
		temp1 = eax;
	}
	else
	{
		temp1 = ecx;
		ecx -= ((CHARS *)edi)->len;
		pMem->cpLine = ecx;
		ecx = ((CHARS *)edi)->len;
	} // endif
	esi -= (REG_T)pMem->hLine;
	pMem->rpLine = esi;
	edx = esi;
	edx /= sizeof(LINE);
	edi -= (REG_T)pMem->hChars;
	pMem->rpChars = edi;
	pMem->line = edx;
	eax = ecx;
	ecx = temp1;
	*pdwCharIndex = ecx;
	*pdwLineNumber = edx;
	return eax;

} // GetCharPtr

// eax=Char index
REG_T GetCpFromLine(EDIT *pMem, DWORD nLine)
{
	REG_T eax = 0, ecx, edx, esi, edi;

	esi = pMem->hLine;
	edi = pMem->hChars;
	ecx = nLine;
	ecx *= sizeof(LINE);
	if(ecx>=pMem->rpLineFree)
	{
		ecx = pMem->rpLineFree;
		ecx -= sizeof(LINE);
	} // endif
	ecx /= sizeof(LINE);
	nLine = ecx;
	if(ecx>=pMem->edtb.topln)
	{
		ecx = pMem->edtb.topln;
		eax = pMem->edtb.topcp;
		while(ecx<nLine)
		{
			edx = ((LINE *)(esi+ecx*sizeof(LINE)))->rpChars;
			eax += ((CHARS *)(edi+edx))->len;
			ecx++;
		} // endw
	}
	else
	{
		ecx = pMem->edtb.topln;
		eax = pMem->edtb.topcp;
		while(ecx>nLine)
		{
			ecx--;
			edx = ((LINE *)(esi+ecx*sizeof(LINE)))->rpChars;
			eax -= ((CHARS *)(edi+edx))->len;
		} // endw
	} // endif
	return eax;

} // GetCpFromLine

REG_T GetLineFromCp(EDIT *pMem, DWORD cp)
{
	REG_T eax = 0, ecx, edx, esi, edi;

	esi = pMem->hLine;
	edi = pMem->hChars;
	eax = cp;
	if(eax>=pMem->edtb.topcp)
	{
		eax = pMem->edtb.topcp;
		ecx = pMem->edtb.topln;
		while(eax<cp)
		{
			edx = ecx*sizeof(LINE);
			if(edx>=pMem->rpLineFree)
			{
				break;
			}
			edx = ((LINE *)(esi+ecx*sizeof(LINE)))->rpChars;
			eax += ((CHARS *)(edi+edx))->len;
			ecx++;
		} // endw
		if(eax>cp)
		{
			ecx--;
		} // endif
		eax = ecx;
		eax *= sizeof(LINE);
		if(eax>=pMem->rpLineFree)
		{
			ecx = pMem->rpLineFree;
			ecx /= sizeof(LINE);
			ecx--;
		} // endif
	}
	else
	{
		eax = pMem->edtb.topcp;
		ecx = pMem->edtb.topln;
		while(eax>cp)
		{
			ecx--;
			edx = ((LINE *)(esi+ecx*sizeof(LINE)))->rpChars;
			eax -= ((CHARS *)(edi+edx))->len;
		} // endw
	} // endif
	eax = ecx;
	return eax;

} // GetLineFromCp

REG_T GetYpFromLine(EDIT *pMem, DWORD nLine)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;

	esi = pMem->hLine;
	edi = pMem->hChars;
	ecx = nLine;
	ecx *= sizeof(LINE);
	if(ecx>=pMem->rpLineFree)
	{
		ecx = pMem->rpLineFree;
		ecx -= sizeof(LINE);
	} // endif
	ecx /= sizeof(LINE);
	nLine = ecx;
	if(ecx>=pMem->edtb.topln)
	{
		ecx = pMem->edtb.topln;
		eax = pMem->edtb.topyp;
		edx = pMem->fntinfo.fntht;
		ebx = edx;
		while(ecx<nLine)
		{
			edx = ((LINE *)(esi+ecx*sizeof(LINE)))->rpChars;
			if(((CHARS *)(edi+edx))->state&STATE_HIDDEN)
			{
				goto anon_4;
			} // endif
			eax += ebx;
anon_4:
			ecx++;
		} // endw
	}
	else
	{
		ecx = pMem->edtb.topln;
		eax = pMem->edtb.topyp;
		edx = pMem->fntinfo.fntht;
		ebx = edx;
		while(ecx>nLine)
		{
			ecx--;
			edx = ((LINE *)(esi+ecx*sizeof(LINE)))->rpChars;
			if(((CHARS *)(edi+edx))->state&STATE_HIDDEN)
			{
				goto anon_5;
			} // endif
			eax -= ebx;
anon_5: ;
		} // endw
	} // endif
	return eax;

} // GetYpFromLine

REG_T GetLineFromYp(EDIT *pMem, DWORD y)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T maxln;

	eax = pMem->rpLineFree;
	eax /= sizeof(LINE);
	eax--;
	maxln = eax;
	esi = pMem->hLine;
	edi = pMem->hChars;
	eax = y;
	ecx = pMem->fntinfo.fntht;
	eax /= ecx;
	eax *= ecx;
	y = eax;
	if(eax>=pMem->edtb.topyp)
	{
		eax = pMem->edtb.topyp;
		ecx = pMem->edtb.topln;
		edx = pMem->fntinfo.fntht;
		ebx = edx;
		while(eax<y)
		{
			ecx++;
			if(ecx>=maxln)
			{
				break;
			} // endif
			edx = ((LINE *)(esi+ecx*sizeof(LINE)))->rpChars;
			if(((CHARS *)(edi+edx))->state&STATE_HIDDEN)
			{
				goto anon_6;
			} // endif
			eax += ebx;
anon_6: ;
		} // endw
	}
	else
	{
		eax = pMem->edtb.topyp;
		ecx = pMem->edtb.topln;
		edx = pMem->fntinfo.fntht;
		ebx = edx;
		while(eax>y)
		{
			ecx--;
			edx = ((LINE *)(esi+ecx*sizeof(LINE)))->rpChars;
			if(((CHARS *)(edi+edx))->state&STATE_HIDDEN)
			{
				goto anon_7;
			} // endif
			eax -= ebx;
anon_7: ;
		} // endw
	} // endif
	eax = ecx;
	return eax;

} // GetLineFromYp

REG_T GetCpFromXp(EDIT *pMem, REG_T lpChars, DWORD x, DWORD fNoAdjust)
{
	REG_T eax = 0, ecx, edx, esi, edi;
	REG_T temp1, temp2, temp3;
	REG_T hDC;
	RECT rect;
	DWORD lastright;

	auto void TestIt(void);

	eax = GetDC(pMem->hwnd);
	hDC = eax;
	eax = SelectObject(hDC, pMem->fnt.hFont);
	temp1 = eax;
	eax = pMem->cpx;
	eax = -eax;
	eax += pMem->linenrwt;
	eax += pMem->selbarwt;
	x -= eax;
	eax = 0;
	rect.left = eax;
	rect.right = eax;
	rect.top = eax;
	rect.bottom = eax;
	lastright = eax;
	esi = lpChars;
	edi = ((CHARS *)esi)->len;
	edi /= 2;
	edx = edi;
	ecx = 0;
anon_8:
	temp2 = ecx;
	temp3 = edx;
	TestIt();
	edx = temp3;
	ecx = temp2;
	edx /= 2;
	if(R_SIGNED(eax) > (SDWORD)x)
	{
		edi -= edx;
		if(edx!=0)
		{
			goto anon_8;
		} // endif
	}
	else if(R_SIGNED(eax) < (SDWORD)x)
	{
		eax = rect.right;
		lastright = eax;
		ecx = edi;
		edi += edx;
		if(edx!=0)
		{
			goto anon_8;
		} // endif
	} // endif
	edi = ecx;
	if(edi)
	{
		eax = lastright;
		rect.right = eax;
	}
	else
	{
		rect.right = edi;
	} // endif
	while(edi<((CHARS *)esi)->len)
	{
		if(*(BYTE *)(esi+edi+sizeof(CHARS))==0x0D)
		{
			break;
		} // endif
		temp2 = rect.right;
		edi++;
		TestIt();
		edi--;
		edx = temp2;
		if(!fNoAdjust)
		{
			edx -= eax;
			edx = -edx;
			edx /= 2;
			eax -= edx;
		} // endif
		if(R_SIGNED(eax) > (SDWORD)x)
		{
			break;
		} // endif
		edi++;
	} // endw
	eax = temp1;
	eax = SelectObject(hDC, eax);
	eax = ReleaseDC(pMem->hwnd, hDC);
	eax = edi;
	return eax;

	void TestIt(void)
	{
		eax = GetTextWidth(pMem, hDC, esi+sizeof(CHARS), edi, &rect);
		eax = rect.right;
		return;

	}

} // GetCpFromXp

REG_T GetPosFromChar(EDIT *pMem, DWORD cp, REG_T lpPoint)
{
	REG_T eax = 0, esi;
	REG_T temp1;
	HDC hDC;
	RECT rect;
	DWORD ln;
	DWORD y;

	eax = GetLineFromCp(pMem, cp);
	ln = eax;
	eax = GetYpFromLine(pMem, ln);
	y = eax;
	eax = GetCpFromLine(pMem, ln);
	cp -= eax;
	esi = ln;
	esi *= sizeof(LINE);
	esi += pMem->hLine;
	esi = ((LINE *)esi)->rpChars;
	esi += pMem->hChars;
	eax = GetDC(pMem->hwnd);
	hDC = eax;
	eax = SelectObject(hDC, pMem->fnt.hFont);
	temp1 = eax;
	rect.left = 0;
	rect.right = 0;
	rect.top = 0;
	rect.bottom = 0;
	eax = GetTextWidth(pMem, hDC, esi+sizeof(CHARS), cp, &rect);
	eax = temp1;
	eax = SelectObject(hDC, eax);
	eax = ReleaseDC(pMem->hwnd, hDC);
	esi = lpPoint;
	eax = rect.right;
	eax += pMem->linenrwt;
	eax += pMem->selbarwt;
	eax++;
	((POINT *)esi)->x = eax;
	eax = y;
	((POINT *)esi)->y = eax;
	return eax;

} // GetPosFromChar

REG_T GetCharFromPos(EDIT *pMem, DWORD cpy, SDWORD x, SDWORD y)
{
	REG_T eax = 0, edx;
	REG_T temp1;
	DWORD cp;

	eax = y;
	eax += cpy;
	if(R_SIGNED(eax) < 0)
	{
		eax = 0;
	}
	else
	{
		eax = GetLineFromYp(pMem, eax);
	} // endif
	temp1 = eax;
	eax = GetCpFromLine(pMem, eax);
	cp = eax;
	pMem->cpLine = eax;
	edx = temp1;
	edx *= sizeof(LINE);
	if(edx>=pMem->rpLineFree)
	{
		edx = pMem->rpLineFree;
		edx -= sizeof(LINE);
	} // endif
	eax = edx;
	eax /= sizeof(LINE);
	pMem->line = eax;
	pMem->rpLine = edx;
	edx += pMem->hLine;
	edx = ((LINE *)edx)->rpChars;
	pMem->rpChars = edx;
	edx += pMem->hChars;
	eax = x;
	eax -= pMem->cpx;
	if(eax==0)
	{
		goto Ex;
	} // endif
	eax = pMem->nMode;
	eax &= MODE_BLOCK;
	eax = GetCpFromXp(pMem, edx, x, eax);
	cp += eax;
Ex:
	eax = cp;
	return eax;

} // GetCharFromPos

REG_T GetCaretPoint(EDIT *pMem, DWORD cp, DWORD cpy, REG_T lpPoint)
{
	REG_T eax = 0, ecx, esi;
	POINT pt;

	eax = GetPosFromChar(pMem, cp, &pt);
	esi = lpPoint;
	if(pMem->nMode&MODE_BLOCK)
	{
		eax = pMem->blrg.clMin;
		ecx = pMem->fntinfo.fntwt;
		eax *= ecx;
		eax += pMem->linenrwt;
		eax += pMem->selbarwt;
	}
	else
	{
		eax = pt.x;
	} // endif
	eax -= pMem->cpx;
	((POINT *)esi)->x = eax;
	eax = pt.y;
	eax -= cpy;
	((POINT *)esi)->y = eax;
	return eax;

} // GetCaretPoint

REG_T SetCaret(EDIT *pMem, DWORD cpy)
{
	REG_T eax = 0, ecx, edx;
	POINT pt;

	eax = GetFocus();
	if(eax==pMem->focus)
	{
		eax = GetCaretPoint(pMem, pMem->cpMin, cpy, &pt);
		eax = SetCaretPos(pt.x, pt.y);
		eax = pMem->selbarwt;
		eax += pMem->linenrwt;
		ecx = pt.x;
		ecx++;
		edx = pMem->cpMax;
		edx -= pMem->cpMin;
		if(R_SIGNED(eax) <= (SDWORD)pt.x && R_SIGNED(ecx) < (SDWORD)pMem->edtb.rc.right && !edx)
		{
			eax = ShowCaret(pMem->focus);
			eax = ShowCaret(pMem->focus);
			pMem->fCaretHide = FALSE;
		}
		else if(!pMem->fCaretHide)
		{
			eax = HideCaret(pMem->focus);
			pMem->fCaretHide = TRUE;
		} // endif
	} // endif
	return eax;

} // SetCaret

REG_T ScrollEdit(EDIT *pMem, HWND hWin, DWORD x, DWORD y)
{
	REG_T eax = 0, esi;
	REG_T temp1;

	eax = hWin;
	if(eax==pMem->edta.hwnd)
	{
		esi = &pMem->edta;
	}
	else
	{
		esi = &pMem->edtb;
	} // endif
	if(((RAEDT *)esi)->rc.bottom)
	{
		if(x || y)
		{
			if(x)
			{
				temp1 = ((RAEDT *)esi)->rc.left;
				eax = pMem->selbarwt;
				eax += pMem->linenrwt;
				((RAEDT *)esi)->rc.left += eax;
				eax = ScrollWindow(hWin, x, 0, & ((RAEDT *)esi)->rc, & ((RAEDT *)esi)->rc);
				((RAEDT *)esi)->rc.left = temp1;
			} // endif
			if(y)
			{
				eax = GetTopFromYp(pMem, ((RAEDT *)esi)->hwnd, ((RAEDT *)esi)->cpy);
				eax = y;
				if(R_SIGNED(eax) < 0)
				{
					eax = -eax;
				} // endif
				eax += pMem->fntinfo.fntht;
				if(eax<((RAEDT *)esi)->rc.bottom)
				{
					eax = ScrollWindow(hWin, 0, y, & ((RAEDT *)esi)->rc, & ((RAEDT *)esi)->rc);
				}
				else
				{
					eax = InvalidateRect(hWin, NULL, FALSE);
				} // endif
			} // endif
			eax = UpdateWindow(hWin);
			eax = hWin;
			if(eax==pMem->focus)
			{
				eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
			} // endif
		} // endif
	} // endif
	return eax;

} // ScrollEdit

REG_T InvalidateEdit(EDIT *pMem, HWND hWin)
{
	REG_T eax = 0, esi;

	eax = hWin;
	if(eax==pMem->edta.hwnd)
	{
		esi = &pMem->edta;
	}
	else
	{
		esi = &pMem->edtb;
	} // endif
	if(((RAEDT *)esi)->rc.bottom)
	{
		eax = GetTopFromYp(pMem, ((RAEDT *)esi)->hwnd, ((RAEDT *)esi)->cpy);
		eax = InvalidateRect(((RAEDT *)esi)->hwnd, NULL, FALSE);
		// invoke UpdateWindow,((RAEDT *)esi)->hwnd
	} // endif
	return eax;

} // InvalidateEdit

REG_T InvalidateLine(EDIT *pMem, HWND hWin, DWORD nLine)
{
	REG_T eax = 0, ecx, esi;
	RECT rect;

	eax = hWin;
	if(eax==pMem->edta.hwnd)
	{
		esi = &pMem->edta;
	}
	else
	{
		esi = &pMem->edtb;
	} // endif
	if(((RAEDT *)esi)->rc.bottom)
	{
		eax = GetYpFromLine(pMem, nLine);
		eax -= ((RAEDT *)esi)->cpy;
		ecx = eax;
		ecx += pMem->fntinfo.fntht;
		if(R_SIGNED(ecx) > 0 && R_SIGNED(eax) < (SDWORD)((RAEDT *)esi)->rc.bottom)
		{
			rect.top = eax;
			eax += pMem->fntinfo.fntht;
			rect.bottom = eax;
			rect.left = 0;
			eax = ((RAEDT *)esi)->rc.right;
			rect.right = eax;
			eax = InvalidateRect(hWin, &rect, FALSE);
		} // endif
	} // endif
	return eax;

} // InvalidateLine

REG_T InvalidateSelection(EDIT *pMem, HWND hWin, DWORD cpMin, DWORD cpMax)
{
	REG_T eax = 0, ecx, edx, esi;
	REG_T temp1;
	DWORD nLine;

	eax = hWin;
	if(eax==pMem->edta.hwnd)
	{
		esi = &pMem->edta;
	}
	else
	{
		esi = &pMem->edtb;
	} // endif
	if(((RAEDT *)esi)->rc.bottom)
	{
		eax = cpMin;
		if(eax>cpMax)
		{
			temp1 = eax;
			eax = cpMax;
			cpMax = temp1;
			cpMin = eax;
		} // endif
		eax -= cpMax;
		eax = -eax;
		if(eax<10000)
		{
			eax = GetCharPtr(pMem, cpMin, &ecx, &edx);
			nLine = edx;
			eax = pMem->cpLine;
anon_9:
			cpMin = eax;
			eax = InvalidateLine(pMem, hWin, nLine);
			nLine++;
			eax = GetCpFromLine(pMem, nLine);
			if(eax<cpMax && eax!=cpMin)
			{
				goto anon_9;
			} // endif
		}
		else
		{
			eax = InvalidateRect(hWin, NULL, FALSE);
		} // endif
	} // endif
	return eax;

} // InvalidateSelection

REG_T SetCaretVisible(HWND hWin, DWORD cpy)
{
	REG_T eax = 0, ecx, edx, esi, edi;
	REG_T temp1, temp2;
	POINT pt;
	DWORD cpx;
	DWORD ht;
	DWORD fExpand;
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
	eax = ((RAEDT *)esi)->rc.bottom;
	if(R_SIGNED(eax) > 0)
	{
		fExpand = 0;
		ecx = pMem->line;
		ecx++;
anon_10:
		ecx--;
		eax = pMem->hLine;
		edx = ((LINE *)(eax+ecx*sizeof(LINE)))->rpChars;
		edx += pMem->hChars;
		eax = ((CHARS *)edx)->state;
		eax &= STATE_BMMASK;
		if(eax==STATE_BM2)
		{
			temp1 = ecx;
			fExpand++;
		} // endif
		if(((CHARS *)edx)->state&STATE_HIDDEN)
		{
			goto anon_10;
		} // endif
		edi = fExpand;
		fExpand = 0;
		while(edi)
		{
			ecx = temp1;
			if(ecx!=pMem->line)
			{
				eax = Expand(pMem, ecx);
				fExpand++;
			} // endif
			edi--;
		} // endw
		eax = ((RAEDT *)esi)->rc.bottom;
		ecx = pMem->fntinfo.fntht;
		eax /= ecx;
		if(!eax)
		{
			eax++;
		} // endif
		eax *= ecx;
		ht = eax;
		eax = pMem->cpx;
		cpx = eax;
		eax = GetYpFromLine(pMem, pMem->line);
		if(eax<cpy)
		{
			((RAEDT *)esi)->cpy = eax;
		}
		else
		{
			eax += pMem->fntinfo.fntht;
			eax -= cpy;
			if(eax>ht)
			{
				eax -= ht;
				((RAEDT *)esi)->cpy += eax;
			} // endif
		} // endif
		eax = GetCaretPoint(pMem, pMem->cpMin, cpy, &pt);
		edx = ((RAEDT *)esi)->rc.right;
		edx -= 16;
		eax = pt.x;
		ecx = pMem->linenrwt;
		ecx += pMem->selbarwt;
		ecx += 16+1;
		if(R_SIGNED(eax) < R_SIGNED(ecx))
		{
			if(R_SIGNED(eax) > 0)
			{
				ecx = pMem->fntinfo.fntwt;
				ecx <<= 3;
				eax += ecx;
				if(pMem->cpx>eax)
				{
					pMem->cpx -= eax;
				}
				else
				{
					pMem->cpx = 0;
				} // endif
			}
			else
			{
				ecx = pMem->fntinfo.fntwt;
				ecx <<= 3;
				eax -= ecx;
				pMem->cpx += eax;
				if(pMem->cpx>=RDWORD(eax))
				{
					pMem->cpx = 0;
				} // endif
			} // endif
		}
		else if(eax>edx)
		{
			ecx = pMem->fntinfo.fntwt;
			ecx <<= 3;
			eax -= edx;
			eax += ecx;
			pMem->cpx += eax;
		} // endif
		eax = GetTopFromYp(pMem, hWin, ((RAEDT *)esi)->cpy);
		ecx = cpx;
		ecx -= pMem->cpx;
		edx = cpy;
		edx -= ((RAEDT *)esi)->cpy;
		eax = 0;
		eax--;
		if(fExpand)
		{
			eax = InvalidateEdit(pMem, hWin);
			eax = 0;
		}
		else if(ecx || edx)
		{
			if(ecx)
			{
				temp1 = ecx;
				temp2 = edx;
				eax = ScrollEdit(pMem, pMem->edta.hwnd, ecx, edx);
				edx = temp2;
				ecx = temp1;
				eax = ScrollEdit(pMem, pMem->edtb.hwnd, ecx, edx);
			}
			else
			{
				eax = ScrollEdit(pMem, hWin, ecx, edx);
			} // endif
			eax = 0;
		} // endif
	}
	else
	{
		eax = 0;
		eax--;
	} // endif
	return eax;

} // SetCaretVisible

REG_T GetBlockCp(EDIT *pMem, DWORD nLine, DWORD nPos)
{
	REG_T eax = 0, ecx, edx, edi;

	eax = GetCpFromLine(pMem, nLine);
	edi = eax;
	eax = nPos;
	ecx = pMem->fntinfo.fntwt;
	eax *= ecx;
	eax += pMem->linenrwt;
	eax += pMem->selbarwt;
	eax -= pMem->cpx;
	edx = nLine;
	edx *= sizeof(LINE);
	if(edx>=pMem->rpLineFree)
	{
		edx = pMem->rpLineFree;
		edx -= sizeof(LINE);
	} // endif
	edx += pMem->hLine;
	edx = ((LINE *)edx)->rpChars;
	edx += pMem->hChars;
	eax = GetCpFromXp(pMem, edx, eax, TRUE);
	eax += edi;
	return eax;

} // GetBlockCp

REG_T SetCpxMax(EDIT *pMem, HWND hWin)
{
	REG_T eax = 0, esi;
	POINT pt;

	eax = hWin;
	if(eax==pMem->edta.hwnd)
	{
		esi = &pMem->edta;
	}
	else
	{
		esi = &pMem->edtb;
	} // endif
	eax = GetCaretPos(&pt);
	eax = pt.x;
	eax -= pMem->selbarwt;
	eax -= pMem->linenrwt;
	eax += pMem->cpx;
	((RAEDT *)esi)->cpxmax = eax;
	return eax;

} // SetCpxMax

REG_T SetBlockFromCp(EDIT *pMem, DWORD cp, DWORD fShift)
{
	REG_T eax = 0, ecx, edx;
	POINT pt;

	eax = cp;
	pMem->cpMin = eax;
	pMem->cpMax = eax;
	eax = GetCharPtr(pMem, cp, &ecx, &edx);
	eax = GetPosFromChar(pMem, cp, &pt);
	eax = pt.x;
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
	return eax;

} // SetBlockFromCp

