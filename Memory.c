#include <windows.h>
#include "Data.h"

REG_T xGlobalAlloc(DWORD t, DWORD s)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	s >>= 3;
	s++;
	s <<= 3;
	eax = GlobalAlloc(t, s);
	if(!eax)
	{
		eax = MessageBox(NULL, &szGlobalFail, &szToolTip, MB_OK);
		eax = 0;
	} // endif
	return eax;

} // xGlobalAlloc

REG_T xHeapAlloc(DWORD h, DWORD t, DWORD s)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	s >>= 3;
	s++;
	s <<= 3;
	eax = HeapAlloc(h, t, s);
	if(!eax)
	{
		eax = MessageBox(NULL, &szHeapFail, &szToolTip, MB_OK);
		eax = 0;
	} // endif
	return eax;

} // xHeapAlloc

REG_T ExpandLineMem(DWORD hMem)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	eax = ((EDIT *)ebx)->rpLineFree;
	eax += MAXLINEMEM;
	eax >>= 12;
	eax++;
	eax <<= 12;
	if(eax>((EDIT *)ebx)->cbLine)
	{
		esi = ((EDIT *)ebx)->hLine;
		edi = ((EDIT *)ebx)->cbLine;
		((EDIT *)ebx)->cbLine += MAXLINEMEM;
		eax = HeapAlloc(((EDIT *)ebx)->hHeap, HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY, ((EDIT *)ebx)->cbLine);
		if(!eax)
		{
			((EDIT *)ebx)->cbLine = edi;
			eax = MessageBox(((EDIT *)ebx)->hwnd, &szMemFailLine, &szToolTip, MB_OK);
			eax = 0;
		}
		else
		{
			((EDIT *)ebx)->hLine = eax;
			ecx = edi;
			edi = eax;
			temp1 = esi;
			ecx /= 4;
			while(ecx > 0)
			{
				*(DWORD *)edi = *(DWORD *)esi;
				edi += 4;
				esi += 4;
				ecx--;
			}
			esi = temp1;
			eax = HeapFree(((EDIT *)ebx)->hHeap, 0, esi);
		} // endif
	} // endif
	return eax;

} // ExpandLineMem

REG_T GarbageCollection(DWORD lpEdit, DWORD lpLine, DWORD lpSrc, DWORD lpDst)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	eax = lpLine;
	ebx = lpEdit;
	ecx = ((EDIT *)ebx)->rpLineFree;
	lpLine += ecx;
	edi = lpDst;
anon_1:
	esi = lpSrc;
	esi += ((LINE *)eax)->rpChars;
	edx = edi;
	edx -= lpDst;
	((LINE *)eax)->rpChars = edx;
	ecx = ((CHARS *)esi)->len;
	edx = ((CHARS *)esi)->max;
	ecx = ecx+sizeof(CHARS);
	edx = edx+sizeof(CHARS);
	edx -= ecx;
	temp1 = ecx;
	ecx /= 4;
	while(ecx > 0)
	{
		*(DWORD *)edi = *(DWORD *)esi;
		edi += 4;
		esi += 4;
		ecx--;
	}
	ecx = temp1;
	ecx &= 3;
	while(ecx > 0)
	{
		*(BYTE *)edi = *(BYTE *)esi;
		edi++;
		esi++;
		ecx--;
	}
	edi += edx;
	eax = eax+sizeof(LINE);
	if(eax!=lpLine)
	{
		goto anon_1;
	} // endif
	edi -= lpDst;
	((EDIT *)ebx)->rpCharsFree = edi;
	return eax;

} // GarbageCollection

REG_T ExpandCharMem(DWORD hMem, DWORD nLen)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	eax = nLen;
	eax >>= 12;
	eax++;
	eax <<= 12;
	eax += ((EDIT *)ebx)->rpCharsFree;
	eax += MAXCHARMEM;
	if(eax>((EDIT *)ebx)->cbChars)
	{
		esi = ((EDIT *)ebx)->hChars;
		edi = ((EDIT *)ebx)->cbChars;
		eax = nLen;
		eax >>= 12;
		eax++;
		eax <<= 12;
		eax += MAXCHARMEM;
		((EDIT *)ebx)->cbChars += eax;
		eax = HeapAlloc(((EDIT *)ebx)->hHeap, HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY, ((EDIT *)ebx)->cbChars);
		if(!eax)
		{
			((EDIT *)ebx)->cbChars = edi;
			eax = MessageBox(((EDIT *)ebx)->hwnd, &szMemFailChar, &szToolTip, MB_OK);
			eax = 0;
		}
		else
		{
			((EDIT *)ebx)->hChars = eax;
			eax = GarbageCollection(ebx, ((EDIT *)ebx)->hLine, esi, ((EDIT *)ebx)->hChars);
			eax = HeapFree(((EDIT *)ebx)->hHeap, 0, esi);
		} // endif
	} // endif
	return eax;

} // ExpandCharMem

REG_T ExpandUndoMem(DWORD hMem, DWORD cb)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	eax = ((EDIT *)ebx)->rpUndo;
	eax += cb;
	eax += 8*1024;
	eax >>= 12;
	eax++;
	eax <<= 12;
	if(eax>((EDIT *)ebx)->cbUndo)
	{
		esi = ((EDIT *)ebx)->hUndo;
		edi = ((EDIT *)ebx)->cbUndo;
		eax += MAXUNDOMEM;
		eax &= 0x0FFFFFF00;
		((EDIT *)ebx)->cbUndo = eax;
		eax = HeapAlloc(((EDIT *)ebx)->hHeap, HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY, ((EDIT *)ebx)->cbUndo);
		if(!eax)
		{
			((EDIT *)ebx)->cbUndo = edi;
			eax = MessageBox(((EDIT *)ebx)->hwnd, &szMemFailUndo, &szToolTip, MB_OK);
			eax = 0;
		}
		else
		{
			((EDIT *)ebx)->hUndo = eax;
			ecx = edi;
			edi = eax;
			temp1 = esi;
			ecx /= 4;
			while(ecx > 0)
			{
				*(DWORD *)edi = *(DWORD *)esi;
				edi += 4;
				esi += 4;
				ecx--;
			}
			esi = temp1;
			eax = HeapFree(((EDIT *)ebx)->hHeap, 0, esi);
		} // endif
	} // endif
	return eax;

} // ExpandUndoMem

REG_T ExpandWordMem(void)
{
	REG_T eax = 0, ecx, edx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	eax = cbWrdMem;
	eax -= rpWrdFree;
	if(eax<256)
	{
		esi = cbWrdMem;
		cbWrdMem += MAXWORDMEM;
		eax = GetProcessHeap();
		eax = HeapAlloc(eax, HEAP_ZERO_MEMORY, cbWrdMem);
		if(!eax)
		{
			eax = MessageBox(NULL, &szMemFailSyntax, &szToolTip, MB_OK);
			eax = 0;
		}
		else
		{
			ecx = esi;
			edi = eax;
			esi = hWrdMem;
			hWrdMem = edi;
			temp1 = esi;
			ecx /= 4;
			while(ecx > 0)
			{
				*(DWORD *)edi = *(DWORD *)esi;
				edi += 4;
				esi += 4;
				ecx--;
			}
			esi = temp1;
			eax = GetProcessHeap();
			eax = HeapFree(eax, 0, esi);
		} // endif
	} // endif
	return eax;

} // ExpandWordMem

