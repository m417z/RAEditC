#include <windows.h>
#include "Data.h"

REG_T DoUndo(DWORD hMem)
{
	REG_T eax = 0, ecx, edx, ebx, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD undoid;

	ebx = hMem;
	edi = ((EDIT *)ebx)->hUndo;
Nxt:
	edx = ((EDIT *)ebx)->rpUndo;
	if(edx)
	{
		edx = ((RAUNDO *)(edi+edx))->rpPrev;
		((EDIT *)ebx)->rpUndo = edx;
		eax = ((RAUNDO *)(edi+edx))->undoid;
		undoid = eax;
		eax = ((RAUNDO *)(edi+edx))->cp;
		((EDIT *)ebx)->cpMin = eax;
		((EDIT *)ebx)->cpMax = eax;
		temp1 = edx;
		eax = SelChange(ebx, SEL_TEXT);
		edx = temp1;
		RBYTE_LOW(eax) = ((RAUNDO *)(edi+edx))->fun;
		if(RBYTE_LOW(eax)==UNDO_INSERT)
		{
			eax = ((RAUNDO *)(edi+edx))->cp;
			((EDIT *)ebx)->cpMin = eax;
			((EDIT *)ebx)->cpMax = eax;
			eax = DeleteChar(ebx, eax);
		}
		else if(RBYTE_LOW(eax)==UNDO_OVERWRITE)
		{
			eax = *(BYTE *)(edi+edx+sizeof(RAUNDO));
			if(RBYTE_LOW(eax)!=0x0D)
			{
				temp1 = ((EDIT *)ebx)->fOvr;
				((EDIT *)ebx)->fOvr = TRUE;
				ecx = ((RAUNDO *)(edi+edx))->cp;
				((EDIT *)ebx)->cpMin = ecx;
				((EDIT *)ebx)->cpMax = ecx;
				temp2 = edx;
				eax = InsertChar(ebx, ecx, eax);
				edx = temp2;
				*(BYTE *)(edi+edx+sizeof(RAUNDO)) = RBYTE_LOW(eax);
				((EDIT *)ebx)->fOvr = temp1;
			}
			else
			{
				eax = ((RAUNDO *)(edi+edx))->cp;
				((EDIT *)ebx)->cpMin = eax;
				((EDIT *)ebx)->cpMax = eax;
				eax = DeleteChar(ebx, eax);
			} // endif
		}
		else if(RBYTE_LOW(eax)==UNDO_DELETE)
		{
			temp1 = ((EDIT *)ebx)->fOvr;
			((EDIT *)ebx)->fOvr = FALSE;
			eax = *(BYTE *)(edi+edx+sizeof(RAUNDO));
			ecx = ((RAUNDO *)(edi+edx))->cp;
			((EDIT *)ebx)->cpMin = ecx;
			((EDIT *)ebx)->cpMax = ecx;
			eax = InsertChar(ebx, ecx, eax);
			((EDIT *)ebx)->fOvr = temp1;
		}
		else if(RBYTE_LOW(eax)==UNDO_BACKDELETE)
		{
			temp1 = ((EDIT *)ebx)->fOvr;
			((EDIT *)ebx)->fOvr = FALSE;
			eax = *(BYTE *)(edi+edx+sizeof(RAUNDO));
			ecx = ((RAUNDO *)(edi+edx))->cp;
			((EDIT *)ebx)->cpMin = ecx;
			((EDIT *)ebx)->cpMax = ecx;
			eax = InsertChar(ebx, ecx, eax);
			((EDIT *)ebx)->cpMin++;
			((EDIT *)ebx)->cpMax++;
			((EDIT *)ebx)->fOvr = temp1;
		}
		else if(RBYTE_LOW(eax)==UNDO_INSERTBLOCK)
		{
			eax = ((RAUNDO *)(edi+edx))->cp;
			((EDIT *)ebx)->cpMin = eax;
			((EDIT *)ebx)->cpMax = eax;
			ecx = ((RAUNDO *)(edi+edx))->cb;
			while(ecx)
			{
				temp1 = ecx;
				eax = DeleteChar(ebx, ((EDIT *)ebx)->cpMin);
				ecx = temp1;
				ecx--;
			} // endw
		}
		else if(RBYTE_LOW(eax)==UNDO_DELETEBLOCK)
		{
			temp1 = ((EDIT *)ebx)->fOvr;
			((EDIT *)ebx)->fOvr = FALSE;
			ecx = ((RAUNDO *)(edi+edx))->cp;
			((EDIT *)ebx)->cpMin = ecx;
			((EDIT *)ebx)->cpMax = ecx;
			ecx = ((RAUNDO *)(edi+edx))->cb;
			edx += sizeof(RAUNDO);
			while(ecx)
			{
				temp2 = ecx;
				temp3 = edx;
				eax = *(BYTE *)(edi+edx);
				eax = InsertChar(ebx, ((EDIT *)ebx)->cpMin, eax);
				((EDIT *)ebx)->cpMin++;
				edx = temp3;
				ecx = temp2;
				edx++;
				ecx--;
			} // endw
			((EDIT *)ebx)->fOvr = temp1;
		} // endif
		edx = ((EDIT *)ebx)->rpUndo;
		if(edx)
		{
			edx = ((RAUNDO *)(edi+edx))->rpPrev;
			eax = undoid;
			if(eax==((RAUNDO *)(edi+edx))->undoid)
			{
				goto Nxt;
			} // endif
		} // endif
	} // endif
	return eax;

} // DoUndo

