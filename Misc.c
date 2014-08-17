#include <windows.h>
#include "Data.h"

REG_T SetFont(DWORD hMem, DWORD lpRafont)
{
	REG_T eax = 0, ecx, edx, ebx, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	HDC hDC;
	DRAWTEXTPARAMS dtp;
	RECT rect;
	POINT pt;
	TEXTMETRIC tm;

	ebx = hMem;
	edx = lpRafont;
	eax = ((RAFONT *)edx)->hFont;
	((EDIT *)ebx)->fnt.hFont = eax;
	eax = ((RAFONT *)edx)->hIFont;
	((EDIT *)ebx)->fnt.hIFont = eax;
	eax = ((RAFONT *)edx)->hLnrFont;
	((EDIT *)ebx)->fnt.hLnrFont = eax;
	eax = GetDC(((EDIT *)ebx)->hwnd);
	hDC = eax;
	eax = SelectObject(hDC, ((EDIT *)ebx)->fnt.hFont);
	temp1 = eax;
	// Get height & width
	eax = GetTextExtentPoint32(hDC, &szX, 1, &pt);
	eax = pt.x;
	((EDIT *)ebx)->fntinfo.fntwt = eax;
	eax = pt.y;
	eax += ((EDIT *)ebx)->fntinfo.linespace;
	((EDIT *)ebx)->fntinfo.fntht = eax;
	// Test if monospaced font
	eax = GetTextExtentPoint32(hDC, &szW, 1, &pt);
	temp2 = pt.x;
	eax = GetTextExtentPoint32(hDC, &szI, 1, &pt);
	eax = temp2;
	if(eax==pt.x)
	{
		((EDIT *)ebx)->fntinfo.monospace = TRUE;
	}
	else
	{
		((EDIT *)ebx)->fntinfo.monospace = FALSE;
	} // endif
	// Get space width
	eax = GetTextExtentPoint32(hDC, &szSpace, 1, &pt);
	eax = pt.x;
	((EDIT *)ebx)->fntinfo.spcwt = eax;
	// Get tab width
	dtp.cbSize = sizeof(dtp);
	eax = ((EDIT *)ebx)->nTab;
	dtp.iTabLength = eax;
	dtp.iLeftMargin = 0;
	dtp.iRightMargin = 0;
	dtp.uiLengthDrawn = 0;
	rect.left = 0;
	rect.top = 0;
	rect.right = 0;
	rect.bottom = 0;
	eax = DrawTextEx(hDC, &szTab, 1, &rect, DT_EDITCONTROL | DT_CALCRECT | DT_SINGLELINE | DT_NOPREFIX | DT_EXPANDTABS | DT_TABSTOP, &dtp);
	eax = rect.right;
	((EDIT *)ebx)->fntinfo.tabwt = eax;
	// Check if DBCS
	eax = GetTextMetrics(hDC, &tm);
	eax = (BYTE)tm.tmCharSet;
	((EDIT *)ebx)->fntinfo.charset = eax;
	// SHIFTJIS_CHARSET		equ 128
	// HANGEUL_CHARSET		equ 129
	// GB2312_CHARSET			equ 134
	// CHINESEBIG5_CHARSET	equ 136
	((EDIT *)ebx)->fntinfo.fDBCS = 0;
	if(eax==134 || eax==136 || eax==128 || eax==129)
	{
		((EDIT *)ebx)->fntinfo.fDBCS = eax;
	} // endif
	// Check if italic has same height
	eax = SelectObject(hDC, ((EDIT *)ebx)->fnt.hIFont);
	eax = GetTextExtentPoint32(hDC, &szX, 1, &pt);
	eax = pt.y;
	eax += ((EDIT *)ebx)->fntinfo.linespace;
	eax -= ((EDIT *)ebx)->fntinfo.fntht;
	((EDIT *)ebx)->fntinfo.italic = eax;
	eax = temp1;
	eax = SelectObject(hDC, eax);
	eax = ReleaseDC(0, hDC);
	return eax;

} // SetFont

