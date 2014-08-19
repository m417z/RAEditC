#include "ClipBoard.h"

#include "Edit.h"
#include "Function.h"
#include "Memory.h"
#include "Misc.h"
#include "Position.h"
#include "Undo.h"

REG_T SetClipData(LPSTR lpData, DWORD dwSize)
{
	REG_T eax = 0;
	HANDLE hMem;
	REG_T pMem;

	eax = dwSize;
	eax >>= 3;
	eax++;
	eax <<= 3;
	eax = xGlobalAlloc(GHND | GMEM_DDESHARE, eax);
	if(eax==0)
	{
		goto exit2;
	} // endif
	hMem = eax;
	eax = GlobalLock(eax); // hGlob
	if(eax==0)
	{
		goto exit1;
	} // endif
	pMem = eax;
	eax = RtlMoveMemory(eax, lpData, dwSize);
	eax = pMem;
	eax += dwSize;
	*(BYTE *)eax = 0;
	eax = GlobalUnlock(hMem);
	eax = OpenClipboard(NULL);
	if(eax)
	{
		eax = EmptyClipboard();
		eax = SetClipboardData(CF_TEXT, hMem);
		eax = CloseClipboard();
		eax = 0; // 0 - Ok
		goto exit3;
	} // endif
exit1:
	eax = GlobalFree(hMem);
	eax = 0;
exit2:
	eax--; // -1 - error
exit3:
	return eax;

} // SetClipData

REG_T EditCopy(EDIT *pMem, REG_T lpCMem)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1;
	DWORD cpMin;
	DWORD cpMax;

	eax = pMem->cpMin;
	edx = pMem->cpMax;
	if(eax>edx)
	{
		temp1 = eax;
		eax = edx;
		edx = temp1;
	} // endif
	cpMin = eax;
	cpMax = edx;
	eax = GetCharPtr(pMem, cpMin, &ecx, &edx);
	ecx = eax;
	edi = lpCMem;
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
		*(BYTE *)edi = RBYTE_LOW(eax);
		edi++;
		if(RBYTE_LOW(eax)==0x0D)
		{
			*(BYTE *)edi = 0x0A;
			edi++;
		} // endif
		eax = temp1;
		if(ecx==((CHARS *)eax)->len)
		{
			ecx = 0;
			esi += sizeof(LINE);
		} // endif
		edx++;
	} // endw
	edi -= lpCMem;
	eax = edi;
	return eax;

} // EditCopy

REG_T EditCopyBlock(EDIT *pMem, REG_T lpCMem)
{
	REG_T eax = 0, edx, ebx, esi;
	BLOCKRANGE blrg;

	auto void CopyBlockChar(void);
	auto void CopyBlockLine(void);

	eax = GetBlockRange(& pMem->blrg, &blrg);
	esi = lpCMem;
	edx = blrg.lnMin;
	while(edx<=blrg.lnMax)
	{
		CopyBlockLine();
		edx++;
	} // endw
	eax = esi;
	eax -= lpCMem;
	return eax;

	void CopyBlockChar(void)
	{
		eax = GetBlockCp(pMem, edx, eax);
		eax = GetChar(pMem, eax);
		if(eax==VK_RETURN || eax==VK_TAB)
		{
			eax = VK_SPACE;
		} // endif
		*(BYTE *)esi = RBYTE_LOW(eax);
		esi++;
		return;

	}

	void CopyBlockLine(void)
	{
		REG_T temp1, temp2;
		eax = blrg.clMin;
		while(eax<blrg.clMax)
		{
			temp1 = eax;
			temp2 = edx;
			CopyBlockChar();
			edx = temp2;
			eax = temp1;
			eax++;
		} // endw
		eax = 0x0A0D;
		*(WORD *)esi = eax;
		esi += 2;
		return;

	}

} // EditCopyBlock

REG_T EditCopyNoLF(EDIT *pMem, REG_T lpCMem)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1;
	DWORD cpMin;
	DWORD cpMax;

	eax = pMem->cpMin;
	edx = pMem->cpMax;
	if(eax>edx)
	{
		temp1 = eax;
		eax = edx;
		edx = temp1;
	} // endif
	cpMin = eax;
	cpMax = edx;
	eax = GetCharPtr(pMem, cpMin, &ecx, &edx);
	ecx = eax;
	edi = lpCMem;
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
		*(BYTE *)edi = RBYTE_LOW(eax);
		edi++;
		eax = temp1;
		if(ecx==((CHARS *)eax)->len)
		{
			ecx = 0;
			esi += sizeof(LINE);
		} // endif
		edx++;
	} // endw
	edi -= lpCMem;
	eax = edi;
	return eax;

} // EditCopyNoLF