REG_T DoRedo(DWORD hMem)
{
	REG_T eax = 0, ecx, edx, ebx, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD undoid;

	ebx = hMem;
	edi = ((EDIT *)ebx)->hUndo;
Nxt:
	edx = ((EDIT *)ebx)->rpUndo;
	eax = ((RAUNDO *)(edi+edx))->cb;
	if(eax)
	{
		eax = ((RAUNDO *)(edi+edx))->undoid;
		undoid = eax;
		eax = ((RAUNDO *)(edi+edx))->cp;
		((EDIT *)ebx)->cpMin = eax;
		((EDIT *)ebx)->cpMax = eax;
		temp1 = edx;
		eax = SelChange(ebx, SEL_TEXT);
		edx = temp1;
		RBYTE_LOW(eax) = ((RAUNDO *)(edi+edx))->fun;
		if(RBYTE_LOW(eax)==UNDO_INSERT)
		{
			temp1 = ((EDIT *)ebx)->fOvr;
			((EDIT *)ebx)->fOvr = FALSE;
			ecx = ((RAUNDO *)(edi+edx))->cp;
			((EDIT *)ebx)->cpMin = ecx;
			((EDIT *)ebx)->cpMax = ecx;
			eax = *(BYTE *)(edi+edx+sizeof(RAUNDO));
			edx += sizeof(RAUNDO)+1;
			((EDIT *)ebx)->rpUndo = edx;
			eax = InsertChar(ebx, ecx, eax);
			((EDIT *)ebx)->cpMin++;
			((EDIT *)ebx)->cpMax++;
			((EDIT *)ebx)->fOvr = temp1;
		}
		else if(RBYTE_LOW(eax)==UNDO_OVERWRITE)
		{
			eax = *(BYTE *)(edi+edx+sizeof(RAUNDO));
			if(RBYTE_LOW(eax)!=0x0D)
			{
				temp1 = ((EDIT *)ebx)->fOvr;
				((EDIT *)ebx)->fOvr = TRUE;
				ecx = ((RAUNDO *)(edi+edx))->cp;
				((EDIT *)ebx)->cpMin = ecx;
				((EDIT *)ebx)->cpMax = ecx;
				temp2 = edx;
				eax = InsertChar(ebx, ecx, eax);
				edx = temp2;
				*(BYTE *)(edi+edx+sizeof(RAUNDO)) = RBYTE_LOW(eax);
				((EDIT *)ebx)->fOvr = temp1;
				edx += sizeof(RAUNDO)+1;
				((EDIT *)ebx)->rpUndo = edx;
				((EDIT *)ebx)->cpMin++;
				((EDIT *)ebx)->cpMax++;
			}
			else
			{
				ecx = ((RAUNDO *)(edi+edx))->cp;
				edx += sizeof(RAUNDO)+1;
				((EDIT *)ebx)->rpUndo = edx;
				((EDIT *)ebx)->cpMin = ecx;
				((EDIT *)ebx)->cpMax = ecx;
				eax = InsertChar(ebx, ecx, eax);
				((EDIT *)ebx)->cpMin++;
				((EDIT *)ebx)->cpMax++;
			} // endif
		}
		else if(RBYTE_LOW(eax)==UNDO_DELETE)
		{
			ecx = ((RAUNDO *)(edi+edx))->cp;
			((EDIT *)ebx)->cpMin = ecx;
			((EDIT *)ebx)->cpMax = ecx;
			edx += sizeof(RAUNDO)+1;
			((EDIT *)ebx)->rpUndo = edx;
			eax = DeleteChar(ebx, ecx);
		}
		else if(RBYTE_LOW(eax)==UNDO_BACKDELETE)
		{
			ecx = ((RAUNDO *)(edi+edx))->cp;
			((EDIT *)ebx)->cpMin = ecx;
			((EDIT *)ebx)->cpMax = ecx;
			edx += sizeof(RAUNDO)+1;
			((EDIT *)ebx)->rpUndo = edx;
			eax = DeleteChar(ebx, ecx);
		}
		else if(RBYTE_LOW(eax)==UNDO_INSERTBLOCK)
		{
			temp1 = ((EDIT *)ebx)->fOvr;
			((EDIT *)ebx)->fOvr = FALSE;
			ecx = ((RAUNDO *)(edi+edx))->cp;
			((EDIT *)ebx)->cpMin = ecx;
			((EDIT *)ebx)->cpMax = ecx;
			ecx = ((RAUNDO *)(edi+edx))->cb;
			edx += sizeof(RAUNDO);
			while(ecx)
			{
				temp2 = ecx;
				temp3 = edx;
				eax = *(BYTE *)(edi+edx);
				eax = InsertChar(ebx, ((EDIT *)ebx)->cpMin, eax);
				((EDIT *)ebx)->cpMin++;
				((EDIT *)ebx)->cpMax++;
				edx = temp3;
				ecx = temp2;
				edx++;
				ecx--;
			} // endw
			((EDIT *)ebx)->rpUndo = edx;
			((EDIT *)ebx)->fOvr = temp1;
		}
		else if(RBYTE_LOW(eax)==UNDO_DELETEBLOCK)
		{
			eax = ((RAUNDO *)(edi+edx))->cp;
			((EDIT *)ebx)->cpMin = eax;
			((EDIT *)ebx)->cpMax = eax;
			ecx = ((RAUNDO *)(edi+edx))->cb;
			edx += ecx;
			edx += sizeof(RAUNDO);
			((EDIT *)ebx)->rpUndo = edx;
			while(ecx)
			{
				temp1 = ecx;
				eax = DeleteChar(ebx, ((EDIT *)ebx)->cpMin);
				ecx = temp1;
				ecx--;
			} // endw
		} // endif
		edx = ((EDIT *)ebx)->rpUndo;
		if(edx)
		{
			eax = undoid;
			if(eax==((RAUNDO *)(edi+edx))->undoid)
			{
				goto Nxt;
			} // endif
		} // endif
	} // endif
	return eax;

} // DoRedo

