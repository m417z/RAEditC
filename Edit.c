#include "Edit.h"

#include "Block.h"
#include "Memory.h"
#include "Misc.h"
#include "Position.h"
#include "Undo.h"

REG_T InsertNewLine(EDIT *pMem, DWORD nLine, DWORD nSize)
{
	REG_T eax = 0, ecx, ebx, esi, edi;
	REG_T temp1;

	eax = nSize;
	// shl		eax,1
	eax = ExpandCharMem(pMem, eax);
	eax = ExpandLineMem(pMem);
	eax = nLine;
	eax *= 4;
	esi = pMem->hLine;
	esi += eax;
	if(eax<pMem->rpLineFree)
	{
		temp1 = esi;
		edi = pMem->rpLineFree;
		edi += pMem->hLine;
		ecx = edi;
		ecx -= esi;
		esi = edi;
		esi -= sizeof(LINE);
		ecx /= 4;
		while(ecx > 0)
		{
			*(DWORD *)edi = *(DWORD *)esi;
			edi -= 4;
			esi -= 4;
			ecx--;
		}
		esi = temp1;
	} // endif
	pMem->rpLineFree += sizeof(LINE);
	eax = pMem->rpCharsFree;
	((LINE *)esi)->rpChars = eax;
	esi -= (ULONG_PTR)pMem->hLine;
	pMem->rpLine = esi;
	esi = eax;
	esi += pMem->hChars;
	eax = nSize;
	eax >>= 8;
	eax <<= 8;
	eax += MAXFREE;
	((CHARS *)esi)->max = eax;
	eax += sizeof(CHARS);
	pMem->rpCharsFree += eax;
	((CHARS *)esi)->len = 0;
	((CHARS *)esi)->state = STATE_CHANGED;
	esi -= (ULONG_PTR)pMem->hChars;
	pMem->rpChars = esi;
	return eax;

} // InsertNewLine

REG_T AddNewLine(EDIT *pMem, DWORD lpLine, DWORD nSize)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;

	eax = ExpandLineMem(pMem);
	eax = ExpandCharMem(pMem, nSize);
	edx = pMem->rpCharsFree;
	esi = pMem->hLine;
	eax = pMem->rpLineFree;
	esi = esi+eax-sizeof(LINE);
	eax = ((LINE *)esi)->rpChars;
	((LINE *)(esi+sizeof(LINE)))->rpChars = eax;
	pMem->rpLineFree += sizeof(LINE);
	((LINE *)esi)->rpChars = edx;
	edi = pMem->hChars;
	edi += edx;
	eax = nSize;
	((CHARS *)edi)->len = eax;
	((CHARS *)edi)->max = eax;
	((CHARS *)edi)->state = 0;
	((CHARS *)edi)->bmid = 0;
	((CHARS *)edi)->errid = 0;
	eax += sizeof(CHARS);
	pMem->rpCharsFree += eax;
	ecx = nSize;
	esi = lpLine;
	edi = edi+sizeof(CHARS);
	while(ecx > 0)
	{
		*(BYTE *)edi = *(BYTE *)esi;
		edi++;
		esi++;
		ecx--;
	}
	return eax;

} // AddNewLine

REG_T ExpandCharLine(EDIT *pMem)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2;

	esi = pMem->rpChars;
	eax = esi;
	esi += pMem->hChars;
	eax += ((CHARS *)esi)->max;
	eax += sizeof(CHARS);
	if(eax==pMem->rpCharsFree)
	{
		// Is at end of chars, just expand
		((CHARS *)esi)->max += MAXFREE;
		pMem->rpCharsFree += MAXFREE;
		eax = pMem->rpCharsFree;
	}
	else
	{
		// Move the line to end of buffer
		eax = ((CHARS *)esi)->max;
		eax += MAXFREE+sizeof(CHARS);
		eax = ExpandCharMem(pMem, eax);
		esi = pMem->rpChars;
		esi += pMem->hChars;
		edi = pMem->rpCharsFree;
		edi += pMem->hChars;
		ecx = ((CHARS *)esi)->max;
		ecx += sizeof(CHARS);
		pMem->rpCharsFree += ecx;
		edx = pMem->rpLine;
		edx += pMem->hLine;
		eax = edi;
		eax -= (ULONG_PTR)pMem->hChars;
		((LINE *)edx)->rpChars = eax;
		pMem->rpChars = eax;
		temp1 = esi;
		temp2 = edi;
		while(ecx > 0)
		{
			*(BYTE *)edi = *(BYTE *)esi;
			edi++;
			esi++;
			ecx--;
		}
		edi = temp2;
		esi = temp1;
		((CHARS *)edi)->max += MAXFREE;
		pMem->rpCharsFree += MAXFREE;
		((CHARS *)esi)->state |= STATE_GARBAGE;
	} // endif
	return eax;

} // ExpandCharLine

