#include "Undo.h"

#include "Edit.h"
#include "Function.h"
#include "Memory.h"
#include "Position.h"

REG_T DoUndo(EDIT *pMem)
{
	REG_T eax = 0, ecx, edx, edi;
	REG_T temp1, temp2, temp3;
	DWORD undoid;

	edi = pMem->hUndo;
Nxt:
	edx = pMem->rpUndo;
	if(edx)
	{
		edx = ((RAUNDO *)(edi+edx))->rpPrev;
		pMem->rpUndo = edx;
		eax = ((RAUNDO *)(edi+edx))->undoid;
		undoid = eax;
		eax = ((RAUNDO *)(edi+edx))->cp;
		pMem->cpMin = eax;
		pMem->cpMax = eax;
		temp1 = edx;
		eax = SelChange(pMem, SEL_TEXT);
		edx = temp1;
		RBYTE_LOW(eax) = ((RAUNDO *)(edi+edx))->fun;
		if(RBYTE_LOW(eax)==UNDO_INSERT)
		{
			eax = ((RAUNDO *)(edi+edx))->cp;
			pMem->cpMin = eax;
			pMem->cpMax = eax;
			eax = DeleteChar(pMem, eax);
		}
		else if(RBYTE_LOW(eax)==UNDO_OVERWRITE)
		{
			eax = *(BYTE *)(edi+edx+sizeof(RAUNDO));
			if(RBYTE_LOW(eax)!=0x0D)
			{
				temp1 = pMem->fOvr;
				pMem->fOvr = TRUE;
				ecx = ((RAUNDO *)(edi+edx))->cp;
				pMem->cpMin = ecx;
				pMem->cpMax = ecx;
				temp2 = edx;
				eax = InsertChar(pMem, ecx, eax);
				edx = temp2;
				*(BYTE *)(edi+edx+sizeof(RAUNDO)) = RBYTE_LOW(eax);
				pMem->fOvr = temp1;
			}
			else
			{
				eax = ((RAUNDO *)(edi+edx))->cp;
				pMem->cpMin = eax;
				pMem->cpMax = eax;
				eax = DeleteChar(pMem, eax);
			} // endif
		}
		else if(RBYTE_LOW(eax)==UNDO_DELETE)
		{
			temp1 = pMem->fOvr;
			pMem->fOvr = FALSE;
			eax = *(BYTE *)(edi+edx+sizeof(RAUNDO));
			ecx = ((RAUNDO *)(edi+edx))->cp;
			pMem->cpMin = ecx;
			pMem->cpMax = ecx;
			eax = InsertChar(pMem, ecx, eax);
			pMem->fOvr = temp1;
		}
		else if(RBYTE_LOW(eax)==UNDO_BACKDELETE)
		{
			temp1 = pMem->fOvr;
			pMem->fOvr = FALSE;
			eax = *(BYTE *)(edi+edx+sizeof(RAUNDO));
			ecx = ((RAUNDO *)(edi+edx))->cp;
			pMem->cpMin = ecx;
			pMem->cpMax = ecx;
			eax = InsertChar(pMem, ecx, eax);
			pMem->cpMin++;
			pMem->cpMax++;
			pMem->fOvr = temp1;
		}
		else if(RBYTE_LOW(eax)==UNDO_INSERTBLOCK)
		{
			eax = ((RAUNDO *)(edi+edx))->cp;
			pMem->cpMin = eax;
			pMem->cpMax = eax;
			ecx = ((RAUNDO *)(edi+edx))->cb;
			while(ecx)
			{
				temp1 = ecx;
				eax = DeleteChar(pMem, pMem->cpMin);
				ecx = temp1;
				ecx--;
			} // endw
		}
		else if(RBYTE_LOW(eax)==UNDO_DELETEBLOCK)
		{
			temp1 = pMem->fOvr;
			pMem->fOvr = FALSE;
			ecx = ((RAUNDO *)(edi+edx))->cp;
			pMem->cpMin = ecx;
			pMem->cpMax = ecx;
			ecx = ((RAUNDO *)(edi+edx))->cb;
			edx += sizeof(RAUNDO);
			while(ecx)
			{
				temp2 = ecx;
				temp3 = edx;
				eax = *(BYTE *)(edi+edx);
				eax = InsertChar(pMem, pMem->cpMin, eax);
				pMem->cpMin++;
				edx = temp3;
				ecx = temp2;
				edx++;
				ecx--;
			} // endw
			pMem->fOvr = temp1;
		} // endif
		edx = pMem->rpUndo;
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

REG_T DoRedo(EDIT *pMem)
{
	REG_T eax = 0, ecx, edx, edi;
	REG_T temp1, temp2, temp3;
	DWORD undoid;

	edi = pMem->hUndo;
Nxt:
	edx = pMem->rpUndo;
	eax = ((RAUNDO *)(edi+edx))->cb;
	if(eax)
	{
		eax = ((RAUNDO *)(edi+edx))->undoid;
		undoid = eax;
		eax = ((RAUNDO *)(edi+edx))->cp;
		pMem->cpMin = eax;
		pMem->cpMax = eax;
		temp1 = edx;
		eax = SelChange(pMem, SEL_TEXT);
		edx = temp1;
		RBYTE_LOW(eax) = ((RAUNDO *)(edi+edx))->fun;
		if(RBYTE_LOW(eax)==UNDO_INSERT)
		{
			temp1 = pMem->fOvr;
			pMem->fOvr = FALSE;
			ecx = ((RAUNDO *)(edi+edx))->cp;
			pMem->cpMin = ecx;
			pMem->cpMax = ecx;
			eax = *(BYTE *)(edi+edx+sizeof(RAUNDO));
			edx += sizeof(RAUNDO)+1;
			pMem->rpUndo = edx;
			eax = InsertChar(pMem, ecx, eax);
			pMem->cpMin++;
			pMem->cpMax++;
			pMem->fOvr = temp1;
		}
		else if(RBYTE_LOW(eax)==UNDO_OVERWRITE)
		{
			eax = *(BYTE *)(edi+edx+sizeof(RAUNDO));
			if(RBYTE_LOW(eax)!=0x0D)
			{
				temp1 = pMem->fOvr;
				pMem->fOvr = TRUE;
				ecx = ((RAUNDO *)(edi+edx))->cp;
				pMem->cpMin = ecx;
				pMem->cpMax = ecx;
				temp2 = edx;
				eax = InsertChar(pMem, ecx, eax);
				edx = temp2;
				*(BYTE *)(edi+edx+sizeof(RAUNDO)) = RBYTE_LOW(eax);
				pMem->fOvr = temp1;
				edx += sizeof(RAUNDO)+1;
				pMem->rpUndo = edx;
				pMem->cpMin++;
				pMem->cpMax++;
			}
			else
			{
				ecx = ((RAUNDO *)(edi+edx))->cp;
				edx += sizeof(RAUNDO)+1;
				pMem->rpUndo = edx;
				pMem->cpMin = ecx;
				pMem->cpMax = ecx;
				eax = InsertChar(pMem, ecx, eax);
				pMem->cpMin++;
				pMem->cpMax++;
			} // endif
		}
		else if(RBYTE_LOW(eax)==UNDO_DELETE)
		{
			ecx = ((RAUNDO *)(edi+edx))->cp;
			pMem->cpMin = ecx;
			pMem->cpMax = ecx;
			edx += sizeof(RAUNDO)+1;
			pMem->rpUndo = edx;
			eax = DeleteChar(pMem, ecx);
		}
		else if(RBYTE_LOW(eax)==UNDO_BACKDELETE)
		{
			ecx = ((RAUNDO *)(edi+edx))->cp;
			pMem->cpMin = ecx;
			pMem->cpMax = ecx;
			edx += sizeof(RAUNDO)+1;
			pMem->rpUndo = edx;
			eax = DeleteChar(pMem, ecx);
		}
		else if(RBYTE_LOW(eax)==UNDO_INSERTBLOCK)
		{
			temp1 = pMem->fOvr;
			pMem->fOvr = FALSE;
			ecx = ((RAUNDO *)(edi+edx))->cp;
			pMem->cpMin = ecx;
			pMem->cpMax = ecx;
			ecx = ((RAUNDO *)(edi+edx))->cb;
			edx += sizeof(RAUNDO);
			while(ecx)
			{
				temp2 = ecx;
				temp3 = edx;
				eax = *(BYTE *)(edi+edx);
				eax = InsertChar(pMem, pMem->cpMin, eax);
				pMem->cpMin++;
				pMem->cpMax++;
				edx = temp3;
				ecx = temp2;
				edx++;
				ecx--;
			} // endw
			pMem->rpUndo = edx;
			pMem->fOvr = temp1;
		}
		else if(RBYTE_LOW(eax)==UNDO_DELETEBLOCK)
		{
			eax = ((RAUNDO *)(edi+edx))->cp;
			pMem->cpMin = eax;
			pMem->cpMax = eax;
			ecx = ((RAUNDO *)(edi+edx))->cb;
			edx += ecx;
			edx += sizeof(RAUNDO);
			pMem->rpUndo = edx;
			while(ecx)
			{
				temp1 = ecx;
				eax = DeleteChar(pMem, pMem->cpMin);
				ecx = temp1;
				ecx--;
			} // endw
		} // endif
		edx = pMem->rpUndo;
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

REG_T SaveUndo(EDIT *pMem, DWORD nFun, DWORD cp, REG_T lp, REG_T cb)
{
	REG_T eax = 0, ecx, edx, esi, edi;
	REG_T temp1;

	if(!fNoSaveUndo)
	{
		eax = ExpandUndoMem(pMem, cb);
		edi = pMem->hUndo;
		edx = pMem->rpUndo;
		eax = nFun;
		if(eax==UNDO_INSERT || eax==UNDO_OVERWRITE || eax==UNDO_DELETE || eax==UNDO_BACKDELETE)
		{
			((RAUNDO *)(edi+edx))->fun = RBYTE_LOW(eax);
			eax = pMem->lockundoid;
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
			pMem->rpUndo = edx;
		}
		else if(eax==UNDO_INSERTBLOCK || eax==UNDO_DELETEBLOCK)
		{
			((RAUNDO *)(edi+edx))->fun = RBYTE_LOW(eax);
			eax = pMem->lockundoid;
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
			pMem->rpUndo = edx;
		} // endif
	} // endif
	return eax;

} // SaveUndo

REG_T Undo(RAEDT *raedt, EDIT *pMem, HWND hWin)
{
	REG_T eax = 0, ecx, edx, esi;

    esi = raedt;
	if(!(pMem->nMode&MODE_BLOCK))
	{
		eax = DoUndo(pMem);
	}
	else
	{
		eax = DoUndo(pMem);
		eax = SetBlockFromCp(pMem, pMem->cpMin, FALSE);
	} // endif
	eax = GetCharPtr(pMem, pMem->cpMin, &ecx, &edx);
	eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
	eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
	eax = InvalidateEdit(pMem, pMem->edta.hwnd);
	eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
	eax = SetCpxMax(pMem, hWin);
	eax = SelChange(pMem, SEL_TEXT);
	return eax;

} // Undo

REG_T Redo(RAEDT *raedt, EDIT *pMem, HWND hWin)
{
	REG_T eax = 0, ecx, edx, esi;

    esi = raedt;
	if(!(pMem->nMode&MODE_BLOCK))
	{
		eax = DoRedo(pMem);
	}
	else
	{
		eax = DoRedo(pMem);
		eax = SetBlockFromCp(pMem, pMem->cpMin, FALSE);
	} // endif
	eax = GetCharPtr(pMem, pMem->cpMin, &ecx, &edx);
	eax = SetCaretVisible(hWin, ((RAEDT *)esi)->cpy);
	eax = SetCaret(pMem, ((RAEDT *)esi)->cpy);
	eax = InvalidateEdit(pMem, pMem->edta.hwnd);
	eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
	eax = SetCpxMax(pMem, hWin);
	eax = SelChange(pMem, SEL_TEXT);
	return eax;

} // Redo

REG_T GetUndo(EDIT *pMem, DWORD nCount, REG_T lpMem)
{
	REG_T eax = 0, ecx, edx, esi, edi;
	REG_T rpstart;
	REG_T rpend;

	auto void GetHeader(void);
	auto void GetData(void);

	esi = pMem->hUndo;
	edx = pMem->rpUndo;
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
		eax = pMem->rpUndo;
		eax -= rpstart;
		*(REG_T *)edi = eax;
		edi += sizeof(REG_T);
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

REG_T SetUndo(EDIT *pMem, REG_T nSize, REG_T lpMem)
{
	REG_T eax = 0, ecx, edx, esi, edi;

	eax = ExpandUndoMem(pMem, nSize);
	esi = lpMem;
	edi = pMem->hUndo;
	eax = *(REG_T *)esi;
	pMem->rpUndo = eax;
	esi += sizeof(REG_T);
	nSize -= sizeof(REG_T);
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