REG_T SetColor(DWORD hMem, DWORD lpRAColor)
{
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	edx = lpRAColor;
	eax = ((RACOLOR *)edx)->bckcol;
	((EDIT *)ebx)->clr.bckcol = eax;
	eax = ((RACOLOR *)edx)->txtcol;
	((EDIT *)ebx)->clr.txtcol = eax;
	eax = ((RACOLOR *)edx)->selbckcol;
	((EDIT *)ebx)->clr.selbckcol = eax;
	eax = ((RACOLOR *)edx)->seltxtcol;
	((EDIT *)ebx)->clr.seltxtcol = eax;
	eax = ((RACOLOR *)edx)->cmntcol;
	((EDIT *)ebx)->clr.cmntcol = eax;
	eax = ((RACOLOR *)edx)->strcol;
	((EDIT *)ebx)->clr.strcol = eax;
	eax = ((RACOLOR *)edx)->oprcol;
	((EDIT *)ebx)->clr.oprcol = eax;
	eax = ((RACOLOR *)edx)->selbarbck;
	((EDIT *)ebx)->clr.selbarbck = eax;
	eax = ((RACOLOR *)edx)->lnrcol;
	((EDIT *)ebx)->clr.lnrcol = eax;
	return eax;

} // SetColor

REG_T DestroyBrushes(DWORD hMem)
{
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	eax = ((EDIT *)ebx)->br.hBrBck;
	if(eax)
	{
		eax = DeleteObject(eax);
	} // endif
	eax = ((EDIT *)ebx)->br.hBrSelBck;
	if(eax)
	{
		eax = DeleteObject(eax);
	} // endif
	eax = ((EDIT *)ebx)->br.hBrHilite1;
	if(eax)
	{
		eax = DeleteObject(eax);
	} // endif
	eax = ((EDIT *)ebx)->br.hBrHilite2;
	if(eax)
	{
		eax = DeleteObject(eax);
	} // endif
	eax = ((EDIT *)ebx)->br.hBrHilite3;
	if(eax)
	{
		eax = DeleteObject(eax);
	} // endif
	eax = ((EDIT *)ebx)->br.hBrSelBar;
	if(eax)
	{
		eax = DeleteObject(eax);
	} // endif
	eax = ((EDIT *)ebx)->br.hPenSelbar;
	if(eax)
	{
		eax = DeleteObject(eax);
	} // endif
	return eax;

} // DestroyBrushes

REG_T CreateBrushes(DWORD hMem)
{
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	eax = DestroyBrushes(ebx);
	eax = CreateSolidBrush(((EDIT *)ebx)->clr.bckcol);
	((EDIT *)ebx)->br.hBrBck = eax;
	eax = CreateSolidBrush(((EDIT *)ebx)->clr.selbckcol);
	((EDIT *)ebx)->br.hBrSelBck = eax;
	eax = CreateSolidBrush(((EDIT *)ebx)->clr.hicol1);
	((EDIT *)ebx)->br.hBrHilite1 = eax;
	eax = CreateSolidBrush(((EDIT *)ebx)->clr.hicol2);
	((EDIT *)ebx)->br.hBrHilite2 = eax;
	eax = CreateSolidBrush(((EDIT *)ebx)->clr.hicol3);
	((EDIT *)ebx)->br.hBrHilite3 = eax;
	eax = CreateSolidBrush(((EDIT *)ebx)->clr.selbarbck);
	((EDIT *)ebx)->br.hBrSelBar = eax;
	eax = CreatePen(PS_SOLID, 1, ((EDIT *)ebx)->clr.selbarpen);
	((EDIT *)ebx)->br.hPenSelbar = eax;
	return eax;

} // CreateBrushes

REG_T DwToAscii(DWORD dwVal, DWORD lpAscii)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	eax = dwVal;
	edi = lpAscii;
	if(R_SIGNED(eax) >= 0)
	{
		goto pos;
	} // endif
	*(BYTE *)edi = '-';
	eax = -eax;
	edi++;
pos:
	ecx = 429496730;
	esi = edi;
