#include <windows.h>
#include "Data.h"

REG_T SetClipData(LPSTR lpData, DWORD dwSize)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	HANDLE hMem;
	DWORD pMem;

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

REG_T EditCopy(DWORD hMem, DWORD lpCMem)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD cpMin;
	DWORD cpMax;

	ebx = hMem;
	eax = ((EDIT *)ebx)->cpMin;
	edx = ((EDIT *)ebx)->cpMax;
	if(eax>edx)
	{
		temp1 = eax;
		eax = edx;
		edx = temp1;
	} // endif
	cpMin = eax;
	cpMax = edx;
	eax = GetCharPtr(ebx, cpMin);
	ecx = eax;
	edi = lpCMem;
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

REG_T EditCopyBlock(DWORD hMem, DWORD lpCMem)
{
	REG_T eax = 0, ecx, edx, ebx, esi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	BLOCKRANGE blrg;

	auto void CopyBlockChar(void);
	auto void CopyBlockLine(void);

	ebx = hMem;
	eax = GetBlockRange(& ((EDIT *)ebx)->blrg, &blrg);
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
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		eax = GetBlockCp(ebx, edx, eax);
		eax = GetChar(ebx, eax);
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
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
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
		*(DWORD *)esi = eax;
		esi += 2;
		return;

	}

} // EditCopyBlock

REG_T EditCopyNoLF(DWORD hMem, DWORD lpCMem)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD cpMin;
	DWORD cpMax;

	ebx = hMem;
	eax = ((EDIT *)ebx)->cpMin;
	edx = ((EDIT *)ebx)->cpMax;
	if(eax>edx)
	{
		temp1 = eax;
		eax = edx;
		edx = temp1;
	} // endif
	cpMin = eax;
	cpMax = edx;
	eax = GetCharPtr(ebx, cpMin);
	ecx = eax;
	edi = lpCMem;
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

REG_T Copy(DWORD hMem)
{
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD hCMem;

	ebx = hMem;
	if(!(((EDIT *)ebx)->nMode&MODE_BLOCK))
	{
		eax = ((EDIT *)ebx)->cpMin;
		eax -= ((EDIT *)ebx)->cpMax;
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
			eax = EditCopy(ebx, eax);
			edx = temp1;
			eax = SetClipData(edx, eax);
			eax = GlobalUnlock(hCMem);
			eax = GlobalFree(hCMem);
		} // endif
	}
	else
	{
		eax = ((EDIT *)ebx)->blrg.clMin;
		if(eax!=((EDIT *)ebx)->blrg.clMax)
		{
			eax = xGlobalAlloc(GMEM_ZEROINIT, 256*1024);
			hCMem = eax;
			eax = GlobalLock(hCMem);
			temp1 = eax;
			eax = EditCopyBlock(ebx, eax);
			edx = temp1;
			eax = SetClipData(edx, eax);
			eax = GlobalUnlock(hCMem);
			eax = GlobalFree(hCMem);
		} // endif
	} // endif
	return eax;

} // Copy

REG_T EditPaste(DWORD hMem, DWORD hData)
{
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	auto void InsertMem(void);

	ebx = hMem;
	eax = hData;
	if(eax)
	{
		InsertMem();
	}
	else
	{
		eax = OpenClipboard(((EDIT *)ebx)->hwnd);
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
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		temp1 = eax;
		eax = GlobalLock(eax);
		temp2 = ((EDIT *)ebx)->fOvr;
		((EDIT *)ebx)->fOvr = FALSE;
		temp3 = ((EDIT *)ebx)->cpMin;
		temp4 = eax;
		eax = EditInsert(ebx, ((EDIT *)ebx)->cpMin, eax);
		((EDIT *)ebx)->cpMin += eax;
		((EDIT *)ebx)->cpMax += eax;
		edx = temp4;
		ecx = temp3;
		eax = SaveUndo(ebx, UNDO_INSERTBLOCK, ecx, edx, eax);
		((EDIT *)ebx)->fOvr = temp2;
		eax = temp1;
		eax = GlobalUnlock(eax);
		return;

	}

} // EditPaste