REG_T SaveUndo(DWORD hMem, DWORD nFun, DWORD cp, DWORD lp, DWORD cb)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	if(!fNoSaveUndo)
	{
		ebx = hMem;
		eax = ExpandUndoMem(ebx, cb);
		edi = ((EDIT *)ebx)->hUndo;
		edx = ((EDIT *)ebx)->rpUndo;
		eax = nFun;
		if(eax==UNDO_INSERT || eax==UNDO_OVERWRITE || eax==UNDO_DELETE || eax==UNDO_BACKDELETE)
		{
			((RAUNDO *)(edi+edx))->fun = RBYTE_LOW(eax);
			eax = ((EDIT *)ebx)->lockundoid;
			if(!eax)
			{
				eax = nUndoid;
			} // endif
			((RAUNDO *)(edi+edx))->undoid = eax;
			eax = cp;
			((RAUNDO *)(edi+edx))->cp = eax;
			((RAUNDO *)(edi+edx))->cb = 1;
			eax = lp;
			*(BYTE *)(edi+edx+sizeof(RAUNDO)) = RBYTE_LOW(eax);
			eax = edx;
			edx += sizeof(RAUNDO)+1;
			((RAUNDO *)(edi+edx))->rpPrev = eax;
			eax = 0;
			((RAUNDO *)(edi+edx))->cp = eax;
			((RAUNDO *)(edi+edx))->cb = eax;
			((RAUNDO *)(edi+edx))->fun = RBYTE_LOW(eax);
			((EDIT *)ebx)->rpUndo = edx;
		}
		else if(eax==UNDO_INSERTBLOCK || eax==UNDO_DELETEBLOCK)
		{
			((RAUNDO *)(edi+edx))->fun = RBYTE_LOW(eax);
			eax = ((EDIT *)ebx)->lockundoid;
			if(!eax)
			{
				eax = nUndoid;
			} // endif
			((RAUNDO *)(edi+edx))->undoid = eax;
			eax = cp;
			((RAUNDO *)(edi+edx))->cp = eax;
			ecx = cb;
			((RAUNDO *)(edi+edx))->cb = ecx;
			esi = lp;
			temp1 = edx;
			edx += sizeof(RAUNDO);
			while(ecx)
			{
				RBYTE_LOW(eax) = *(BYTE *)esi;
				esi++;
				if(RBYTE_LOW(eax)!=0x0A)
				{
					*(BYTE *)(edi+edx) = RBYTE_LOW(eax);
					edx++;
					ecx--;
				} // endif
			} // endw
			eax = temp1;
			((RAUNDO *)(edi+edx))->rpPrev = eax;
			eax = 0;
			((RAUNDO *)(edi+edx))->cp = eax;
			((RAUNDO *)(edi+edx))->cb = eax;
			((RAUNDO *)(edi+edx))->fun = RBYTE_LOW(eax);
			((EDIT *)ebx)->rpUndo = edx;
		} // endif
	} // endif
	return eax;

} // SaveUndo