REG_T Copy(EDIT *pMem)
{
	REG_T eax = 0, edx, ebx;
	REG_T temp1;
	REG_T hCMem;

	if(!(pMem->nMode&MODE_BLOCK))
	{
		eax = pMem->cpMin;
		eax -= pMem->cpMax;
		if(eax)
		{
			if(R_SIGNED(eax) < 0)
			{
				eax = -eax;
			} // endif
			eax >>= 3;
			eax++;
			eax <<= 4;
			eax = xGlobalAlloc(GMEM_ZEROINIT, eax);
			hCMem = eax;
			eax = GlobalLock(hCMem);
			temp1 = eax;
			eax = EditCopy(pMem, eax);
			edx = temp1;
			eax = SetClipData(edx, eax);
			eax = GlobalUnlock(hCMem);
			eax = GlobalFree(hCMem);
		} // endif
	}
	else
	{
		eax = pMem->blrg.clMin;
		if(eax!=pMem->blrg.clMax)
		{
			eax = xGlobalAlloc(GMEM_ZEROINIT, 256*1024);
			hCMem = eax;
			eax = GlobalLock(hCMem);
			temp1 = eax;
			eax = EditCopyBlock(pMem, eax);
			edx = temp1;
			eax = SetClipData(edx, eax);
			eax = GlobalUnlock(hCMem);
			eax = GlobalFree(hCMem);
		} // endif
	} // endif
	return eax;

} // Copy

REG_T EditPaste(EDIT *pMem, REG_T hData)
{
	REG_T eax = 0, ecx, edx, ebx;

	auto void InsertMem(void);

	eax = hData;
	if(eax)
	{
		InsertMem();
	}
	else
	{
		eax = OpenClipboard(pMem->hwnd);
		if(eax)
		{
			eax = GetClipboardData(CF_TEXT);
			if(eax)
			{
				InsertMem();
			} // endif
			eax = CloseClipboard();
		} // endif
	} // endif
	return eax;

	void InsertMem(void)
	{
		REG_T temp1, temp2, temp3, temp4;
		temp1 = eax;
		eax = GlobalLock(eax);
		temp2 = pMem->fOvr;
		pMem->fOvr = FALSE;
		temp3 = pMem->cpMin;
		temp4 = eax;
		eax = EditInsert(pMem, pMem->cpMin, eax);
		pMem->cpMin += eax;
		pMem->cpMax += eax;
		edx = temp4;
		ecx = temp3;
		eax = SaveUndo(pMem, UNDO_INSERTBLOCK, ecx, edx, eax);
		pMem->fOvr = temp2;
		eax = temp1;
		eax = GlobalUnlock(eax);
		return;

	}

} // EditPaste

REG_T EditPasteBlock(EDIT *pMem, REG_T hData)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	DWORD nSpc;
	BLOCKRANGE blrg;

	auto void InsertMem(void);

	eax = GetBlockRange(& pMem->blrg, &blrg);
	eax = hData;
	if(eax)
	{
		InsertMem();
	}
	else
	{
		eax = OpenClipboard(pMem->hwnd);
		if(eax)
		{
			eax = GetClipboardData(CF_TEXT);
			if(eax)
			{
				InsertMem();
			} // endif
			eax = CloseClipboard();
			eax = blrg.lnMin;
			pMem->blrg.lnMin = eax;
			pMem->blrg.lnMax = eax;
			eax = blrg.clMin;
			pMem->blrg.clMin = eax;
			pMem->blrg.clMax = eax;
		} // endif
	} // endif
	return eax;

	void InsertMem(void)
	{
		REG_T temp1, temp2, temp3;
		temp1 = eax;
		eax = GlobalLock(eax);
		esi = eax;
		temp2 = pMem->fOvr;
		pMem->fOvr = FALSE;
		nSpc = 0;
anon_1:
		eax = GetBlockCp(pMem, blrg.lnMin, blrg.clMin);
		edi = eax;
		eax = GetChar(pMem, edi);
		if(!eax)
		{
			eax = InsertChar(pMem, edi, VK_RETURN);
		} // endif
		if(blrg.clMin)
		{
			eax = GetBlockCp(pMem, blrg.lnMin, blrg.clMin);
			temp3 = eax;
			eax = blrg.clMin;
			eax--;
			eax = GetBlockCp(pMem, blrg.lnMin, eax);
			edx = temp3;
			if(eax==edx)
			{
				temp3 = edx;
				eax = GetCharPtr(pMem, edx, &ecx, &edx);
				edx = temp3;
				eax = InsertChar(pMem, edx, VK_SPACE);
				nSpc++;
				goto anon_1;
			} // endif
		} // endif
		eax = GetBlockCp(pMem, blrg.lnMin, blrg.clMin);
		edi = eax;
		temp3 = edi;
		while(*(BYTE *)esi!=VK_RETURN && *(BYTE *)esi)
		{
			eax = *(BYTE *)esi;
			eax = InsertChar(pMem, edi, eax);
			edi++;
			esi++;
		} // endw
		pMem->cpMin = edi;
		pMem->cpMax = edi;
		if(*(BYTE *)esi==VK_RETURN)
		{
			esi++;
		} // endif
		if(*(BYTE *)esi==0x0A)
		{
			esi++;
		} // endif
		eax = temp3;
		temp3 = eax;
		eax = GetCharPtr(pMem, eax, &ecx, &edx);
		ecx = temp3;
		edi -= ecx;
		edx = pMem->rpChars;
		edx += pMem->hChars;
		edx += sizeof(CHARS);
		edx += eax;
		edx -= nSpc;
		ecx -= nSpc;
		eax = edi;
		eax += nSpc;
		eax = SaveUndo(pMem, UNDO_INSERTBLOCK, ecx, edx, eax);
		if(*(BYTE *)esi)
		{
			blrg.lnMin++;
			nSpc = 0;
			goto anon_1;
		} // endif
		blrg.clMin += edi;
		eax = blrg.clMin;
		blrg.clMax = eax;
		eax = blrg.lnMin;
		blrg.lnMax = eax;
		pMem->fOvr = temp2;
		eax = temp1;
		eax = GlobalUnlock(eax);
		return;

	}

} // EditPasteBlock

