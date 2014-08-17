#include <windows.h>
#include "Data.h"

REG_T GetTopFromYp(DWORD hMem, DWORD hWin, DWORD yp)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD cp;

	ebx = hMem;
	eax = hWin;
	if(eax==((EDIT *)ebx)->edta.hwnd)
	{
		edx = &((EDIT *)ebx)->edta;
	}
	else
	{
		edx = &((EDIT *)ebx)->edtb;
	} // endif
	temp1 = edx;
	esi = ((EDIT *)ebx)->hLine;
	temp2 = ((EDIT *)ebx)->hChars;
	temp3 = edx;
	edi = ((EDIT *)ebx)->fntinfo.fntht;
	eax = yp;
	// xor		edx,edx
	eax /= edi;
	eax *= edi;
	yp = eax;
	edx = temp3;
	ecx = ((RAEDT *)edx)->topln;
	if(eax>=((RAEDT *)edx)->topyp)
	{
		eax -= ((RAEDT *)edx)->topyp;
		// xor		edx,edx
		eax /= edi;
		edi = temp2;
		cp = 0;
		if(eax)
		{
			edx = [esi+ecx*sizeof(LINE)].LINE.rpChars;
Nxt1:
			edx = ((CHARS *)(edi+edx))->len;
			cp += edx;
			edx = ecx*sizeof(LINE);
			if(edx<((EDIT *)ebx)->rpLineFree)
			{
				ecx++;
				edx = [esi+ecx*sizeof(LINE)].LINE.rpChars;
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
		// xor		edx,edx
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
			edx = [esi+ecx*sizeof(LINE)].LINE.rpChars;
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
	if(!((EDIT *)ebx)->edta.rc.bottom)
	{
		edx = &((EDIT *)ebx)->edta;
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
REG_T GetCharPtr(DWORD hMem, DWORD cp)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD rpLineMax;

	ebx = hMem;
	esi = ((EDIT *)ebx)->hLine;
	eax = ((EDIT *)ebx)->rpLineFree;
	eax += esi;
	rpLineMax = eax;
	edi = ((EDIT *)ebx)->hChars;
	ecx = 0;
	eax = cp;
	edx = ((EDIT *)ebx)->cpLine;
	if(eax>=edx)
	{
		ecx = edx;
		esi += ((EDIT *)ebx)->rpLine;
	}
	else
	{
		edx /= 1;
		if(eax>=edx)
		{
			ecx = ((EDIT *)ebx)->cpLine;
			esi += ((EDIT *)ebx)->rpLine;
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
	ebx = hMem;
	esi -= sizeof(LINE);
	edi = ((EDIT *)ebx)->hChars;
	edi += ((LINE *)esi)->rpChars;
	edx++;
	if(edx!=0)
	{
		temp1 = ecx;
		((EDIT *)ebx)->cpLine = ecx;
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
		((EDIT *)ebx)->cpLine = ecx;
		ecx = ((CHARS *)edi)->len;
	} // endif
	esi -= ((EDIT *)ebx)->hLine;
	((EDIT *)ebx)->rpLine = esi;
	edx = esi;
	edx /= 4;
	edi -= ((EDIT *)ebx)->hChars;
	((EDIT *)ebx)->rpChars = edi;
	((EDIT *)ebx)->line = edx;
	eax = ecx;
	ecx = temp1;
	return eax;

} // GetCharPtr

// eax=Char index
REG_T GetCpFromLine(DWORD hMem, DWORD nLine)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	esi = ((EDIT *)ebx)->hLine;
	edi = ((EDIT *)ebx)->hChars;
	ecx = nLine;
	ecx *= 4;
	if(ecx>=((EDIT *)ebx)->rpLineFree)
	{
		ecx = ((EDIT *)ebx)->rpLineFree;
		ecx -= sizeof(LINE);
	} // endif
	ecx /= 4;
	nLine = ecx;
	if(ecx>=((EDIT *)ebx)->edtb.topln)
	{
		ecx = ((EDIT *)ebx)->edtb.topln;
		eax = ((EDIT *)ebx)->edtb.topcp;
		while(ecx<nLine)
		{
			edx = [esi+ecx*sizeof(LINE)].LINE.rpChars;
			eax += ((CHARS *)(edi+edx))->len;
			ecx++;
		} // endw
	}
	else
	{
		ecx = ((EDIT *)ebx)->edtb.topln;
		eax = ((EDIT *)ebx)->edtb.topcp;
		while(ecx>nLine)
		{
			ecx--;
			edx = [esi+ecx*sizeof(LINE)].LINE.rpChars;
			eax -= ((CHARS *)(edi+edx))->len;
		} // endw
	} // endif
	return eax;

} // GetCpFromLine

REG_T GetLineFromCp(DWORD hMem, DWORD cp)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	esi = ((EDIT *)ebx)->hLine;
	edi = ((EDIT *)ebx)->hChars;
	eax = cp;
	if(eax>=((EDIT *)ebx)->edtb.topcp)
	{
		eax = ((EDIT *)ebx)->edtb.topcp;
		ecx = ((EDIT *)ebx)->edtb.topln;
		while(eax<cp)
		{
			edx = ecx*4;
			if(edx>=((EDIT *)ebx)->rpLineFree)
			{
				break;
			}
			edx = [esi+ecx*sizeof(LINE)].LINE.rpChars;
			eax += ((CHARS *)(edi+edx))->len;
			ecx++;
		} // endw
		if(eax>cp)
		{
			ecx--;
		} // endif
		eax = ecx;
		eax *= 4;
		if(eax>=((EDIT *)ebx)->rpLineFree)
		{
			ecx = ((EDIT *)ebx)->rpLineFree;
			ecx /= 4;
			ecx--;
		} // endif
	}
	else
	{
		eax = ((EDIT *)ebx)->edtb.topcp;
		ecx = ((EDIT *)ebx)->edtb.topln;
		while(eax>cp)
		{
			ecx--;
			edx = [esi+ecx*sizeof(LINE)].LINE.rpChars;
			eax -= ((CHARS *)(edi+edx))->len;
		} // endw
	} // endif
	eax = ecx;
	return eax;

} // GetLineFromCp

REG_T GetYpFromLine(DWORD hMem, DWORD nLine)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	esi = ((EDIT *)ebx)->hLine;
	edi = ((EDIT *)ebx)->hChars;
	ecx = nLine;
	ecx *= 4;
	if(ecx>=((EDIT *)ebx)->rpLineFree)
	{
		ecx = ((EDIT *)ebx)->rpLineFree;
		ecx -= sizeof(LINE);
	} // endif
	ecx /= 4;
	nLine = ecx;
	if(ecx>=((EDIT *)ebx)->edtb.topln)
	{
		ecx = ((EDIT *)ebx)->edtb.topln;
		eax = ((EDIT *)ebx)->edtb.topyp;
		edx = ((EDIT *)ebx)->fntinfo.fntht;
		ebx = edx;
		while(ecx<nLine)
		{
			edx = [esi+ecx*sizeof(LINE)].LINE.rpChars;
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
		ecx = ((EDIT *)ebx)->edtb.topln;
		eax = ((EDIT *)ebx)->edtb.topyp;
		edx = ((EDIT *)ebx)->fntinfo.fntht;
		ebx = edx;
		while(ecx>nLine)
		{
			ecx--;
			edx = [esi+ecx*sizeof(LINE)].LINE.rpChars;
			if(((CHARS *)(edi+edx))->state&STATE_HIDDEN)
			{
				goto anon_5;
			} // endif
			eax -= ebx;
anon_5:
		} // endw
	} // endif
	return eax;

} // GetYpFromLine

REG_T GetLineFromYp(DWORD hMem, DWORD y)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD maxln;

	ebx = hMem;
	eax = ((EDIT *)ebx)->rpLineFree;
	eax /= 4;
	eax--;
	maxln = eax;
	esi = ((EDIT *)ebx)->hLine;
	edi = ((EDIT *)ebx)->hChars;
	eax = y;
	ecx = ((EDIT *)ebx)->fntinfo.fntht;
	// xor		edx,edx
	eax /= ecx;
	eax *= ecx;
	y = eax;
	if(eax>=((EDIT *)ebx)->edtb.topyp)
	{
		eax = ((EDIT *)ebx)->edtb.topyp;
		ecx = ((EDIT *)ebx)->edtb.topln;
		edx = ((EDIT *)ebx)->fntinfo.fntht;
		ebx = edx;
		while(eax<y)
		{
			ecx++;
			if(ecx>=maxln)
			{
				break;
			} // endif
			edx = [esi+ecx*sizeof(LINE)].LINE.rpChars;
			if(((CHARS *)(edi+edx))->state&STATE_HIDDEN)
			{
				goto anon_6;
			} // endif
			eax += ebx;
anon_6:
		} // endw
	}
	else
	{
		eax = ((EDIT *)ebx)->edtb.topyp;
		ecx = ((EDIT *)ebx)->edtb.topln;
		edx = ((EDIT *)ebx)->fntinfo.fntht;
		ebx = edx;
		while(eax>y)
		{
			ecx--;
			edx = [esi+ecx*sizeof(LINE)].LINE.rpChars;
			if(((CHARS *)(edi+edx))->state&STATE_HIDDEN)
			{
				goto anon_7;
			} // endif
			eax -= ebx;
anon_7:
		} // endw
	} // endif
	eax = ecx;
	return eax;

} // GetLineFromYp

REG_T GetCpFromXp(DWORD hMem, DWORD lpChars, DWORD x, DWORD fNoAdjust)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD hDC;
	RECT rect;
	DWORD lastright;

	auto void TestIt(void);

	ebx = hMem;
	eax = GetDC(((EDIT *)ebx)->hwnd);
	hDC = eax;
	eax = SelectObject(hDC, ((EDIT *)ebx)->fnt.hFont);
	temp1 = eax;
	eax = ((EDIT *)ebx)->cpx;
	eax = -eax;
	eax += ((EDIT *)ebx)->linenrwt;
	eax += ((EDIT *)ebx)->selbarwt;
	x -= eax;
	eax = 0;
	rect.left = eax;
	rect.right = eax;
	rect.top = eax;
	rect.bottom = eax;
	lastright = eax;
	esi = lpChars;
	edi = ((CHARS *)esi)->len;
	edi /= 1;
	edx = edi;
	ecx = 0;
anon_8:
	temp2 = ecx;
	temp3 = edx;
	TestIt();
	edx = temp3;
	ecx = temp2;
	edx /= 1;
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
			edx /= 1;
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
	eax = ReleaseDC(((EDIT *)ebx)->hwnd, hDC);
	eax = edi;
	return eax;

	void TestIt(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		eax = GetTextWidth(ebx, hDC, esi+sizeof(CHARS), edi, &rect);
		eax = rect.right;
		return;

	}

} // GetCpFromXp

REG_T GetPosFromChar(DWORD hMem, DWORD cp, DWORD lpPoint)
{
	REG_T eax = 0, ecx, edx, ebx, esi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	HDC hDC;
	RECT rect;
	DWORD ln;
	DWORD y;

	ebx = hMem;
	eax = GetLineFromCp(ebx, cp);
	ln = eax;
	eax = GetYpFromLine(ebx, ln);
	y = eax;
	eax = GetCpFromLine(ebx, ln);
	cp -= eax;
	esi = ln;
	esi *= 4;
	esi += ((EDIT *)ebx)->hLine;
	esi = ((LINE *)esi)->rpChars;
	esi += ((EDIT *)ebx)->hChars;
	eax = GetDC(((EDIT *)ebx)->hwnd);
	hDC = eax;
	eax = SelectObject(hDC, ((EDIT *)ebx)->fnt.hFont);
	temp1 = eax;
	rect.left = 0;
	rect.right = 0;
	rect.top = 0;
	rect.bottom = 0;
	eax = GetTextWidth(ebx, hDC, esi+sizeof(CHARS), cp, &rect);
	eax = temp1;
	eax = SelectObject(hDC, eax);
	eax = ReleaseDC(((EDIT *)ebx)->hwnd, hDC);
	esi = lpPoint;
	eax = rect.right;
	eax += ((EDIT *)ebx)->linenrwt;
	eax += ((EDIT *)ebx)->selbarwt;
	eax++;
	((POINT *)esi)->x = eax;
	eax = y;
	((POINT *)esi)->y = eax;
	return eax;

} // GetPosFromChar

REG_T GetCharFromPos(DWORD hMem, DWORD cpy, DWORD x, DWORD y)
{
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD cp;

	ebx = hMem;
	eax = y;
	eax += cpy;
	if(R_SIGNED(eax) < 0)
	{
		eax = 0;
	}
	else
	{
		eax = GetLineFromYp(ebx, eax);
	} // endif
	temp1 = eax;
	eax = GetCpFromLine(ebx, eax);
	cp = eax;
	((EDIT *)ebx)->cpLine = eax;
	edx = temp1;
	edx *= 4;
	if(edx>=((EDIT *)ebx)->rpLineFree)
	{
		edx = ((EDIT *)ebx)->rpLineFree;
		edx -= sizeof(LINE);
	} // endif
	eax = edx;
	eax /= 4;
	((EDIT *)ebx)->line = eax;
	((EDIT *)ebx)->rpLine = edx;
	edx += ((EDIT *)ebx)->hLine;
	edx = ((LINE *)edx)->rpChars;
	((EDIT *)ebx)->rpChars = edx;
	edx += ((EDIT *)ebx)->hChars;
	eax = x;
	eax -= ((EDIT *)ebx)->cpx;
	if(eax==0)
	{
		goto Ex;
	} // endif
	eax = ((EDIT *)ebx)->nMode;
	eax &= MODE_BLOCK;
	eax = GetCpFromXp(ebx, edx, x, eax);
	cp += eax;
Ex:
	eax = cp;
	return eax;

} // GetCharFromPos

REG_T GetCaretPoint(DWORD hMem, DWORD cp, DWORD cpy, DWORD lpPoint)
{
	REG_T eax = 0, ecx, edx, ebx, esi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	POINT pt;

	ebx = hMem;
	eax = GetPosFromChar(ebx, cp, &pt);
	esi = lpPoint;
	if(((EDIT *)ebx)->nMode&MODE_BLOCK)
	{
		eax = ((EDIT *)ebx)->blrg.clMin;
		ecx = ((EDIT *)ebx)->fntinfo.fntwt;
		eax *= ecx;
		eax += ((EDIT *)ebx)->linenrwt;
		eax += ((EDIT *)ebx)->selbarwt;
	}
	else
	{
		eax = pt.x;
	} // endif
	eax -= ((EDIT *)ebx)->cpx;
	((POINT *)esi)->x = eax;
	eax = pt.y;
	eax -= cpy;
	((POINT *)esi)->y = eax;
	return eax;

} // GetCaretPoint

REG_T SetCaret(DWORD hMem, DWORD cpy)
{
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	POINT pt;

	ebx = hMem;
	eax = GetFocus();
	if(eax==((EDIT *)ebx)->focus)
	{
		eax = GetCaretPoint(ebx, ((EDIT *)ebx)->cpMin, cpy, &pt);
		eax = SetCaretPos(pt.x, pt.y);
		eax = ((EDIT *)ebx)->selbarwt;
		eax += ((EDIT *)ebx)->linenrwt;
		ecx = pt.x;
		ecx++;
		edx = ((EDIT *)ebx)->cpMax;
		edx -= ((EDIT *)ebx)->cpMin;
		if(R_SIGNED(eax) <= (SDWORD)pt.x && R_SIGNED(ecx) < (SDWORD)((EDIT *)ebx)->edtb.rc.right && !edx)
		{
			eax = ShowCaret(((EDIT *)ebx)->focus);
			eax = ShowCaret(((EDIT *)ebx)->focus);
			((EDIT *)ebx)->fCaretHide = FALSE;
		}
		else if(!((EDIT *)ebx)->fCaretHide)
		{
			eax = HideCaret(((EDIT *)ebx)->focus);
			((EDIT *)ebx)->fCaretHide = TRUE;
		} // endif
	} // endif
	return eax;

} // SetCaret

REG_T ScrollEdit(DWORD hMem, DWORD hWin, DWORD x, DWORD y)
{
	REG_T eax = 0, ecx, edx, ebx, esi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	eax = hWin;
	if(eax==((EDIT *)ebx)->edta.hwnd)
	{
		esi = &((EDIT *)ebx)->edta;
	}
	else
	{
		esi = &((EDIT *)ebx)->edtb;
	} // endif
	if(((RAEDT *)esi)->rc.bottom)
	{
		if(x || y)
		{
			if(x)
			{
				temp1 = ((RAEDT *)esi)->rc.left;
				eax = ((EDIT *)ebx)->selbarwt;
				eax += ((EDIT *)ebx)->linenrwt;
				((RAEDT *)esi)->rc.left += eax;
				eax = ScrollWindow(hWin, x, 0, & ((RAEDT *)esi)->rc, & ((RAEDT *)esi)->rc);
				((RAEDT *)esi)->rc.left = temp1;
			} // endif
			if(y)
			{
				eax = GetTopFromYp(ebx, ((RAEDT *)esi)->hwnd, ((RAEDT *)esi)->cpy);
				eax = y;
				if(R_SIGNED(eax) < 0)
				{
					eax = -eax;
				} // endif
				eax += ((EDIT *)ebx)->fntinfo.fntht;
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
			if(eax==((EDIT *)ebx)->focus)
			{
				eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
			} // endif
		} // endif
	} // endif
	return eax;

} // ScrollEdit

REG_T InvalidateEdit(DWORD hMem, DWORD hWin)
{
	REG_T eax = 0, ecx, edx, ebx, esi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	eax = hWin;
	if(eax==((EDIT *)ebx)->edta.hwnd)
	{
		esi = &((EDIT *)ebx)->edta;
	}
	else
	{
		esi = &((EDIT *)ebx)->edtb;
	} // endif
	if(((RAEDT *)esi)->rc.bottom)
	{
		eax = GetTopFromYp(ebx, ((RAEDT *)esi)->hwnd, ((RAEDT *)esi)->cpy);
		eax = InvalidateRect(((RAEDT *)esi)->hwnd, NULL, FALSE);
		// invoke UpdateWindow,((RAEDT *)esi)->hwnd
	} // endif
	return eax;

} // InvalidateEdit

REG_T InvalidateLine(DWORD hMem, DWORD hWin, DWORD nLine)
{
	REG_T eax = 0, ecx, edx, ebx, esi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	RECT rect;

	ebx = hMem;
	eax = hWin;
	if(eax==((EDIT *)ebx)->edta.hwnd)
	{
		esi = &((EDIT *)ebx)->edta;
	}
	else
	{
		esi = &((EDIT *)ebx)->edtb;
	} // endif
	if(((RAEDT *)esi)->rc.bottom)
	{
		eax = GetYpFromLine(ebx, nLine);
		eax -= ((RAEDT *)esi)->cpy;
		ecx = eax;
		ecx += ((EDIT *)ebx)->fntinfo.fntht;
		if(R_SIGNED(ecx) > 0 && R_SIGNED(eax) < (SDWORD)((RAEDT *)esi)->rc.bottom)
		{
			rect.top = eax;
			eax += ((EDIT *)ebx)->fntinfo.fntht;
			rect.bottom = eax;
			rect.left = 0;
			eax = ((RAEDT *)esi)->rc.right;
			rect.right = eax;
			eax = InvalidateRect(hWin, &rect, FALSE);
		} // endif
	} // endif
	return eax;

} // InvalidateLine

REG_T InvalidateSelection(DWORD hMem, DWORD hWin, DWORD cpMin, DWORD cpMax)
{
	REG_T eax = 0, ecx, edx, ebx, esi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD nLine;

	ebx = hMem;
	eax = hWin;
	if(eax==((EDIT *)ebx)->edta.hwnd)
	{
		esi = &((EDIT *)ebx)->edta;
	}
	else
	{
		esi = &((EDIT *)ebx)->edtb;
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
			eax = GetCharPtr(ebx, cpMin);
			nLine = edx;
			eax = ((EDIT *)ebx)->cpLine;
anon_9:
			cpMin = eax;
			eax = InvalidateLine(ebx, hWin, nLine);
			nLine++;
			eax = GetCpFromLine(ebx, nLine);
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

REG_T SetCaretVisible(DWORD hWin, DWORD cpy)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	POINT pt;
	DWORD cpx;
	DWORD ht;
	DWORD fExpand;

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
	eax = ((RAEDT *)esi)->rc.bottom;
	if(R_SIGNED(eax) > 0)
	{
		fExpand = 0;
		ecx = ((EDIT *)ebx)->line;
		ecx++;
anon_10:
		ecx--;
		eax = ((EDIT *)ebx)->hLine;
		edx = [eax+ecx*sizeof(LINE)].LINE.rpChars;
		edx += ((EDIT *)ebx)->hChars;
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
			if(ecx!=((EDIT *)ebx)->line)
			{
				eax = Expand(ebx, ecx);
				fExpand++;
			} // endif
			edi--;
		} // endw
		eax = ((RAEDT *)esi)->rc.bottom;
		ecx = ((EDIT *)ebx)->fntinfo.fntht;
		// xor		edx,edx
		eax /= ecx;
		if(!eax)
		{
			eax++;
		} // endif
		eax *= ecx;
		ht = eax;
		eax = ((EDIT *)ebx)->cpx;
		cpx = eax;
		eax = GetYpFromLine(ebx, ((EDIT *)ebx)->line);
		if(eax<cpy)
		{
			((RAEDT *)esi)->cpy = eax;
		}
		else
		{
			eax += ((EDIT *)ebx)->fntinfo.fntht;
			eax -= cpy;
			if(eax>ht)
			{
				eax -= ht;
				((RAEDT *)esi)->cpy += eax;
			} // endif
		} // endif
		eax = GetCaretPoint(ebx, ((EDIT *)ebx)->cpMin, cpy, &pt);
		edx = ((RAEDT *)esi)->rc.right;
		edx -= 16;
		eax = pt.x;
		ecx = ((EDIT *)ebx)->linenrwt;
		ecx += ((EDIT *)ebx)->selbarwt;
		ecx += 16+1;
		if(R_SIGNED(eax) < R_SIGNED(ecx))
		{
			if(R_SIGNED(eax) > 0)
			{
				ecx = ((EDIT *)ebx)->fntinfo.fntwt;
				ecx <<= 3;
				eax += ecx;
				if(((EDIT *)ebx)->cpx>eax)
				{
					((EDIT *)ebx)->cpx -= eax;
				}
				else
				{
					((EDIT *)ebx)->cpx = 0;
				} // endif
			}
			else
			{
				ecx = ((EDIT *)ebx)->fntinfo.fntwt;
				ecx <<= 3;
				eax -= ecx;
				if(((EDIT *)ebx)->cpx<eax)
				{
					((EDIT *)ebx)->cpx += eax;
				}
				else
				{
					((EDIT *)ebx)->cpx = 0;
				} // endif
			} // endif
		}
		else if(eax>edx)
		{
			ecx = ((EDIT *)ebx)->fntinfo.fntwt;
			ecx <<= 3;
			eax -= edx;
			eax += ecx;
			((EDIT *)ebx)->cpx += eax;
		} // endif
		eax = GetTopFromYp(ebx, hWin, ((RAEDT *)esi)->cpy);
		ecx = cpx;
		ecx -= ((EDIT *)ebx)->cpx;
		edx = cpy;
		edx -= ((RAEDT *)esi)->cpy;
		eax = 0;
		eax--;
		if(fExpand)
		{
			eax = InvalidateEdit(ebx, hWin);
			eax = 0;
		}
		else if(ecx || edx)
		{
			if(ecx)
			{
				temp1 = ecx;
				temp2 = edx;
				eax = ScrollEdit(ebx, ((EDIT *)ebx)->edta.hwnd, ecx, edx);
				edx = temp2;
				ecx = temp1;
				eax = ScrollEdit(ebx, ((EDIT *)ebx)->edtb.hwnd, ecx, edx);
			}
			else
			{
				eax = ScrollEdit(ebx, hWin, ecx, edx);
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

REG_T GetBlockCp(DWORD hMem, DWORD nLine, DWORD nPos)
{
	REG_T eax = 0, ecx, edx, ebx, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	eax = GetCpFromLine(ebx, nLine);
	edi = eax;
	eax = nPos;
	ecx = ((EDIT *)ebx)->fntinfo.fntwt;
	eax *= ecx;
	eax += ((EDIT *)ebx)->linenrwt;
	eax += ((EDIT *)ebx)->selbarwt;
	eax -= ((EDIT *)ebx)->cpx;
	edx = nLine;
	edx *= 4;
	if(edx>=((EDIT *)ebx)->rpLineFree)
	{
		edx = ((EDIT *)ebx)->rpLineFree;
		edx -= sizeof(LINE);
	} // endif
	edx += ((EDIT *)ebx)->hLine;
	edx = ((LINE *)edx)->rpChars;
	edx += ((EDIT *)ebx)->hChars;
	eax = GetCpFromXp(ebx, edx, eax, TRUE);
	eax += edi;
	return eax;

} // GetBlockCp

REG_T SetCpxMax(DWORD hMem, DWORD hWin)
{
	REG_T eax = 0, ecx, edx, ebx, esi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	POINT pt;

	ebx = hMem;
	eax = hWin;
	if(eax==((EDIT *)ebx)->edta.hwnd)
	{
		esi = &((EDIT *)ebx)->edta;
	}
	else
	{
		esi = &((EDIT *)ebx)->edtb;
	} // endif
	eax = GetCaretPos(&pt);
	eax = pt.x;
	eax -= ((EDIT *)ebx)->selbarwt;
	eax -= ((EDIT *)ebx)->linenrwt;
	eax += ((EDIT *)ebx)->cpx;
	((RAEDT *)esi)->cpxmax = eax;
	return eax;

} // SetCpxMax

REG_T SetBlockFromCp(DWORD hMem, DWORD cp, DWORD fShift)
{
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	POINT pt;

	ebx = hMem;
	eax = cp;
	((EDIT *)ebx)->cpMin = eax;
	((EDIT *)ebx)->cpMax = eax;
	eax = GetCharPtr(ebx, cp);
	eax = GetPosFromChar(ebx, cp, &pt);
	eax = pt.x;
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
	return eax;

} // SetBlockFromCp