REG_T EditPasteBlock(DWORD hMem, DWORD hData)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD nSpc;
	BLOCKRANGE blrg;

	auto void InsertMem(void);

	ebx = hMem;
	eax = GetBlockRange(& ((EDIT *)ebx)->blrg, &blrg);
	eax = hData;
	if(eax)
	{
		InsertMem();
	}
	else
	{
		eax = OpenClipboard(((EDIT *)ebx)->hwnd);
		if(eax)
		{
			eax = GetClipboardData(CF_TEXT);
			if(eax)
			{
				InsertMem();
			} // endif
			eax = CloseClipboard();
			eax = blrg.lnMin;
			((EDIT *)ebx)->blrg.lnMin = eax;
			((EDIT *)ebx)->blrg.lnMax = eax;
			eax = blrg.clMin;
			((EDIT *)ebx)->blrg.clMin = eax;
			((EDIT *)ebx)->blrg.clMax = eax;
		} // endif
	} // endif
	return eax;

	void InsertMem(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		temp1 = eax;
		eax = GlobalLock(eax);
		esi = eax;
		temp2 = ((EDIT *)ebx)->fOvr;
		((EDIT *)ebx)->fOvr = FALSE;
		nSpc = 0;
anon_1:
		eax = GetBlockCp(ebx, blrg.lnMin, blrg.clMin);
		edi = eax;
		eax = GetChar(ebx, edi);
		if(!eax)
		{
			eax = InsertChar(ebx, edi, VK_RETURN);
		} // endif
		if(blrg.clMin)
		{
			eax = GetBlockCp(ebx, blrg.lnMin, blrg.clMin);
			temp3 = eax;
			eax = blrg.clMin;
			eax--;
			eax = GetBlockCp(ebx, blrg.lnMin, eax);
			edx = temp3;
			if(eax==edx)
			{
				temp3 = edx;
				eax = GetCharPtr(ebx, edx);
				edx = temp3;
				eax = InsertChar(ebx, edx, VK_SPACE);
				nSpc++;
				goto anon_1;
			} // endif
		} // endif
		eax = GetBlockCp(ebx, blrg.lnMin, blrg.clMin);
		edi = eax;
		temp3 = edi;
		while(*(BYTE *)esi!=VK_RETURN && *(BYTE *)esi)
		{
			eax = *(BYTE *)esi;
			eax = InsertChar(ebx, edi, eax);
			edi++;
			esi++;
		} // endw
		((EDIT *)ebx)->cpMin = edi;
		((EDIT *)ebx)->cpMax = edi;
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
		eax = GetCharPtr(ebx, eax);
		ecx = temp3;
		edi -= ecx;
		edx = ((EDIT *)ebx)->rpChars;
		edx += ((EDIT *)ebx)->hChars;
		edx += sizeof(CHARS);
		edx += eax;
		edx -= nSpc;
		ecx -= nSpc;
		eax = edi;
		eax += nSpc;
		eax = SaveUndo(ebx, UNDO_INSERTBLOCK, ecx, edx, eax);
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
		((EDIT *)ebx)->fOvr = temp2;
		eax = temp1;
		eax = GlobalUnlock(eax);
		return;

	}

} // EditPasteBlock

REG_T Paste(DWORD hMem, DWORD hWin, DWORD hData)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	POINT pt;
	BLOCKRANGE blrg;

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
	if(!(((EDIT *)ebx)->nMode&MODE_BLOCK))
	{
		eax = DeleteSelection(ebx, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
		((EDIT *)ebx)->cpMin = eax;
		((EDIT *)ebx)->cpMax = eax;
		eax = EditPaste(ebx, hData);
	}
	else
	{
		eax = GetBlockRange(& ((EDIT *)ebx)->blrg, &blrg);
		eax = DeleteSelectionBlock(ebx, blrg.lnMin, blrg.clMin, blrg.lnMax, blrg.clMax);
		eax = GetBlockCp(ebx, blrg.lnMin, blrg.clMin);
		((EDIT *)ebx)->cpMin = eax;
		((EDIT *)ebx)->cpMax = eax;
		eax = EditPasteBlock(ebx, hData);
	} // endif
	eax = GetCharPtr(ebx, ((EDIT *)ebx)->cpMin);
	eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
	eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
	eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
	eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
	eax = SetCpxMax(ebx, hWin);
	eax = SelChange(ebx, SEL_TEXT);
	return eax;

} // Paste

