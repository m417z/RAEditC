#include "Misc.h"

#include "Function.h"
#include "Position.h"

REG_T SetFont(EDIT *pMem, REG_T lpRafont)
{
	REG_T eax = 0, edx, ebx;
	REG_T temp1, temp2;
	HDC hDC;
	DRAWTEXTPARAMS dtp;
	RECT rect;
	POINT pt;
	TEXTMETRIC tm;

	edx = lpRafont;
	eax = ((RAFONT *)edx)->hFont;
	pMem->fnt.hFont = eax;
	eax = ((RAFONT *)edx)->hIFont;
	pMem->fnt.hIFont = eax;
	eax = ((RAFONT *)edx)->hLnrFont;
	pMem->fnt.hLnrFont = eax;
	eax = GetDC(pMem->hwnd);
	hDC = eax;
	eax = SelectObject(hDC, pMem->fnt.hFont);
	temp1 = eax;
	// Get height & width
	eax = GetTextExtentPoint32(hDC, szX, 1, &pt);
	eax = pt.x;
	pMem->fntinfo.fntwt = eax;
	eax = pt.y;
	eax += pMem->fntinfo.linespace;
	pMem->fntinfo.fntht = eax;
	// Test if monospaced font
	eax = GetTextExtentPoint32(hDC, szW, 1, &pt);
	temp2 = pt.x;
	eax = GetTextExtentPoint32(hDC, szI, 1, &pt);
	eax = temp2;
	if(eax==pt.x)
	{
		pMem->fntinfo.monospace = TRUE;
	}
	else
	{
		pMem->fntinfo.monospace = FALSE;
	} // endif
	// Get space width
	eax = GetTextExtentPoint32(hDC, szSpace, 1, &pt);
	eax = pt.x;
	pMem->fntinfo.spcwt = eax;
	// Get tab width
	dtp.cbSize = sizeof(dtp);
	eax = pMem->nTab;
	dtp.iTabLength = eax;
	dtp.iLeftMargin = 0;
	dtp.iRightMargin = 0;
	dtp.uiLengthDrawn = 0;
	rect.left = 0;
	rect.top = 0;
	rect.right = 0;
	rect.bottom = 0;
	eax = DrawTextEx(hDC, szTab, 1, &rect, DT_EDITCONTROL | DT_CALCRECT | DT_SINGLELINE | DT_NOPREFIX | DT_EXPANDTABS | DT_TABSTOP, &dtp);
	eax = rect.right;
	pMem->fntinfo.tabwt = eax;
	// Check if DBCS
	eax = GetTextMetrics(hDC, &tm);
	eax = (BYTE)tm.tmCharSet;
	pMem->fntinfo.charset = eax;
	// SHIFTJIS_CHARSET		equ 128
	// HANGEUL_CHARSET		equ 129
	// GB2312_CHARSET			equ 134
	// CHINESEBIG5_CHARSET	equ 136
	pMem->fntinfo.fDBCS = 0;
	if(eax==134 || eax==136 || eax==128 || eax==129)
	{
		pMem->fntinfo.fDBCS = eax;
	} // endif
	// Check if italic has same height
	eax = SelectObject(hDC, pMem->fnt.hIFont);
	eax = GetTextExtentPoint32(hDC, szX, 1, &pt);
	eax = pt.y;
	eax += pMem->fntinfo.linespace;
	eax -= pMem->fntinfo.fntht;
	pMem->fntinfo.italic = eax;
	eax = temp1;
	eax = SelectObject(hDC, eax);
	eax = ReleaseDC(0, hDC);
	return eax;

} // SetFont