REG_T DeleteLine(EDIT *pMem, DWORD nLine)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;

	esi = pMem->hLine;
	edi = 0;
	eax = nLine;
	eax *= 4;
	if(eax<pMem->rpLineFree)
	{
		edi = pMem->hChars;
		edx = *(DWORD *)(esi+eax+sizeof(LINE));
		pMem->rpChars = edx;
		edi += *(DWORD *)(esi+eax);
		if(((CHARS *)edi)->state&STATE_HIDDEN)
		{
			pMem->nHidden--;
		} // endif
		((CHARS *)edi)->state |= STATE_GARBAGE;
		while(eax<pMem->rpLineFree)
		{
			ecx = *(DWORD *)(esi+eax+sizeof(LINE));
			*(DWORD *)(esi+eax) = ecx;
			eax += sizeof(LINE);
		} // endw
		pMem->rpLineFree -= sizeof(LINE);
	} // endif
	eax = edi;
	return eax;

} // DeleteLine

REG_T InsertChar(EDIT *pMem, DWORD cp, DWORD nChr)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2;

	eax = ExpandLineMem(pMem);
	eax = ExpandCharMem(pMem, MAXCHARMEM);
	edx = cp;
	eax = 0;
	if(edx<pMem->edta.topcp)
	{
		pMem->edta.topyp = eax;
		pMem->edta.topln = eax;
		pMem->edta.topcp = eax;
	} // endif
	if(edx<pMem->edtb.topcp)
	{
		pMem->edtb.topyp = eax;
		pMem->edtb.topln = eax;
		pMem->edtb.topcp = eax;
	} // endif
	eax = GetCharPtr(pMem, edx, &ecx, &edx);
	edi = eax;
	esi = pMem->rpChars;
	esi += pMem->hChars;
	ecx = ((CHARS *)esi)->state;
	if(ecx&STATE_HIDDEN)
	{
		eax = TestExpand(pMem, pMem->line);
	}
	else
	{
		ecx &= STATE_BMMASK;
		if((ecx==STATE_BM2 || ecx==STATE_BM8) && nChr==VK_RETURN)
		{
			eax = Expand(pMem, pMem->line);
		} // endif
	} // endif
	ecx = nChr;
	if(pMem->fOvr && ecx!=0x0D)
	{
		if(edi<((CHARS *)esi)->len)
		{
			eax = *(BYTE *)(esi+edi+sizeof(CHARS));
			if(RBYTE_LOW(eax)!=0x0D)
			{
				// Replace char
				((CHARS *)esi)->state &= -1 ^ STATE_CHANGESAVED;
				((CHARS *)esi)->state |= STATE_CHANGED;
				*(BYTE *)(esi+edi+sizeof(CHARS)) = RBYTE_LOW(ecx);
				goto Ex;
			} // endif
		} // endif
	} // endif
	eax = ((CHARS *)esi)->max;
	eax = ((CHARS *)esi)->len;
	if(eax==((CHARS *)esi)->max)
	{
		eax = ExpandCharLine(pMem);
	} // endif
	// Insert char
	esi = pMem->rpChars;
	esi += pMem->hChars;
	temp1 = esi;
	temp2 = edi;
	ecx = ((CHARS *)esi)->len;
	ecx -= edi;
	if(ecx)
	{
		esi = esi+ecx+sizeof(CHARS)-1;
		esi = esi+edi;
		edi = esi+1;
		while(ecx > 0)
		{
			*(BYTE *)edi = *(BYTE *)esi;
			edi--;
			esi--;
			ecx--;
		}
	} // endif
	edi = temp2;
	esi = temp1;
	ecx = nChr;
	*(BYTE *)(esi+edi+sizeof(CHARS)) = RBYTE_LOW(ecx);
	((CHARS *)esi)->len++;
	ecx = nChr;
	if(ecx==0x0D)
	{
		// Break the line
		eax = pMem->rpLine;
		eax /= 4;
		ecx = ((CHARS *)esi)->state;
		ecx &= STATE_BMMASK;
		if(ecx==STATE_BM2 || ecx==STATE_BM8)
		{
			temp1 = eax;
			eax = TestExpand(pMem, eax);
			eax = temp1;
		} // endif
		// Save line number
		temp1 = eax;
		eax++;
		edi++;
		ecx = MAXFREE;
		ecx += ((CHARS *)esi)->len;
		ecx -= edi;
		// Insert a new line and expand the CHAR mem
		eax = InsertNewLine(pMem, eax, ecx);
		// Find the pointer to old line characters
		esi = temp1;
		esi *= 4;
		esi += pMem->hLine;
		esi = *(DWORD *)esi;
		esi += pMem->hChars;
		ecx = edi;
		edx = 0;
		edi = pMem->rpChars;
		edi += pMem->hChars;
		while(ecx<((CHARS *)esi)->len)
		{
			RBYTE_LOW(eax) = *(BYTE *)(esi+ecx+sizeof(CHARS));
			*(BYTE *)(edi+edx+sizeof(CHARS)) = RBYTE_LOW(eax);
			ecx++;
			edx++;
		} // endw
		// Update length of old and new line
		((CHARS *)edi)->len = edx;
		((CHARS *)esi)->len -= edx;
		eax = ((CHARS *)esi)->len;
		pMem->cpLine += eax;
		pMem->line++;
		if(edx>1)
		{
			((CHARS *)esi)->state &= -1 ^ STATE_CHANGESAVED;
			((CHARS *)esi)->state |= STATE_CHANGED;
		} // endif
		((CHARS *)edi)->state &= -1 ^ STATE_CHANGESAVED;
		((CHARS *)edi)->state |= STATE_CHANGED;
	}
	else
	{
		((CHARS *)esi)->state &= -1 ^ STATE_CHANGESAVED;
		((CHARS *)esi)->state |= STATE_CHANGED;
	} // endif
	eax = 0;