REG_T Paste(EDIT *pMem, HWND hWin, REG_T hData)
{
	REG_T eax = 0, ecx, edx, ebx, esi;
	BLOCKRANGE blrg;

	eax = hWin;
	if(eax==pMem->edta.hwnd)
	{
		esi = &pMem->edta;
	}
	else
	{
		esi = &pMem->edtb;
	} // endif
	if(!(pMem->nMode&MODE_BLOCK))
	{
		eax = DeleteSelection(pMem, pMem->cpMin, pMem->cpMax);
		pMem->cpMin = eax;
		pMem->cpMax = eax;
		eax = EditPaste(pMem, hData);
	}
	else
	{
		eax = GetBlockRange(& pMem->blrg, &blrg);
		eax = DeleteSelectionBlock(pMem, blrg.lnMin, blrg.clMin, blrg.lnMax, blrg.clMax);
		eax = GetBlockCp(pMem, blrg.lnMin, blrg.clMin);
		pMem->cpMin = eax;
		pMem->cpMax = eax;
		eax = EditPasteBlock(pMem, hData);
	} // endif
	eax = GetCharPtr(pMem, pMem->cpMin, &ecx, &edx);
	eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
	eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
	eax = InvalidateEdit(pMem, pMem->edta.hwnd);
	eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
	eax = SetCpxMax(pMem, hWin);
	eax = SelChange(pMem, SEL_TEXT);
	return eax;

} // Paste

REG_T Cut(EDIT *pMem, HWND hWin)
{
	REG_T eax = 0, ecx, edx, ebx, esi;

	eax = hWin;
	if(eax==pMem->edta.hwnd)
	{
		esi = &pMem->edta;
	}
	else
	{
		esi = &pMem->edtb;
	} // endif
	if(!(pMem->nMode&MODE_BLOCK))
	{
		eax = Copy(pMem);
		eax = DeleteSelection(pMem, pMem->cpMin, pMem->cpMax);
		pMem->cpMin = eax;
		pMem->cpMax = eax;
	}
	else
	{
		eax = GetBlockRange(& pMem->blrg, & pMem->blrg);
		eax = Copy(pMem);
		eax = DeleteSelectionBlock(pMem, pMem->blrg.lnMin, pMem->blrg.clMin, pMem->blrg.lnMax, pMem->blrg.clMax);
		eax = pMem->blrg.clMin;
		edx = pMem->blrg.lnMin;
		pMem->blrg.clMax = eax;
		pMem->blrg.lnMax = edx;
		eax = GetBlockCp(pMem, edx, eax);
		pMem->cpMin = eax;
		pMem->cpMax = eax;
	} // endif
	eax = GetCharPtr(pMem, pMem->cpMin, &ecx, &edx);
	eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
	eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
	eax = InvalidateEdit(pMem, pMem->edta.hwnd);
	eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
	eax = SetCpxMax(pMem, hWin);
	eax = SelChange(pMem, SEL_TEXT);
	return eax;

} // Cut