anon_1:
	ebx = eax;
	eax *= ecx;
	eax = edx;
	edx = edx*4+edx;
	edx += edx;
	ebx -= edx;
	RBYTE_LOW(ebx) += '0';
	*(BYTE *)edi = RBYTE_LOW(ebx);
	edi++;
	if(eax!=0)
	{
		goto anon_1;
	} // endif
	*(BYTE *)edi = RBYTE_LOW(eax);
	while(esi<edi)
	{
		edi--;
		RBYTE_LOW(eax) = *(BYTE *)esi;
		RBYTE_HIGH(eax) = *(BYTE *)edi;
		*(BYTE *)edi = RBYTE_LOW(eax);
		*(BYTE *)esi = RBYTE_HIGH(eax);
		esi++;
	} // endw
	return eax;

} // DwToAscii
/*
REG_T strlen(DWORD lpSource)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	eax = lpSource;
	eax -= 4;

anon_2:
	eax += 4;
	edx = *(WORD *)eax;
	if(RBYTE_LOW(edx)==0)
	{
		goto lb1;
	} // endif

	if(RBYTE_HIGH(edx)==0)
	{
		goto lb2;
	} // endif

	edx = *(WORD *)(eax+2);
	if(RBYTE_LOW(edx)==0)
	{
		goto lb3;
	} // endif

	if(RBYTE_HIGH(edx)!=0)
	{
		goto anon_2;
	} // endif

	eax -= lpSource;
	eax += 3;
	return eax;

lb3:
	eax -= lpSource;
	eax += 2;
	return eax;

lb2:
	eax -= lpSource;
	eax += 1;
	return eax;

lb1:
	eax -= lpSource;
	return eax;

} // strlen
*/
REG_T GetChar(DWORD hMem, DWORD cp)
{
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	eax = GetCharPtr(ebx, cp);
	edx *= 4;
	if(edx==((EDIT *)ebx)->rpLineFree)
	{
		eax = 0;
	}
	else
	{
		eax += ((EDIT *)ebx)->rpChars;
		eax += ((EDIT *)ebx)->hChars;
		eax = *(BYTE *)(eax+sizeof(CHARS));
	} // endif
	return eax;

} // GetChar

REG_T IsChar(void)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	eax = RBYTE_LOW(eax);
	eax = eax+CharTab;
	RBYTE_LOW(eax) = *(BYTE *)eax;
	return eax;

} // IsChar

REG_T IsCharLeadByte(DWORD hMem, DWORD cp)
{
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	if(((EDIT *)ebx)->fntinfo.fDBCS)
	{
		eax = GetCharPtr(ebx, cp);
		cp = eax;
		edx = ((EDIT *)ebx)->rpChars;
		edx += ((EDIT *)ebx)->hChars;
		edx += sizeof(CHARS);
		ecx = 0;
		while(ecx<=cp)
		{
			RBYTE_LOW(eax) = *(BYTE *)(edx+ecx);
			if(RBYTE_LOW(eax)>=0x80)
			{
				ecx++;
			} // endif
			ecx++;
		} // endw
		ecx -= cp;
		if(RBYTE_LOW(eax)>0x80 && ecx==2)
		{
			eax = TRUE;
		}
		else
		{
			eax = FALSE;
		} // endif
	}
	else
	{
		eax = FALSE;
	} // endif
	return eax;

} // IsCharLeadByte

REG_T GetTextWidth(DWORD hMem, HDC hDC, DWORD lpText, DWORD nChars, DWORD lpRect)
{
	REG_T eax = 0, ecx, edx, ebx, esi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DRAWTEXTPARAMS dtp;

	ebx = hMem;
	if(((EDIT *)ebx)->fntinfo.monospace)
	{
		eax = ((EDIT *)ebx)->fntinfo.fntht;
		esi = lpRect;
		eax += ((RECT *)esi)->top;
		((RECT *)esi)->bottom = eax;
		esi = lpText;
		ecx = 0;
		eax = 0;
		while(ecx<nChars)
		{
			if(*(BYTE *)(esi+ecx)==VK_TAB)
			{
				eax += ((EDIT *)ebx)->nTab;
				// xor		edx,edx
				eax /= ((EDIT *)ebx)->nTab;
				eax *= ((EDIT *)ebx)->nTab;
			}
			else
			{
				eax++;
			} // endif
			ecx++;
		} // endw
		eax *= ((EDIT *)ebx)->fntinfo.fntwt;
		esi = lpRect;
		eax += ((RECT *)esi)->left;
		((RECT *)esi)->right = eax;
	}
	else
	{
		dtp.cbSize = sizeof(dtp);
		eax = ((EDIT *)ebx)->nTab;
		dtp.iTabLength = eax;
		eax = 0;
		dtp.iLeftMargin = eax;
		dtp.iRightMargin = eax;
		dtp.uiLengthDrawn = eax;
		eax = DrawTextEx(hDC, lpText, nChars, lpRect, DT_EDITCONTROL | DT_CALCRECT | DT_SINGLELINE | DT_NOPREFIX | DT_EXPANDTABS | DT_TABSTOP, &dtp);
		edx = lpRect;
		eax = ((RECT *)edx)->top;
		eax += ((EDIT *)ebx)->fntinfo.fntht;
		((RECT *)edx)->bottom = eax;
	} // endif
	return eax;

} // GetTextWidth