Ex:
	if(!pMem->fChanged)
	{
		pMem->fChanged = TRUE;
		temp1 = eax;
		eax = InvalidateRect(pMem->hsta, NULL, TRUE);
		eax = temp1;
	} // endif
	pMem->nchange++;
	return eax;

} // InsertChar

REG_T DeleteChar(EDIT *pMem, DWORD cp)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3;

	edx = cp;
	eax = 0;
	if(edx<pMem->edta.topcp)
	{
		pMem->edta.topyp = eax;
		pMem->edta.topln = eax;
		pMem->edta.topcp = eax;
	} // endif
	if(edx<pMem->edtb.topcp)
	{
		pMem->edtb.topyp = eax;
		pMem->edtb.topln = eax;
		pMem->edtb.topcp = eax;
	} // endif
	eax = GetCharPtr(pMem, edx, &ecx, &edx);
	edi = eax;
	esi = pMem->rpChars;
	esi += pMem->hChars;
	if(((CHARS *)esi)->state&STATE_HIDDEN)
	{
		eax = TestExpand(pMem, pMem->line);
	} // endif
	eax = *(BYTE *)(esi+edi+sizeof(CHARS));
	temp1 = eax;
	if(RBYTE_LOW(eax)==0x0D)
	{
		edi = esi;
		if(((CHARS *)esi)->len==1)
		{
			eax = DeleteLine(pMem, pMem->line);
		}
		else
		{
			temp2 = pMem->fOvr;
			pMem->fOvr = FALSE;
			eax = cp;
			eax++;
			eax = GetCharPtr(pMem, eax, &ecx, &edx);
			esi = pMem->rpChars;
			esi += pMem->hChars;
			if(((CHARS *)esi)->len)
			{
				eax = DeleteLine(pMem, pMem->line);
				if(eax)
				{
					esi = eax;
					eax = ((CHARS *)esi)->len;
					eax += sizeof(CHARS);
					temp3 = eax;
					eax = GlobalAlloc(GMEM_FIXED, eax);
					edi = eax;
					ecx = temp3;
					temp3 = edi;
					while(ecx > 0)
					{
						*(BYTE *)edi = *(BYTE *)esi;
						edi++;
						esi++;
						ecx--;
					}
					esi = temp3;
					edi = 0;
					while(edi<((CHARS *)esi)->len)
					{
						eax = *(BYTE *)(esi+edi+sizeof(CHARS));
						if(eax==VK_RETURN)
						{
							break;
						} // endif
						eax = InsertChar(pMem, cp, eax);
						edi++;
						cp++;
						eax = 0;
					} // endw
					temp3 = eax;
					eax = GlobalFree(esi);
					esi = pMem->line;
					esi *= 4;
					esi += pMem->hLine;
					esi = ((LINE *)esi)->rpChars;
					esi += pMem->hChars;
					eax = temp3;
					if(!eax)
					{
						((CHARS *)esi)->len--;
					} // endif
				} // endif
			}
			else
			{
				((CHARS *)edi)->len--;
				eax = DeleteLine(pMem, pMem->line);
				eax = GetCharPtr(pMem, 0, &ecx, &edx);
				esi = edi;
			} // endif
			((CHARS *)esi)->state &= -1 ^ STATE_CHANGESAVED;
			((CHARS *)esi)->state |= STATE_CHANGED;
			pMem->fOvr = temp2;
		} // endif
		if(!pMem->fChanged)
		{
			pMem->fChanged = TRUE;
			eax = InvalidateRect(pMem->hsta, NULL, TRUE);
		} // endif
		pMem->nchange++;
	}
	else if(RBYTE_LOW(eax) && ((CHARS *)esi)->len)
	{
		((CHARS *)esi)->len--;
		if(((CHARS *)esi)->len!=0)
		{
			while(edi<((CHARS *)esi)->len)
			{
				RBYTE_LOW(eax) = *(BYTE *)(esi+edi+sizeof(CHARS)+1);
				*(BYTE *)(esi+edi+sizeof(CHARS)) = RBYTE_LOW(eax);
				edi++;
			} // endw
			*(BYTE *)(esi+edi+sizeof(CHARS)) = 0;
		} // endif
		((CHARS *)esi)->state &= -1 ^ STATE_CHANGESAVED;
		((CHARS *)esi)->state |= STATE_CHANGED;
		if(!pMem->fChanged)
		{
			pMem->fChanged = TRUE;
			eax = InvalidateRect(pMem->hsta, NULL, TRUE);
		} // endif
		pMem->nchange++;
	} // endif
	eax = temp1;
	return eax;

} // DeleteChar