REG_T ConvertCase(EDIT *pMem, DWORD nFunction)
{
	REG_T eax = 0, edx, ebx, edi;
	REG_T temp1;

	nUndoid++;
	eax = pMem->cpMin;
	edx = pMem->cpMax;
	if(eax<edx)
	{
		temp1 = eax;
		eax = edx;
		edx = temp1;
	} // endif
	eax -= edx;
	eax++;
	eax = xGlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, eax);
	edi = eax;
	eax = EditCopyNoLF(pMem, edi);
	if(nFunction==CONVERT_UPPERCASE)
	{
		eax = CharUpper(edi);
	}
	else if(nFunction==CONVERT_LOWERCASE)
	{
		eax = CharLower(edi);
	} // endif
	eax = Paste(pMem, pMem->focus, edi);
	eax = GlobalFree(edi);
	nUndoid++;
	return eax;

} // ConvertCase

REG_T ConvertIndent(EDIT *pMem, DWORD nFunction)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1;
	REG_T hCMem;
	REG_T hLMem;
	DWORD cpst;
	DWORD cpen;
	DWORD cpMin;
	DWORD cpMax;
	DWORD nxt;
	DWORD len;
	DWORD spcount;

	auto void NextLine(void);
	auto void GetIndent(void);

	nUndoid++;
	edx = pMem->cpMin;
	eax = pMem->cpMax;
	if(eax<edx)
	{
		temp1 = eax;
		eax = edx;
		edx = temp1;
	} // endif
	temp1 = eax;
	eax = GetLineFromCp(pMem, edx);
	eax = GetCpFromLine(pMem, eax);
	edx = eax;
	eax = temp1;
	cpst = edx;
	cpMin = edx;
	pMem->cpMin = edx;
	cpen = eax;
	cpMax = eax;
	pMem->cpMax = eax;
	eax -= edx;
	eax++;
	eax = xGlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, eax);
	edi = eax;
	hCMem = eax;
	eax = xGlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, 1024*32);
	hLMem = eax;
	eax = EditCopyNoLF(pMem, edi);
	while(*(BYTE *)edi)
	{
		GetIndent();
		if(edx)
		{
			len = edx;
			esi = hLMem;
			ecx = 0;
			edx = 0;
			if(nFunction==CONVERT_TABTOSPACE)
			{
				eax = pMem->nTab;
				nxt = eax;
				while(edx<len)
				{
					RBYTE_LOW(eax) = *(BYTE *)(edi+edx);
					if(RBYTE_LOW(eax)==VK_TAB)
					{
						while(ecx<nxt)
						{
							*(BYTE *)(esi+ecx) = VK_SPACE;
							ecx++;
						} // endw
					}
					else if(RBYTE_LOW(eax)==VK_SPACE)
					{
						*(BYTE *)(esi+ecx) = VK_SPACE;
						ecx++;
					} // endif
					if(ecx==nxt)
					{
						eax = pMem->nTab;
						nxt += eax;
					} // endif
					edx++;
				} // endw
			}
			else if(nFunction==CONVERT_SPACETOTAB)
			{
				eax = pMem->nTab;
				nxt = eax;
				spcount = edx;
				while(edx<len)
				{
					RBYTE_LOW(eax) = *(BYTE *)(edi+edx);
					edx++;
					if(RBYTE_LOW(eax)==VK_TAB)
					{
						nxt = edx;
					}
					else if(RBYTE_LOW(eax)==VK_SPACE)
					{
						spcount++;
					} // endif
					if(edx==nxt)
					{
						spcount = 0;
						*(BYTE *)(esi+ecx) = VK_TAB;
						ecx++;
						eax = pMem->nTab;
						nxt += eax;
					} // endif
				} // endw
				while(spcount)
				{
					*(BYTE *)(esi+ecx) = VK_SPACE;
					ecx++;
					spcount--;
				} // endw
			} // endif
			*(BYTE *)(esi+ecx) = 0;
			eax = ecx;
			eax -= len;
			cpMin += eax;
			cpMax += eax;
			cpen += eax;
			eax = DeleteSelection(pMem, pMem->cpMin, pMem->cpMax);
			pMem->cpMin = eax;
			pMem->cpMax = eax;
			eax = EditPaste(pMem, esi);
		} // endif
		NextLine();
	} // endw
	eax = GlobalFree(hLMem);
	eax = GlobalFree(hCMem);
	eax = cpst;
	pMem->cpMin = eax;
	eax = cpen;
	pMem->cpMax = eax;
	nUndoid++;
	return eax;

	void NextLine(void)
	{
		while(*(BYTE *)edi && *(BYTE *)edi!=VK_RETURN)
		{
			edi++;
		} // endw
		if(*(BYTE *)edi)
		{
			edi++;
		} // endif
		return;

	}

	void GetIndent(void)
	{
		edx = 0;
		while(*(BYTE *)(edi+edx)==VK_SPACE || *(BYTE *)(edi+edx)==VK_TAB)
		{
			edx++;
		} // endw
		eax = edi;
		eax -= hCMem;
		eax += cpMin;
		pMem->cpMin = eax;
		eax += edx;
		pMem->cpMax = eax;
		return;

	}

} // ConvertIndent

