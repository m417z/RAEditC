#include <windows.h>
#include "Data.h"

REG_T InsertNewLine(DWORD hMem, DWORD nLine, DWORD nSize)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	eax = nSize;
	// shl		eax,1
	eax = ExpandCharMem(ebx, eax);
	eax = ExpandLineMem(ebx);
	eax = nLine;
	eax *= 4;
	esi = ((EDIT *)ebx)->hLine;
	esi += eax;
	if(eax<((EDIT *)ebx)->rpLineFree)
	{
		temp1 = esi;
		edi = ((EDIT *)ebx)->rpLineFree;
		edi += ((EDIT *)ebx)->hLine;
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
	((EDIT *)ebx)->rpLineFree += sizeof(LINE);
	eax = ((EDIT *)ebx)->rpCharsFree;
	((LINE *)esi)->rpChars = eax;
	esi -= ((EDIT *)ebx)->hLine;
	((EDIT *)ebx)->rpLine = esi;
	esi = eax;
	esi += ((EDIT *)ebx)->hChars;
	eax = nSize;
	eax >>= 8;
	eax <<= 8;
	eax += MAXFREE;
	((CHARS *)esi)->max = eax;
	eax += sizeof(CHARS);
	((EDIT *)ebx)->rpCharsFree += eax;
	((CHARS *)esi)->len = 0;
	((CHARS *)esi)->state = STATE_CHANGED;
	esi -= ((EDIT *)ebx)->hChars;
	((EDIT *)ebx)->rpChars = esi;
	return eax;

} // InsertNewLine

REG_T AddNewLine(DWORD hMem, DWORD lpLine, DWORD nSize)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	eax = ExpandLineMem(ebx);
	eax = ExpandCharMem(ebx, nSize);
	edx = ((EDIT *)ebx)->rpCharsFree;
	esi = ((EDIT *)ebx)->hLine;
	eax = ((EDIT *)ebx)->rpLineFree;
	esi = esi+eax-sizeof(LINE);
	eax = ((LINE *)esi)->rpChars;
	((LINE *)(esi+sizeof(LINE)))->rpChars = eax;
	((EDIT *)ebx)->rpLineFree += sizeof(LINE);
	((LINE *)esi)->rpChars = edx;
	edi = ((EDIT *)ebx)->hChars;
	edi += edx;
	eax = nSize;
	((CHARS *)edi)->len = eax;
	((CHARS *)edi)->max = eax;
	((CHARS *)edi)->state = 0;
	((CHARS *)edi)->bmid = 0;
	((CHARS *)edi)->errid = 0;
	eax += sizeof(CHARS);
	((EDIT *)ebx)->rpCharsFree += eax;
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

REG_T ExpandCharLine(DWORD hMem)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	esi = ((EDIT *)ebx)->rpChars;
	eax = esi;
	esi += ((EDIT *)ebx)->hChars;
	eax += ((CHARS *)esi)->max;
	eax += sizeof(CHARS);
	if(eax==((EDIT *)ebx)->rpCharsFree)
	{
		// Is at end of chars, just expand
		((CHARS *)esi)->max += MAXFREE;
		((EDIT *)ebx)->rpCharsFree += MAXFREE;
		eax = ((EDIT *)ebx)->rpCharsFree;
	}
	else
	{
		// Move the line to end of buffer
		eax = ((CHARS *)esi)->max;
		eax += MAXFREE+sizeof(CHARS);
		eax = ExpandCharMem(ebx, eax);
		esi = ((EDIT *)ebx)->rpChars;
		esi += ((EDIT *)ebx)->hChars;
		edi = ((EDIT *)ebx)->rpCharsFree;
		edi += ((EDIT *)ebx)->hChars;
		ecx = ((CHARS *)esi)->max;
		ecx += sizeof(CHARS);
		((EDIT *)ebx)->rpCharsFree += ecx;
		edx = ((EDIT *)ebx)->rpLine;
		edx += ((EDIT *)ebx)->hLine;
		eax = edi;
		eax -= ((EDIT *)ebx)->hChars;
		((LINE *)edx)->rpChars = eax;
		((EDIT *)ebx)->rpChars = eax;
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
		((EDIT *)ebx)->rpCharsFree += MAXFREE;
		((CHARS *)esi)->state |= STATE_GARBAGE;
	} // endif
	return eax;

} // ExpandCharLine