REG_T Cut(DWORD hMem, DWORD hWin)
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
	if(!(((EDIT *)ebx)->nMode&MODE_BLOCK))
	{
		eax = Copy(ebx);
		eax = DeleteSelection(ebx, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
		((EDIT *)ebx)->cpMin = eax;
		((EDIT *)ebx)->cpMax = eax;
	}
	else
	{
		eax = GetBlockRange(& ((EDIT *)ebx)->blrg, & ((EDIT *)ebx)->blrg);
		eax = Copy(ebx);
		eax = DeleteSelectionBlock(ebx, ((EDIT *)ebx)->blrg.lnMin, ((EDIT *)ebx)->blrg.clMin, ((EDIT *)ebx)->blrg.lnMax, ((EDIT *)ebx)->blrg.clMax);
		eax = ((EDIT *)ebx)->blrg.clMin;
		edx = ((EDIT *)ebx)->blrg.lnMin;
		((EDIT *)ebx)->blrg.clMax = eax;
		((EDIT *)ebx)->blrg.lnMax = edx;
		eax = GetBlockCp(ebx, edx, eax);
		((EDIT *)ebx)->cpMin = eax;
		((EDIT *)ebx)->cpMax = eax;
	} // endif
	eax = GetCharPtr(ebx, ((EDIT *)ebx)->cpMin);
	eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
	eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
	eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
	eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
	eax = SetCpxMax(ebx, hWin);
	eax = SelChange(ebx, SEL_TEXT);
	return eax;

} // Cut

REG_T ConvertCase(DWORD hMem, DWORD nFunction)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	nUndoid++;
	eax = ((EDIT *)ebx)->cpMin;
	edx = ((EDIT *)ebx)->cpMax;
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
	eax = EditCopyNoLF(ebx, edi);
	if(nFunction==CONVERT_UPPERCASE)
	{
		eax = CharUpper(edi);
	}
	else if(nFunction==CONVERT_LOWERCASE)
	{
		eax = CharLower(edi);
	} // endif
	eax = Paste(ebx, ((EDIT *)ebx)->focus, edi);
	eax = GlobalFree(edi);
	nUndoid++;
	return eax;

} // ConvertCase

REG_T ConvertIndent(DWORD hMem, DWORD nFunction)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD hCMem;
	DWORD hLMem;
	DWORD cpst;
	DWORD cpen;
	DWORD cpMin;
	DWORD cpMax;
	DWORD nxt;
	DWORD len;
	DWORD spcount;

	auto void NextLine(void);
	auto void GetIndent(void);

	ebx = hMem;
	nUndoid++;
	edx = ((EDIT *)ebx)->cpMin;
	eax = ((EDIT *)ebx)->cpMax;
	if(eax<edx)
	{
		temp1 = eax;
		eax = edx;
		edx = temp1;
	} // endif
	temp1 = eax;
	eax = GetLineFromCp(ebx, edx);
	eax = GetCpFromLine(ebx, eax);
	edx = eax;
	eax = temp1;
	cpst = edx;
	cpMin = edx;
	((EDIT *)ebx)->cpMin = edx;
	cpen = eax;
	cpMax = eax;
	((EDIT *)ebx)->cpMax = eax;
	eax -= edx;
	eax++;
	eax = xGlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, eax);
	edi = eax;
	hCMem = eax;
	eax = xGlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, 1024*32);
	hLMem = eax;
	eax = EditCopyNoLF(ebx, edi);
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
				eax = ((EDIT *)ebx)->nTab;
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
						eax = ((EDIT *)ebx)->nTab;
						nxt += eax;
					} // endif
					edx++;
				} // endw
			}
			else if(nFunction==CONVERT_SPACETOTAB)
			{
				eax = ((EDIT *)ebx)->nTab;
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
						eax = ((EDIT *)ebx)->nTab;
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
			eax = DeleteSelection(ebx, ((EDIT *)ebx)->cpMin, ((EDIT *)ebx)->cpMax);
			((EDIT *)ebx)->cpMin = eax;
			((EDIT *)ebx)->cpMax = eax;
			eax = EditPaste(ebx, esi);
		} // endif
		NextLine();
	} // endw
	eax = GlobalFree(hLMem);
	eax = GlobalFree(hCMem);
	eax = cpst;
	((EDIT *)ebx)->cpMin = eax;
	eax = cpen;
	((EDIT *)ebx)->cpMax = eax;
	nUndoid++;
	return eax;

	void NextLine(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
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
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		edx = 0;
		while(*(BYTE *)(edi+edx)==VK_SPACE || *(BYTE *)(edi+edx)==VK_TAB)
		{
			edx++;
		} // endw
		eax = edi;
		eax -= hCMem;
		eax += cpMin;
		((EDIT *)ebx)->cpMin = eax;
		eax += edx;
		((EDIT *)ebx)->cpMax = eax;
		return;

	}

} // ConvertIndent