REG_T GetBlockRange(DWORD lpSrc, DWORD lpDst)
{
	REG_T eax = 0, ecx, edx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	esi = lpSrc;
	edi = lpDst;
	eax = ((BLOCKRANGE *)esi)->lnMin;
	edx = ((BLOCKRANGE *)esi)->lnMax;
	if(eax>edx)
	{
		temp1 = eax;
		eax = edx;
		edx = temp1;
	} // endif
	((BLOCKRANGE *)edi)->lnMin = eax;
	((BLOCKRANGE *)edi)->lnMax = edx;
	eax = ((BLOCKRANGE *)esi)->clMin;
	edx = ((BLOCKRANGE *)esi)->clMax;
	if(eax>edx)
	{
		temp1 = eax;
		eax = edx;
		edx = temp1;
	} // endif
	((BLOCKRANGE *)edi)->clMin = eax;
	((BLOCKRANGE *)edi)->clMax = edx;
	return eax;

} // GetBlockRange

REG_T GetBlockRects(DWORD hMem, DWORD lpRects)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	BLOCKRANGE blrg;

	auto void GetRect(void);

	ebx = hMem;
	eax = GetBlockRange(& ((EDIT *)ebx)->blrg, &blrg);
	edi = lpRects;
	esi = &((EDIT *)ebx)->edta;
	GetRect();
	edi += sizeof(RECT);
	esi = &((EDIT *)ebx)->edtb;
	GetRect();
	return eax;

	void GetRect(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		eax = GetYpFromLine(ebx, blrg.lnMin);
		eax -= ((RAEDT *)esi)->cpy;
		((RECT *)edi)->top = eax;
		eax = blrg.lnMax;
		eax++;
		eax = GetYpFromLine(ebx, eax);
		eax -= ((RAEDT *)esi)->cpy;
		((RECT *)edi)->bottom = eax;
		ecx = ((EDIT *)ebx)->fntinfo.fntwt;
		eax = blrg.clMin;
		eax *= ecx;
		((RECT *)edi)->left = eax;
		eax = blrg.clMax;
		eax++;
		eax *= ecx;
		((RECT *)edi)->right = eax;
		eax = ((EDIT *)ebx)->cpx;
		eax = -eax;
		eax += ((EDIT *)ebx)->linenrwt;
		eax += ((EDIT *)ebx)->selbarwt;
		((RECT *)edi)->left += eax;
		((RECT *)edi)->right += eax;
		return;

	}

} // GetBlockRects