REG_T SetColor(EDIT *pMem, REG_T lpRAColor)
{
	REG_T eax = 0, edx, ebx;

	edx = lpRAColor;
	eax = ((RACOLOR *)edx)->bckcol;
	pMem->clr.bckcol = eax;
	eax = ((RACOLOR *)edx)->txtcol;
	pMem->clr.txtcol = eax;
	eax = ((RACOLOR *)edx)->selbckcol;
	pMem->clr.selbckcol = eax;
	eax = ((RACOLOR *)edx)->seltxtcol;
	pMem->clr.seltxtcol = eax;
	eax = ((RACOLOR *)edx)->cmntcol;
	pMem->clr.cmntcol = eax;
	eax = ((RACOLOR *)edx)->strcol;
	pMem->clr.strcol = eax;
	eax = ((RACOLOR *)edx)->oprcol;
	pMem->clr.oprcol = eax;
	eax = ((RACOLOR *)edx)->selbarbck;
	pMem->clr.selbarbck = eax;
	eax = ((RACOLOR *)edx)->lnrcol;
	pMem->clr.lnrcol = eax;
	return eax;

} // SetColor

REG_T DestroyBrushes(EDIT *pMem)
{
	REG_T eax = 0, ebx;

	eax = pMem->br.hBrBck;
	if(eax)
	{
		eax = DeleteObject(eax);
	} // endif
	eax = pMem->br.hBrSelBck;
	if(eax)
	{
		eax = DeleteObject(eax);
	} // endif
	eax = pMem->br.hBrHilite1;
	if(eax)
	{
		eax = DeleteObject(eax);
	} // endif
	eax = pMem->br.hBrHilite2;
	if(eax)
	{
		eax = DeleteObject(eax);
	} // endif
	eax = pMem->br.hBrHilite3;
	if(eax)
	{
		eax = DeleteObject(eax);
	} // endif
	eax = pMem->br.hBrSelBar;
	if(eax)
	{
		eax = DeleteObject(eax);
	} // endif
	eax = pMem->br.hPenSelbar;
	if(eax)
	{
		eax = DeleteObject(eax);
	} // endif
	return eax;

} // DestroyBrushes

REG_T CreateBrushes(EDIT *pMem)
{
	REG_T eax = 0, ebx;

	eax = DestroyBrushes(pMem);
	eax = CreateSolidBrush(pMem->clr.bckcol);
	pMem->br.hBrBck = eax;
	eax = CreateSolidBrush(pMem->clr.selbckcol);
	pMem->br.hBrSelBck = eax;
	eax = CreateSolidBrush(pMem->clr.hicol1);
	pMem->br.hBrHilite1 = eax;
	eax = CreateSolidBrush(pMem->clr.hicol2);
	pMem->br.hBrHilite2 = eax;
	eax = CreateSolidBrush(pMem->clr.hicol3);
	pMem->br.hBrHilite3 = eax;
	eax = CreateSolidBrush(pMem->clr.selbarbck);
	pMem->br.hBrSelBar = eax;
	eax = CreatePen(PS_SOLID, 1, pMem->clr.selbarpen);
	pMem->br.hPenSelbar = eax;
	return eax;

} // CreateBrushes