REG_T DeleteSelection(EDIT *pMem, DWORD cpMin, DWORD cpMax)
{
	REG_T eax = 0, ebx, edi;
	REG_T temp1, temp2;

	eax = cpMin;
	if(eax>cpMax)
	{
		temp1 = cpMax;
		cpMax = eax;
		eax = temp1;
		cpMin = eax;
	} // endif
	temp1 = eax;
	eax = GetLineFromCp(pMem, cpMin);
	edi = eax;
	eax = GetLineFromCp(pMem, cpMax);
	while(edi<eax)
	{
		temp2 = eax;
		eax = TestExpand(pMem, edi);
		eax = temp2;
		edi++;
	} // endw
	eax = temp1;
	if(eax!=cpMax)
	{
		eax = cpMax;
		eax -= cpMin;
		temp1 = eax;
		eax = xGlobalAlloc(GMEM_FIXED, eax);
		edi = eax;
		temp2 = edi;
		eax = cpMin;
		while(eax!=cpMax)
		{
			eax = DeleteChar(pMem, eax);
			*(BYTE *)edi = RBYTE_LOW(eax);
			edi++;
			cpMax--;
			eax = cpMin;
		} // endw
		edi = temp2;
		eax = temp1;
		eax = SaveUndo(pMem, UNDO_DELETEBLOCK, cpMin, edi, eax);
		eax = GlobalFree(edi);
		eax = cpMin;
		pMem->cpMin = eax;
		pMem->cpMax = eax;
	} // endif
	return eax;

} // DeleteSelection

REG_T DeleteSelectionBlock(EDIT *pMem, DWORD lnMin, DWORD clMin, DWORD lnMax, DWORD clMax)
{
	REG_T eax = 0, edx, ebx, esi, edi;

	eax = clMin;
	edx = clMax;
	if(eax!=edx)
	{
		if(eax>edx)
		{
			clMax = eax;
			clMin = edx;
		} // endif
		eax = lnMin;
		edx = lnMax;
		if(eax>edx)
		{
			lnMax = eax;
			lnMin = edx;
		} // endif
		eax = lnMin;
		while(eax<=lnMax)
		{
			eax = GetBlockCp(pMem, lnMin, clMin);
			esi = eax;
			eax = GetChar(pMem, esi);
			if(eax && eax!=VK_RETURN)
			{
				eax = GetBlockCp(pMem, lnMin, clMax);
				edi = eax;
anon_1:
				if(R_SIGNED(edi) > R_SIGNED(esi))
				{
					edi--;
					eax = GetChar(pMem, edi);
					if(!eax || eax==VK_RETURN)
					{
						goto anon_1;
					} // endif
					edi++;
					eax = DeleteSelection(pMem, esi, edi);
				} // endif
			} // endif
			lnMin++;
			eax = lnMin;
		} // endw
	} // endif
	return eax;

} // DeleteSelectionBlock

REG_T EditInsert(EDIT *pMem, DWORD cp, DWORD lpBuff)
{
	REG_T eax = 0, ebx, esi, edi;

	esi = lpBuff;
	edi = cp;
	if(esi)
	{
		RBYTE_LOW(eax) = *(BYTE *)esi;
		while(RBYTE_LOW(eax))
		{
			if(RBYTE_LOW(eax)!=0x0A)
			{
				eax = RBYTE_LOW(eax);
				eax = InsertChar(pMem, edi, eax);
				edi++;
			} // endif
			esi++;
			RBYTE_LOW(eax) = *(BYTE *)esi;
		} // endw
	} // endif
	eax = edi;
	eax -= cp;
	return eax;

} // EditInsert