REG_T Undo(DWORD hMem, DWORD hWin)
{
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	POINT pt;

	ebx = hMem;
	if(!(((EDIT *)ebx)->nMode&MODE_BLOCK))
	{
		eax = DoUndo(ebx);
	}
	else
	{
		eax = DoUndo(ebx);
		eax = SetBlockFromCp(ebx, ((EDIT *)ebx)->cpMin, FALSE);
	} // endif
	eax = GetCharPtr(ebx, ((EDIT *)ebx)->cpMin);
	eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
	eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
	eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
	eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
	eax = SetCpxMax(ebx, hWin);
	eax = SelChange(ebx, SEL_TEXT);
	return eax;

} // Undo

REG_T Redo(DWORD hMem, DWORD hWin)
{
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	POINT pt;
	RECT oldrects[2];

	ebx = hMem;
	if(!(((EDIT *)ebx)->nMode&MODE_BLOCK))
	{
		eax = DoRedo(ebx);
	}
	else
	{
		eax = DoRedo(ebx);
		eax = SetBlockFromCp(ebx, ((EDIT *)ebx)->cpMin, FALSE);
	} // endif
	eax = GetCharPtr(ebx, ((EDIT *)ebx)->cpMin);
	eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
	eax = SetCaret(ebx, ((RAEDT *)esi)->cpy);
	eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
	eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
	eax = SetCpxMax(ebx, hWin);
	eax = SelChange(ebx, SEL_TEXT);
	return eax;

} // Redo