REG_T DwToAscii(DWORD dwVal, REG_T lpAscii)
{
	REG_T eax = 0, edx, ebx, esi, edi;

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
	esi = edi;
anon_1:
	ebx = eax;
	edx = eax;
	edx /= 10;
	eax = edx;
	edx *= 10;
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
REG_T strlen(REG_T lpSource)
{
	REG_T eax = 0, edx;

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
REG_T GetChar(EDIT *pMem, DWORD cp)
{
	REG_T eax = 0, ecx, edx, ebx;

	eax = GetCharPtr(pMem, cp, &ecx, &edx);
	edx *= sizeof(LINE);
	if(edx==pMem->rpLineFree)
	{
		eax = 0;
	}
	else
	{
		eax += pMem->rpChars;
		eax += pMem->hChars;
		eax = *(BYTE *)(eax+sizeof(CHARS));
	} // endif
	return eax;

} // GetChar

REG_T IsChar(BYTE ch)
{
	REG_T eax = 0;

	eax = ch;
	eax = eax+CharTab;
	RBYTE_LOW(eax) = *(BYTE *)eax;
	return eax;

} // IsChar

REG_T IsCharLeadByte(EDIT *pMem, DWORD cp)
{
	REG_T eax = 0, ecx, edx, ebx;

	if(pMem->fntinfo.fDBCS)
	{
		eax = GetCharPtr(pMem, cp, &ecx, &edx);
		cp = eax;
		edx = pMem->rpChars;
		edx += pMem->hChars;
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

REG_T GetTextWidth(EDIT *pMem, HDC hDC, REG_T lpText, DWORD nChars, REG_T lpRect)
{
	REG_T eax = 0, ecx, edx, ebx, esi;
	DRAWTEXTPARAMS dtp;

	if(pMem->fntinfo.monospace)
	{
		eax = pMem->fntinfo.fntht;
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
				eax += pMem->nTab;
				eax /= pMem->nTab;
				eax *= pMem->nTab;
			}
			else
			{
				eax++;
			} // endif
			ecx++;
		} // endw
		eax *= pMem->fntinfo.fntwt;
		esi = lpRect;
		eax += ((RECT *)esi)->left;
		((RECT *)esi)->right = eax;
	}
	else
	{
		dtp.cbSize = sizeof(dtp);
		eax = pMem->nTab;
		dtp.iTabLength = eax;
		eax = 0;
		dtp.iLeftMargin = eax;
		dtp.iRightMargin = eax;
		dtp.uiLengthDrawn = eax;
		eax = DrawTextEx(hDC, lpText, nChars, lpRect, DT_EDITCONTROL | DT_CALCRECT | DT_SINGLELINE | DT_NOPREFIX | DT_EXPANDTABS | DT_TABSTOP, &dtp);
		edx = lpRect;
		eax = ((RECT *)edx)->top;
		eax += pMem->fntinfo.fntht;
		((RECT *)edx)->bottom = eax;
	} // endif
	return eax;

} // GetTextWidth

REG_T GetBlockRange(REG_T lpSrc, REG_T lpDst)
{
	REG_T eax = 0, edx, esi, edi;
	REG_T temp1;

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

REG_T GetBlockRects(EDIT *pMem, REG_T lpRects)
{
	REG_T eax = 0, ecx, ebx, esi, edi;
	BLOCKRANGE blrg;

	auto void GetRect(void);

	eax = GetBlockRange(& pMem->blrg, &blrg);
	edi = lpRects;
	esi = &pMem->edta;
	GetRect();
	edi += sizeof(RECT);
	esi = &pMem->edtb;
	GetRect();
	return eax;

	void GetRect(void)
	{
		eax = GetYpFromLine(pMem, blrg.lnMin);
		eax -= ((RAEDT *)esi)->cpy;
		((RECT *)edi)->top = eax;
		eax = blrg.lnMax;
		eax = GetYpFromLine(pMem, eax);
		eax -= ((RAEDT *)esi)->cpy;
		eax += pMem->fntinfo.fntht;
		((RECT *)edi)->bottom = eax;
		ecx = pMem->fntinfo.fntwt;
		eax = blrg.clMin;
		eax *= ecx;
		((RECT *)edi)->left = eax;
		eax = blrg.clMax;
		eax++;
		eax *= ecx;
		((RECT *)edi)->right = eax;
		eax = pMem->cpx;
		eax = -eax;
		eax += pMem->linenrwt;
		eax += pMem->selbarwt;
		((RECT *)edi)->left += eax;
		((RECT *)edi)->right += eax;
		return;

	}

} // GetBlockRects

REG_T InvalidateBlock(EDIT *pMem, REG_T lpOldRects)
{
	REG_T eax = 0, edx, ebx, esi, edi;
	RECT newrects[2];
	RECT rect;
	DWORD wt;
	DWORD ht;

	auto void DoRect(void);

	eax = pMem->fntinfo.fntwt;
	wt = eax;
	eax = pMem->fntinfo.fntht;
	ht = eax;
	esi = lpOldRects;
	edi = &newrects;
	eax = GetBlockRects(pMem, edi);
	eax = pMem->edta.rc.bottom;
	eax -= pMem->edta.rc.top;
	if(eax)
	{
		eax = pMem->edta.hwnd;
		DoRect();
	} // endif
	esi += sizeof(RECT);
	edi += sizeof(RECT);
	eax = pMem->edtb.hwnd;
	DoRect();
	return eax;

	void DoRect(void)
	{
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
		return;

	}

} // InvalidateBlock

