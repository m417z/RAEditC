#include "Memory.h"

REG_T xGlobalAlloc(DWORD t, DWORD s)
{
	REG_T eax = 0;

	s >>= 3;
	s++;
	s <<= 3;
	eax = GlobalAlloc(t, s);
	if(!eax)
	{
		eax = MessageBox(NULL, szGlobalFail, szToolTip, MB_OK);
		eax = 0;
	} // endif
	return eax;

} // xGlobalAlloc

REG_T xHeapAlloc(DWORD h, DWORD t, DWORD s)
{
	REG_T eax = 0;

	s >>= 3;
	s++;
	s <<= 3;
	eax = HeapAlloc(h, t, s);
	if(!eax)
	{
		eax = MessageBox(NULL, szHeapFail, szToolTip, MB_OK);
		eax = 0;
	} // endif
	return eax;

} // xHeapAlloc

REG_T ExpandLineMem(EDIT *pMem)
{
	REG_T eax = 0, ecx, ebx, esi, edi;
	REG_T temp1;

	eax = pMem->rpLineFree;
	eax += MAXLINEMEM;
	eax >>= 12;
	eax++;
	eax <<= 12;
	if(eax>pMem->cbLine)
	{
		esi = pMem->hLine;
		edi = pMem->cbLine;
		pMem->cbLine += MAXLINEMEM;
		eax = HeapAlloc(pMem->hHeap, HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY, pMem->cbLine);
		if(!eax)
		{
			pMem->cbLine = edi;
			eax = MessageBox(pMem->hwnd, szMemFailLine, szToolTip, MB_OK);
			eax = 0;
		}
		else
		{
			pMem->hLine = eax;
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
			eax = HeapFree(pMem->hHeap, 0, esi);
		} // endif
	} // endif
	return eax;

} // ExpandLineMem

REG_T GarbageCollection(DWORD lpEdit, DWORD lpLine, DWORD lpSrc, DWORD lpDst)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1;
	EDIT *pMem;

	eax = lpLine;
	pMem = lpEdit;
	ecx = pMem->rpLineFree;
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
	pMem->rpCharsFree = edi;
	return eax;

} // GarbageCollection

REG_T ExpandCharMem(EDIT *pMem, DWORD nLen)
{
	REG_T eax = 0, ebx, esi, edi;

	eax = nLen;
	eax >>= 12;
	eax++;
	eax <<= 12;
	eax += pMem->rpCharsFree;
	eax += MAXCHARMEM;
	if(eax>pMem->cbChars)
	{
		esi = pMem->hChars;
		edi = pMem->cbChars;
		eax = nLen;
		eax >>= 12;
		eax++;
		eax <<= 12;
		eax += MAXCHARMEM;
		pMem->cbChars += eax;
		eax = HeapAlloc(pMem->hHeap, HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY, pMem->cbChars);
		if(!eax)
		{
			pMem->cbChars = edi;
			eax = MessageBox(pMem->hwnd, szMemFailChar, szToolTip, MB_OK);
			eax = 0;
		}
		else
		{
			pMem->hChars = eax;
			eax = GarbageCollection(pMem, pMem->hLine, esi, pMem->hChars);
			eax = HeapFree(pMem->hHeap, 0, esi);
		} // endif
	} // endif
	return eax;

} // ExpandCharMem

REG_T ExpandUndoMem(EDIT *pMem, DWORD cb)
{
	REG_T eax = 0, ecx, ebx, esi, edi;
	REG_T temp1;

	eax = pMem->rpUndo;
	eax += cb;
	eax += 8*1024;
	eax >>= 12;
	eax++;
	eax <<= 12;
	if(eax>pMem->cbUndo)
	{
		esi = pMem->hUndo;
		edi = pMem->cbUndo;
		eax += MAXUNDOMEM;
		eax &= 0x0FFFFFF00;
		pMem->cbUndo = eax;
		eax = HeapAlloc(pMem->hHeap, HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY, pMem->cbUndo);
		if(!eax)
		{
			pMem->cbUndo = edi;
			eax = MessageBox(pMem->hwnd, szMemFailUndo, szToolTip, MB_OK);
			eax = 0;
		}
		else
		{
			pMem->hUndo = eax;
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
			eax = HeapFree(pMem->hHeap, 0, esi);
		} // endif
	} // endif
	return eax;

} // ExpandUndoMem

REG_T ExpandWordMem(void)
{
	REG_T eax = 0, ecx, esi, edi;
	REG_T temp1;

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
			eax = MessageBox(NULL, szMemFailSyntax, szToolTip, MB_OK);
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