REG_T DeleteLine(DWORD hMem, DWORD nLine)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	esi = ((EDIT *)ebx)->hLine;
	edi = 0;
	eax = nLine;
	eax *= 4;
	if(eax<((EDIT *)ebx)->rpLineFree)
	{
		edi = ((EDIT *)ebx)->hChars;
		edx = *(DWORD *)(esi+eax+sizeof(LINE));
		((EDIT *)ebx)->rpChars = edx;
		edi += *(DWORD *)(esi+eax);
		if(((CHARS *)edi)->state&STATE_HIDDEN)
		{
			((EDIT *)ebx)->nHidden--;
		} // endif
		((CHARS *)edi)->state |= STATE_GARBAGE;
		while(eax<((EDIT *)ebx)->rpLineFree)
		{
			ecx = *(DWORD *)(esi+eax+sizeof(LINE));
			*(DWORD *)(esi+eax) = ecx;
			eax += sizeof(LINE);
		} // endw
		((EDIT *)ebx)->rpLineFree -= sizeof(LINE);
	} // endif
	eax = edi;
	return eax;

} // DeleteLine

REG_T InsertChar(DWORD hMem, DWORD cp, DWORD nChr)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	eax = ExpandLineMem(ebx);
	eax = ExpandCharMem(ebx, MAXCHARMEM);
	edx = cp;
	eax = 0;
	if(edx<((EDIT *)ebx)->edta.topcp)
	{
		((EDIT *)ebx)->edta.topyp = eax;
		((EDIT *)ebx)->edta.topln = eax;
		((EDIT *)ebx)->edta.topcp = eax;
	} // endif
	if(edx<((EDIT *)ebx)->edtb.topcp)
	{
		((EDIT *)ebx)->edtb.topyp = eax;
		((EDIT *)ebx)->edtb.topln = eax;
		((EDIT *)ebx)->edtb.topcp = eax;
	} // endif
	eax = GetCharPtr(ebx, edx);
	edi = eax;
	esi = ((EDIT *)ebx)->rpChars;
	esi += ((EDIT *)ebx)->hChars;
	ecx = ((CHARS *)esi)->state;
	if(ecx&STATE_HIDDEN)
	{
		eax = TestExpand(ebx, ((EDIT *)ebx)->line);
	}
	else
	{
		ecx &= STATE_BMMASK;
		if((ecx==STATE_BM2 || ecx==STATE_BM8) && nChr==VK_RETURN)
		{
			eax = Expand(ebx, ((EDIT *)ebx)->line);
		} // endif
	} // endif
	ecx = nChr;
	if(((EDIT *)ebx)->fOvr && ecx!=0x0D)
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
		eax = ExpandCharLine(ebx);
	} // endif
	// Insert char
	esi = ((EDIT *)ebx)->rpChars;
	esi += ((EDIT *)ebx)->hChars;
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
		eax = ((EDIT *)ebx)->rpLine;
		eax /= 4;
		ecx = ((CHARS *)esi)->state;
		ecx &= STATE_BMMASK;
		if(ecx==STATE_BM2 || ecx==STATE_BM8)
		{
			temp1 = eax;
			eax = TestExpand(ebx, eax);
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
		eax = InsertNewLine(ebx, eax, ecx);
		// Find the pointer to old line characters
		esi = temp1;
		esi *= 4;
		esi += ((EDIT *)ebx)->hLine;
		esi = *(DWORD *)esi;
		esi += ((EDIT *)ebx)->hChars;
		ecx = edi;
		edx = 0;
		edi = ((EDIT *)ebx)->rpChars;
		edi += ((EDIT *)ebx)->hChars;
		while(ecx<((CHARS *)esi)->len)
		{
			RBYTE_LOW(eax) = *(BYTE *)(esi+ecx+sizeof(CHARS));
			*(BYTE *)(edi+edx+sizeof(CHARS)) = RBYTE_LOW(eax);
			ecx++;
			edx++;
		} // endw
		// Update lenght of old and new line
		((CHARS *)edi)->len = edx;
		((CHARS *)esi)->len -= edx;
		eax = ((CHARS *)esi)->len;
		((EDIT *)ebx)->cpLine += eax;
		((EDIT *)ebx)->line++;
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
	if(!((EDIT *)ebx)->fChanged)
	{
		((EDIT *)ebx)->fChanged = TRUE;
		temp1 = eax;
		eax = InvalidateRect(((EDIT *)ebx)->hsta, NULL, TRUE);
		eax = temp1;
	} // endif
	((EDIT *)ebx)->nchange++;
	return eax;

} // InsertChar