REG_T GetUndo(DWORD hMem, DWORD nCount, DWORD lpMem)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD rpstart;
	DWORD rpend;

	auto void GetHeader(void);
	auto void GetData(void);

	ebx = hMem;
	esi = ((EDIT *)ebx)->hUndo;
	edx = ((EDIT *)ebx)->rpUndo;
	// Include redo
	while(((RAUNDO *)(esi+edx))->cb)
	{
		eax = ((RAUNDO *)(esi+edx))->cb;
		edx = edx+eax+sizeof(RAUNDO);
	} // endw
	rpend = edx;
	ecx = nCount;
	if(!ecx)
	{
		ecx--;
	} // endif
	// Include undo
	while(edx!=0 && ecx!=0)
	{
		eax = ((RAUNDO *)(esi+edx))->cb;
		edx = ((RAUNDO *)(esi+edx))->rpPrev;
		ecx--;
	} // endw
	rpstart = edx;
	edi = lpMem;
	if(edi)
	{
		eax = ((EDIT *)ebx)->rpUndo;
		eax -= rpstart;
		*(DWORD *)edi = eax;
		edi = edi+4;
		while(edx<rpend)
		{
			GetHeader();
			GetData();
			ecx = ((RAUNDO *)(esi+edx))->cb;
			edi = edi+ecx+sizeof(RAUNDO);
			edx = edx+ecx+sizeof(RAUNDO);
		} // endw
		GetHeader();
		((RAUNDO *)edi)->undoid = 0;
		((RAUNDO *)edi)->cp = 0;
		((RAUNDO *)edi)->cb = 0;
		((RAUNDO *)edi)->fun = 0;
	} // endif
	eax = rpend;
	eax -= rpstart;
	eax += sizeof(RAUNDO)+4;
	return eax;

	void GetHeader(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		ecx = 0;
		while(ecx<sizeof(RAUNDO))
		{
			eax = edx+ecx;
			RBYTE_LOW(eax) = *(BYTE *)(esi+eax);
			*(BYTE *)(edi+ecx) = RBYTE_LOW(eax);
			ecx++;
		} // endw
		eax = ((RAUNDO *)edi)->rpPrev;
		eax -= rpstart;
		((RAUNDO *)edi)->rpPrev = eax;
		return;

	}

	void GetData(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		ecx = 0;
		while(ecx<((RAUNDO *)(esi+edx))->cb)
		{
			eax = edx+ecx;
			RBYTE_LOW(eax) = *(BYTE *)(esi+eax+sizeof(RAUNDO));
			*(BYTE *)(edi+ecx+sizeof(RAUNDO)) = RBYTE_LOW(eax);
			ecx++;
		} // endw
		return;

	}

} // GetUndo

REG_T SetUndo(DWORD hMem, DWORD nSize, DWORD lpMem)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	eax = ExpandUndoMem(ebx, nSize);
	esi = lpMem;
	edi = ((EDIT *)ebx)->hUndo;
	eax = *(DWORD *)esi;
	((EDIT *)ebx)->rpUndo = eax;
	esi = esi+4;
	nSize -= 4;
	eax = RtlMoveMemory(edi, esi, nSize);
	ecx = ((RAUNDO *)edi)->undoid;
	edx = nUndoid;
	while(((RAUNDO *)edi)->cb)
	{
		if(ecx!=((RAUNDO *)edi)->undoid)
		{
			ecx = ((RAUNDO *)edi)->undoid;
			edx++;
		} // endif
		((RAUNDO *)edi)->undoid = edx;
		eax = ((RAUNDO *)edi)->cb;
		edi = edi+eax+sizeof(RAUNDO);
	} // endw
	edx++;
	nUndoid = edx;
	return eax;

} // SetUndo