REG_T InvalidateBlock(DWORD hMem, DWORD lpOldRects)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	RECT newrects[2];
	RECT rect;
	DWORD wt;
	DWORD ht;

	auto void DoRect(void);

	ebx = hMem;
	eax = ((EDIT *)ebx)->fntinfo.fntwt;
	wt = eax;
	eax = ((EDIT *)ebx)->fntinfo.fntht;
	ht = eax;
	esi = lpOldRects;
	edi = &newrects;
	eax = GetBlockRects(ebx, edi);
	eax = ((EDIT *)ebx)->edta.rc.bottom;
	eax -= ((EDIT *)ebx)->edta.rc.top;
	if(eax)
	{
		eax = ((EDIT *)ebx)->edta.hwnd;
		DoRect();
	} // endif
	esi += sizeof(RECT);
	edi += sizeof(RECT);
	eax = ((EDIT *)ebx)->edtb.hwnd;
	DoRect();
	return eax;

	void DoRect(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		temp1 = ebx;
		ebx = eax;
		// Left part
		eax = ((RECT *)esi)->left;
		edx = ((RECT *)edi)->left;
		if(eax!=edx)
		{
			if(R_SIGNED(eax) > R_SIGNED(edx))
			{
				// Old>New
				rect.right = eax;
				rect.left = edx;
			}
			else
			{
				// Old<New
				rect.right = edx;
				rect.left = eax;
			} // endif
			eax = ((RECT *)esi)->top;
			edx = ((RECT *)edi)->top;
			if(R_SIGNED(eax) > R_SIGNED(edx))
			{
				eax = edx;
			} // endif
			rect.top = eax;
			eax = ((RECT *)esi)->bottom;
			edx = ((RECT *)edi)->bottom;
			if(R_SIGNED(eax) < R_SIGNED(edx))
			{
				eax = edx;
			} // endif
			rect.bottom = eax;
			rect.right++;
			eax = InvalidateRect(ebx, &rect, TRUE);
			eax = UpdateWindow(ebx);
		} // endif
		// Right part
		eax = ((RECT *)esi)->right;
		edx = ((RECT *)edi)->right;
		if(eax!=edx)
		{
			edx -= wt;
			edx++;
			edx++;
			eax -= wt;
			eax++;
			eax++;
			if(R_SIGNED(eax) > R_SIGNED(edx))
			{
				// Old>New
				rect.right = eax;
				rect.left = edx;
			}
			else
			{
				// Old<New
				rect.right = edx;
				rect.left = eax;
			} // endif
			eax = ((RECT *)esi)->top;
			edx = ((RECT *)edi)->top;
			if(R_SIGNED(eax) > R_SIGNED(edx))
			{
				eax = edx;
			} // endif
			rect.top = eax;
			eax = ((RECT *)esi)->bottom;
			edx = ((RECT *)edi)->bottom;
			if(R_SIGNED(eax) < R_SIGNED(edx))
			{
				eax = edx;
			} // endif
			rect.bottom = eax;
			eax = InvalidateRect(ebx, &rect, TRUE);
			eax = UpdateWindow(ebx);
		} // endif
		// Top part
		eax = ((RECT *)esi)->top;
		edx = ((RECT *)edi)->top;
		if(eax!=edx)
		{
			if(R_SIGNED(eax) > R_SIGNED(edx))
			{
				// Old>New
				rect.bottom = eax;
				rect.top = edx;
			}
			else
			{
				// Old<New
				rect.bottom = edx;
				rect.top = eax;
			} // endif
			eax = ((RECT *)esi)->left;
			edx = ((RECT *)edi)->left;
			if(R_SIGNED(eax) > R_SIGNED(edx))
			{
				eax = edx;
			} // endif
			rect.left = eax;
			eax = ((RECT *)esi)->right;
			edx = ((RECT *)edi)->right;
			if(R_SIGNED(eax) < R_SIGNED(edx))
			{
				eax = edx;
			} // endif
			rect.right = eax;
			eax = InvalidateRect(ebx, &rect, TRUE);
			eax = UpdateWindow(ebx);
		} // endif
		// Bottom part
		eax = ((RECT *)esi)->bottom;
		edx = ((RECT *)edi)->bottom;
		if(eax!=edx)
		{
			if(R_SIGNED(eax) > R_SIGNED(edx))
			{
				// Old>New
				rect.top = eax;
				rect.bottom = edx;
			}
			else
			{
				// Old<New
				rect.top = edx;
				rect.bottom = eax;
			} // endif
			eax = ((RECT *)esi)->left;
			edx = ((RECT *)edi)->left;
			if(R_SIGNED(eax) > R_SIGNED(edx))
			{
				eax = edx;
			} // endif
			rect.left = eax;
			eax = ((RECT *)esi)->right;
			edx = ((RECT *)edi)->right;
			if(R_SIGNED(eax) < R_SIGNED(edx))
			{
				eax = edx;
			} // endif
			rect.right = eax;
			eax = InvalidateRect(ebx, &rect, TRUE);
			eax = UpdateWindow(ebx);
		} // endif
		ebx = temp1;
		return;

	}

} // InvalidateBlock