REG_T DeleteChar(DWORD hMem, DWORD cp)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	edx = cp;
	eax = 0;
	if(edx<((EDIT *)ebx)->edta.topcp)
	{
		((EDIT *)ebx)->edta.topyp = eax;
		((EDIT *)ebx)->edta.topln = eax;
		((EDIT *)ebx)->edta.topcp = eax;
	} // endif
	if(edx<((EDIT *)ebx)->edtb.topcp)
	{
		((EDIT *)ebx)->edtb.topyp = eax;
		((EDIT *)ebx)->edtb.topln = eax;
		((EDIT *)ebx)->edtb.topcp = eax;
	} // endif
	eax = GetCharPtr(ebx, edx);
	edi = eax;
	esi = ((EDIT *)ebx)->rpChars;
	esi += ((EDIT *)ebx)->hChars;
	if(((CHARS *)esi)->state&STATE_HIDDEN)
	{
		eax = TestExpand(ebx, ((EDIT *)ebx)->line);
	} // endif
	eax = *(BYTE *)(esi+edi+sizeof(CHARS));
	temp1 = eax;
	if(RBYTE_LOW(eax)==0x0D)
	{
		edi = esi;
		if(((CHARS *)esi)->len==1)
		{
			eax = DeleteLine(ebx, ((EDIT *)ebx)->line);
		}
		else
		{
			temp2 = ((EDIT *)ebx)->fOvr;
			((EDIT *)ebx)->fOvr = FALSE;
			eax = cp;
			eax++;
			eax = GetCharPtr(ebx, eax);
			esi = ((EDIT *)ebx)->rpChars;
			esi += ((EDIT *)ebx)->hChars;
			if(((CHARS *)esi)->len)
			{
				eax = DeleteLine(ebx, ((EDIT *)ebx)->line);
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
						eax = InsertChar(ebx, cp, eax);
						edi++;
						cp++;
						eax = 0;
					} // endw
					temp3 = eax;
					eax = GlobalFree(esi);
					esi = ((EDIT *)ebx)->line;
					esi *= 4;
					esi += ((EDIT *)ebx)->hLine;
					esi = ((LINE *)esi)->rpChars;
					esi += ((EDIT *)ebx)->hChars;
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
				eax = DeleteLine(ebx, ((EDIT *)ebx)->line);
				eax = GetCharPtr(ebx, 0);
				esi = edi;
			} // endif
			((CHARS *)esi)->state &= -1 ^ STATE_CHANGESAVED;
			((CHARS *)esi)->state |= STATE_CHANGED;
			((EDIT *)ebx)->fOvr = temp2;
		} // endif
		if(!((EDIT *)ebx)->fChanged)
		{
			((EDIT *)ebx)->fChanged = TRUE;
			eax = InvalidateRect(((EDIT *)ebx)->hsta, NULL, TRUE);
		} // endif
		((EDIT *)ebx)->nchange++;
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
		if(!((EDIT *)ebx)->fChanged)
		{
			((EDIT *)ebx)->fChanged = TRUE;
			eax = InvalidateRect(((EDIT *)ebx)->hsta, NULL, TRUE);
		} // endif
		((EDIT *)ebx)->nchange++;
	} // endif
	eax = temp1;
	return eax;

} // DeleteChar

REG_T DeleteSelection(DWORD hMem, DWORD cpMin, DWORD cpMax)
{
	REG_T eax = 0, ecx, edx, ebx, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	eax = cpMin;
	if(eax>cpMax)
	{
		temp1 = cpMax;
		cpMax = eax;
		eax = temp1;
		cpMin = eax;
	} // endif
	temp1 = eax;
	eax = GetLineFromCp(ebx, cpMin);
	edi = eax;
	eax = GetLineFromCp(ebx, cpMax);
	while(edi<eax)
	{
		temp2 = eax;
		eax = TestExpand(ebx, edi);
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
			eax = DeleteChar(hMem, eax);
			*(BYTE *)edi = RBYTE_LOW(eax);
			edi++;
			cpMax--;
			eax = cpMin;
		} // endw
		edi = temp2;
		eax = temp1;
		eax = SaveUndo(ebx, UNDO_DELETEBLOCK, cpMin, edi, eax);
		eax = GlobalFree(edi);
		eax = cpMin;
		((EDIT *)ebx)->cpMin = eax;
		((EDIT *)ebx)->cpMax = eax;
	} // endif
	return eax;

} // DeleteSelection

REG_T DeleteSelectionBlock(DWORD hMem, DWORD lnMin, DWORD clMin, DWORD lnMax, DWORD clMax)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
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
			eax = GetBlockCp(ebx, lnMin, clMin);
			esi = eax;
			eax = GetChar(ebx, esi);
			if(eax && eax!=VK_RETURN)
			{
				eax = GetBlockCp(ebx, lnMin, clMax);
				edi = eax;
anon_1:
				if(R_SIGNED(edi) > R_SIGNED(esi))
				{
					edi--;
					eax = GetChar(ebx, edi);
					if(!eax || eax==VK_RETURN)
					{
						goto anon_1;
					} // endif
					edi++;
					eax = DeleteSelection(ebx, esi, edi);
				} // endif
			} // endif
			lnMin++;
			eax = lnMin;
		} // endw
	} // endif
	return eax;

} // DeleteSelectionBlock

REG_T EditInsert(DWORD hMem, DWORD cp, DWORD lpBuff)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
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
				eax = InsertChar(ebx, edi, eax);
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


