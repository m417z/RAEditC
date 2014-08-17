#include <windows.h>
#include "Data.h"
/*
REG_T IsEqualGUID(REG_T rguid1, REG_T rguid2)
{
	REG_T eax = 0, ecx, edx, ecx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	eax = 0;
	esi = rguid1;
	edi = rguid2;
	ecx = sizeof(GUID)/4;
	eax = memcmp(esi, edi, ecx*4) == 0;
	return eax;

} // IsEqualGUID
*/
// IDropTarget methods
REG_T IDropTarget_QueryInterface(REG_T pthis, REG_T iid, REG_T ppvObject)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	// PrintText 'IDropTarget_QueryInterface'
	eax = IsEqualGUID(iid, &IID_IDropTarget);
	if(!eax)
	{
		eax = IsEqualGUID(iid, &IID_IUnknown);
	} // endif
	edx = ppvObject;
	if(eax)
	{
		eax = pthis;
		*(DWORD *)edx = eax;
		edx = *(DWORD *)eax;
		eax = ((IDropTarget *)edx)->lpVtbl->AddRef(eax);
		eax = S_OK;
	}
	else
	{
		*(DWORD *)edx = 0;
		eax = E_NOINTERFACE;
	} // endif
	return eax;

} // IDropTarget_QueryInterface

REG_T IDropTarget_AddRef(REG_T pthis)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	// PrintText 'IDropTarget_AddRef'
	eax = pthis;
	edx = *(DWORD *)eax;
	((IDropTarget *)edx)->refcount++;
	eax = ((IDropTarget *)edx)->refcount;
	return eax;

} // IDropTarget_AddRef

REG_T IDropTarget_Release(REG_T pthis)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	// PrintText 'IDropTarget_Release'
	eax = pthis;
	edx = *(DWORD *)eax;
	if(((IDropTarget *)edx)->refcount)
	{
		((IDropTarget *)edx)->refcount--;
	} // endif
	eax = ((IDropTarget *)edx)->refcount;
	return eax;

} // IDropTarget_Release

REG_T IDropTarget_DragEnter(REG_T pthis, REG_T lpDataObject, REG_T grfKeyState, POINT pt, REG_T lpdwEffect)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	STGMEDIUM medium;
	FORMATETC fmte;

	// PrintText 'IDropTarget_DragEnter'
	esi = lpDataObject;
	ebx = lpdwEffect;
	edi = pthis;
	edi = *(DWORD *)edi;

	*(DWORD *)ebx = DROPEFFECT_NONE;
	((IDropTarget *)edi)->valid = FALSE;
	eax = E_INVALIDARG;
	if(esi)
	{
		eax = WindowFromPoint(pt.x, pt.y);
		temp1 = eax;
		edx = eax;
		eax = ScreenToClient(edx, &pt);
		eax = temp1;
		eax = ChildWindowFromPoint(eax, pt.x, pt.y);
		ebx = eax;
		eax = GetWindowLong(ebx, 0);
		if(eax)
		{
			((IDropTarget *)edi)->hwnd = ebx;
			ebx = eax;
			if(!(((EDIT *)ebx)->fstyle & STYLE_READONLY))
			{
				fmte.cfFormat = CF_TEXT;
				fmte.lptd = NULL;
				fmte.dwAspect = DVASPECT_CONTENT;
				fmte.lindex = -1;
				fmte.tymed = TYMED_HGLOBAL;
				edx = *(DWORD *)esi;
				eax = ((IDataObject *)edx)->GetData(esi, &fmte, &medium);
				if(eax==S_OK)
				{
					ebx = lpdwEffect;
					if(grfKeyState & MK_CONTROL)
					{
						*(DWORD *)ebx = DROPEFFECT_COPY;
					}
					else
					{
						*(DWORD *)ebx = DROPEFFECT_MOVE;
					} // endif
					((IDropTarget *)edi)->valid = TRUE;
					eax = medium.pUnkForRelease;
					if(eax)
					{
						edx = *(DWORD *)eax;
						eax = ((IDataObject *)edx)->lpVtbl->Release(eax);
					}
					else
					{
						eax = GlobalFree(medium.hGlobal);
					} // endif
				} // endif
			} // endif
		} // endif
		eax = S_OK;
	} // endif
	return eax;

} // IDropTarget_DragEnter

REG_T IDropTarget_DragOver(REG_T pthis, REG_T grfKeyState, POINT pt, REG_T lpdwEffect)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	RECT rect;

	// PrintText 'IDropTarget_DragOver'
	edi = pthis;
	edi = *(DWORD *)edi;
	edx = lpdwEffect;
	*(DWORD *)edx = DROPEFFECT_NONE;
	if(((IDropTarget *)edi)->valid)
	{
		eax = WindowFromPoint(pt.x, pt.y);
		temp1 = eax;
		edx = eax;
		eax = ScreenToClient(edx, &pt);
		eax = temp1;
		eax = ChildWindowFromPoint(eax, pt.x, pt.y);
		ebx = eax;
		eax = GetWindowLong(ebx, 0);
		if(eax)
		{
			((IDropTarget *)edi)->hwnd = ebx;
			ebx = eax;
			eax = SetFocus(((IDropTarget *)edi)->hwnd);
			eax = ((IDropTarget *)edi)->hwnd;
			if(eax==((EDIT *)ebx)->edta.hwnd)
			{
				esi = &((EDIT *)ebx)->edta;
			}
			else
			{
				esi = &((EDIT *)ebx)->edtb;
			} // endif
			eax = GetClientRect(((IDropTarget *)edi)->hwnd, &rect);
			ecx = ((EDIT *)ebx)->fntinfo.fntht;
			ecx /= 1;
			eax = pt.y;
			edx = eax+ecx;
			if(eax<ecx)
			{
				eax = SendMessage(((IDropTarget *)edi)->hwnd, WM_VSCROLL, SB_LINEUP, ((RAEDT *)esi)->hvscroll);
			}
			else if(edx>rect.bottom)
			{
				eax = SendMessage(((IDropTarget *)edi)->hwnd, WM_VSCROLL, SB_LINEDOWN, ((RAEDT *)esi)->hvscroll);
			}
			else
			{
				eax = pt.x;
				edx = eax+32;
				ecx = ((EDIT *)ebx)->selbarwt;
				ecx += ((EDIT *)ebx)->linenrwt;
				if(eax<ecx)
				{
					eax = SendMessage(((IDropTarget *)edi)->hwnd, WM_HSCROLL, SB_LINEUP, ((EDIT *)ebx)->hhscroll);
				}
				else if(edx>rect.right)
				{
					eax = SendMessage(((IDropTarget *)edi)->hwnd, WM_HSCROLL, SB_LINEDOWN, ((EDIT *)ebx)->hhscroll);
				} // endif
			} // endif
			eax = GetCharFromPos(ebx, ((RAEDT *)esi)->cpy, pt.x, pt.y);
			edx = eax;
			((IDropTarget *)edi)->cp = eax;
			eax = GetPosFromChar(ebx, edx, &pt);
			eax = pt.x;
			eax -= ((EDIT *)ebx)->cpx;
			edx = pt.y;
			edx -= ((RAEDT *)esi)->cpy;
			eax = SetCaretPos(eax, edx);
			eax = ShowCaret(((IDropTarget *)edi)->hwnd);
			((EDIT *)ebx)->fCaretHide = FALSE;
			edx = lpdwEffect;
			if(grfKeyState & MK_CONTROL)
			{
				*(DWORD *)edx = DROPEFFECT_COPY;
			}
			else
			{
				*(DWORD *)edx = DROPEFFECT_MOVE;
			} // endif
		} // endif
	} // endif
	eax = S_OK;
	return eax;

} // IDropTarget_DragOver

REG_T IDropTarget_DragLeave(REG_T pthis)
{
	REG_T eax = 0, ecx, edx, ebx, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	// PrintText 'IDropTarget_DragLeave'
	edi = pthis;
	edi = *(DWORD *)edi;
	if(((IDropTarget *)edi)->hwnd)
	{
		eax = GetWindowLong(((IDropTarget *)edi)->hwnd, 0);
		if(eax)
		{
			ebx = eax;
			if(!((EDIT *)ebx)->fCaretHide)
			{
				eax = HideCaret(((IDropTarget *)edi)->hwnd);
				((EDIT *)ebx)->fCaretHide = TRUE;
			} // endif
		} // endif
		((IDropTarget *)edi)->hwnd = 0;
	} // endif
	eax = S_OK;
	return eax;

} // IDropTarget_DragLeave

REG_T IDropTarget_Drop(REG_T pthis, REG_T lpDataObject, REG_T grfKeyState, POINT pt, REG_T lpdwEffect)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	STGMEDIUM medium;
	FORMATETC fmte;

	// PrintText 'IDropTarget_Drop'
	esi = lpDataObject;
	ebx = lpdwEffect;
	edi = pthis;
	edi = *(DWORD *)edi;
	eax = E_INVALIDARG;
	*(DWORD *)ebx = DROPEFFECT_NONE;
	((IDropTarget *)edi)->valid = FALSE;
	if(esi && ((IDropTarget *)edi)->hwnd)
	{
		eax = GetWindowLong(((IDropTarget *)edi)->hwnd, 0);
		if(eax)
		{
			ebx = eax;
			fmte.cfFormat = CF_TEXT;
			fmte.lptd = NULL;
			fmte.dwAspect = DVASPECT_CONTENT;
			fmte.lindex = -1;
			fmte.tymed = TYMED_HGLOBAL;
			edx = *(DWORD *)esi;
			eax = ((IDataObject *)edx)->GetData(esi, &fmte, &medium);
			if(eax==S_OK)
			{
				eax = ((IDropTarget *)edi)->cp;
				edx = ((IDropTarget *)edi)->hwnd;
				edx = hDragWin;
				// Test if Drop is on top of Drag
				if((edx!=((EDIT *)ebx)->edta.hwnd && edx!=((EDIT *)ebx)->edtb.hwnd) || eax<=cpDragSource.cpMin || eax>=cpDragSource.cpMax)
				{
					((EDIT *)ebx)->cpMin = eax;
					((EDIT *)ebx)->cpMax = eax;
					if(eax<=cpDragSource.cpMin && (edx==((EDIT *)ebx)->edta.hwnd || edx==((EDIT *)ebx)->edtb.hwnd))
					{
						ecx = cpDragSource.cpMax;
						ecx -= cpDragSource.cpMin;
						cpDragSource.cpMin += ecx;
						cpDragSource.cpMax += ecx;
					} // endif
					eax = Paste(ebx, ((IDropTarget *)edi)->hwnd, medium.hGlobal);
					edx = lpdwEffect;
					if(grfKeyState & MK_CONTROL)
					{
						*(DWORD *)edx = DROPEFFECT_COPY;
					}
					else
					{
						*(DWORD *)edx = DROPEFFECT_MOVE;
					} // endif
				} // endif
				((IDropTarget *)edi)->valid = TRUE;
				eax = medium.pUnkForRelease;
				if(eax)
				{
					edx = *(DWORD *)eax;
					eax = ((IDataObject *)edx)->lpVtbl->Release(eax);
				}
				else
				{
					eax = GlobalFree(medium.hGlobal);
				} // endif
				eax = ((IDropTarget *)edi)->DragLeave(pthis);
				eax = S_OK;
			} // endif
		}
		else
		{
			eax = E_INVALIDARG;
		} // endif
	} // endif
	return eax;

} // IDropTarget_Drop

// IDropSource methods
REG_T IDropSource_QueryInterface(REG_T pthis, REG_T iid, REG_T ppvObject)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	// PrintText 'IDropSource_QueryInterface'
	eax = IsEqualGUID(iid, &IID_IDropSource);
	if(!eax)
	{
		eax = IsEqualGUID(iid, &IID_IUnknown);
	} // endif
	edx = ppvObject;
	if(eax)
	{
		eax = pthis;
		*(DWORD *)edx = eax;
		edx = *(DWORD *)eax;
		eax = ((IDropSource *)edx)->lpVtbl->AddRef(eax);
		eax = S_OK;
	}
	else
	{
		*(DWORD *)edx = 0;
		eax = E_NOINTERFACE;
	} // endif
	return eax;

} // IDropSource_QueryInterface

REG_T IDropSource_AddRef(REG_T pthis)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	// PrintText 'IDropSource_AddRef'
	eax = pthis;
	edx = *(DWORD *)eax;
	((IDropSource *)edx)->refcount++;
	eax = ((IDropSource *)edx)->refcount;
	return eax;

} // IDropSource_AddRef

REG_T IDropSource_Release(REG_T pthis)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	// PrintText 'IDropSource_Release'
	eax = pthis;
	edx = *(DWORD *)eax;
	if(((IDropTarget *)edx)->refcount)
	{
		((IDropTarget *)edx)->refcount--;
	} // endif
	eax = ((IDropTarget *)edx)->refcount;
	return eax;

} // IDropSource_Release

REG_T IDropSource_QueryContinueDrag(REG_T pthis, REG_T fEscapePressed, REG_T grfKeyState)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	// PrintText 'IDropSource_QueryContinueDrag'
	if(fEscapePressed)
	{
		eax = DRAGDROP_S_CANCEL;
	}
	else if(!(grfKeyState &	MK_LBUTTON))
	{
		eax = DRAGDROP_S_DROP;
	}
	else
	{
		eax = S_OK;
	} // endif
	return eax;

} // IDropSource_QueryContinueDrag

REG_T IDropSource_GiveFeedback(REG_T pthis, REG_T dwEffect)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	// PrintText 'IDropSource_GiveFeedback'
	eax = DRAGDROP_S_USEDEFAULTCURSORS;
	return eax;

} // IDropSource_GiveFeedback

// IDataObject methods
REG_T IDO_QueryInterface(REG_T pthis, REG_T iid, REG_T ppvObject)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	// PrintText 'IDataObject_QueryInterface'
	eax = IsEqualGUID(iid, &IID_IDataObject);
	if(!eax)
	{
		eax = IsEqualGUID(iid, &IID_IUnknown);
	} // endif
	edx = ppvObject;
	if(eax)
	{
		eax = pthis;
		*(DWORD *)edx = eax;
		edx = *(DWORD *)eax;
		eax = ((IDataObject *)edx)->lpVtbl->AddRef(eax);
		eax = S_OK;
	}
	else
	{
		*(DWORD *)edx = 0;
		eax = E_NOINTERFACE;
	} // endif
	return eax;

} // IDO_QueryInterface

REG_T IDO_AddRef(REG_T pthis)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	// PrintText 'IDataObject_AddRef'
	eax = pthis;
	edx = *(DWORD *)eax;
	((IDataObject *)edx)->refcount++;
	eax = ((IDataObject *)edx)->refcount;
	return eax;

} // IDO_AddRef

REG_T IDO_Release(REG_T pthis)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	// PrintText 'IDataObject_Release'
	eax = pthis;
	edx = *(DWORD *)eax;
	if(((IDataObject *)edx)->refcount)
	{
		((IDataObject *)edx)->refcount--;
	} // endif
	eax = ((IDataObject *)edx)->refcount;
	return eax;

} // IDO_Release

REG_T IDO_GetData(REG_T pthis, REG_T pFormatetc, REG_T pmedium)
{
	REG_T eax = 0, ecx, edx, ebx, esi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD hCMem;

	// PrintText 'IDataObject_GetData'
	esi = pFormatetc;
	if(((FORMATETC *)esi)->cfFormat==CF_TEXT)
	{
		if(((FORMATETC *)esi)->dwAspect==DVASPECT_CONTENT)
		{
			if(((FORMATETC *)esi)->lindex==-1)
			{
				if(((FORMATETC *)esi)->tymed==TYMED_HGLOBAL)
				{
					ebx = hDragSourceMem;
					eax = ((EDIT *)ebx)->cpMin;
					eax -= ((EDIT *)ebx)->cpMax;
					if(R_SIGNED(eax) < 0)
					{
						eax = -eax;
					} // endif
					eax *= 1;
					eax++;
					eax = xGlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE | GMEM_ZEROINIT, eax);
					hCMem = eax;
					eax = GlobalLock(hCMem);
					eax = EditCopy(ebx, eax);
					eax = GlobalUnlock(hCMem);
					edx = pmedium;
					((STGMEDIUM *)edx)->tymed = TYMED_HGLOBAL;
					eax = hCMem;
					((STGMEDIUM *)edx)->hGlobal = eax;
					((STGMEDIUM *)edx)->pUnkForRelease = NULL;
					eax = S_OK;
				}
				else
				{
					eax = DV_E_TYMED;
				} // endif
			}
			else
			{
				eax = DV_E_LINDEX;
			} // endif
		}
		else
		{
			eax = DV_E_DVASPECT;
		} // endif
	}
	else
	{
		eax = DV_E_CLIPFORMAT;
	} // endif
	return eax;

} // IDO_GetData

REG_T IDO_GetDataHere(REG_T pthis, REG_T pFormatetc, REG_T pmedium)
{
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	// PrintText 'IDataObject_GetDataHere'
	eax = E_NOTIMPL;
	return eax;

} // IDO_GetDataHere

REG_T IDO_QueryGetData(REG_T pthis, REG_T pFormatetc)
{
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	// PrintText 'IDataObject_QueryGetData'
	ebx = pFormatetc;
	if(((FORMATETC *)ebx)->cfFormat==CF_TEXT)
	{
		if(((FORMATETC *)ebx)->dwAspect==DVASPECT_CONTENT)
		{
			if(((FORMATETC *)ebx)->lindex==-1)
			{
				if(((FORMATETC *)ebx)->tymed==TYMED_HGLOBAL)
				{
					eax = S_OK;
				}
				else
				{
					eax = DV_E_TYMED;
				} // endif
			}
			else
			{
				eax = DV_E_LINDEX;
			} // endif
		}
		else
		{
			eax = DV_E_DVASPECT;
		} // endif
	}
	else
	{
		eax = DV_E_CLIPFORMAT;
	} // endif
	return eax;

} // IDO_QueryGetData

REG_T IDO_GetCanonicalFormatEtc(REG_T pthis, REG_T pFormatetcIn, REG_T pFormatetcOut)
{
	REG_T eax = 0, ecx, edx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	// PrintText 'IDataObject_GetCanonicalFormatEtc'
	esi = pFormatetcIn;
	edi = pFormatetcOut;
	ecx = sizeof(FORMATETC);
	while(ecx > 0)
	{
		*(BYTE *)edi = *(BYTE *)esi;
		edi++;
		esi++;
		ecx--;
	}
	((FORMATETC *)edi)->lptd = NULL;
	eax = DATA_S_SAMEFORMATETC;
	return eax;

} // IDO_GetCanonicalFormatEtc

REG_T IDO_SetData(REG_T pthis, REG_T pFormatetc, REG_T pmedium, REG_T fRelease)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	// PrintText 'IDataObject_SetData'
	eax = E_NOTIMPL;
	return eax;

} // IDO_SetData

REG_T IDO_EnumFormatEtc(REG_T pthis, REG_T dwDirection, REG_T ppenumFormatetc)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	// PrintText 'IDataObject_EnumFormatEtc'
	if(dwDirection==DATADIR_GET)
	{
		eax = &pIEnumFORMATETC;
		edx = ppenumFormatetc;
		*(DWORD *)edx = eax;
		eax = S_OK;
	}
	else
	{
		eax = E_NOTIMPL;
	} // endif
	return eax;

} // IDO_EnumFormatEtc

REG_T IDO_DAdvise(REG_T pthis, REG_T pFormatetc, REG_T advf, REG_T pAdvSink, REG_T pdwConnection)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	// PrintText 'IDataObject_DAdvise'
	eax = E_NOTIMPL;
	return eax;

} // IDO_DAdvise

REG_T IDO_DUnadvise(REG_T pthis, REG_T dwConnection)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	// PrintText 'IDataObject_DUnadvise'
	eax = E_NOTIMPL;
	return eax;

} // IDO_DUnadvise

REG_T IDO_EnumDAdvise(REG_T pthis, REG_T ppenumAdvise)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	// PrintText 'IDataObject_EnumDAdvise'
	eax = E_NOTIMPL;
	return eax;

} // IDO_EnumDAdvise

// IEnumFORMATETC	methods
REG_T IEnumFORMATETC_QueryInterface(REG_T pthis, REG_T iid, REG_T ppvObject)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	// PrintText 'IEnumFORMATETC_QueryInterface'
	eax = IsEqualGUID(iid, &IID_IEnumFORMATETC);
	if(!eax)
	{
		eax = IsEqualGUID(iid, &IID_IUnknown);
	} // endif
	edx = ppvObject;
	if(eax)
	{
		eax = pthis;
		*(DWORD *)edx = eax;
		edx = *(DWORD *)eax;
		eax = ((IEnumFORMATETC *)edx)->lpVtbl->AddRef(eax);
		eax = S_OK;
	}
	else
	{
		*(DWORD *)edx = 0;
		eax = E_NOINTERFACE;
	} // endif
	return eax;

} // IEnumFORMATETC_QueryInterface

REG_T IEnumFORMATETC_AddRef(REG_T pthis)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	// PrintText 'IEnumFORMATETC_AddRef'
	eax = pthis;
	edx = *(DWORD *)eax;
	((IEnumFORMATETC *)edx)->refcount++;
	eax = ((IEnumFORMATETC *)edx)->refcount;
	return eax;

} // IEnumFORMATETC_AddRef

REG_T IEnumFORMATETC_Release(REG_T pthis)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	// PrintText 'IEnumFORMATETC_Release'
	eax = pthis;
	edx = *(DWORD *)eax;
	if(((IEnumFORMATETC *)edx)->refcount)
	{
		((IEnumFORMATETC *)edx)->refcount--;
	} // endif
	if(!((IEnumFORMATETC *)edx)->refcount)
	{
		((IEnumFORMATETC *)edx)->ifmt = 0;
	} // endif
	eax = ((IEnumFORMATETC *)edx)->refcount;
	return eax;

} // IEnumFORMATETC_Release

REG_T IEnumFORMATETC_Next(REG_T pthis, REG_T celt, REG_T rgelt, REG_T pceltFetched)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	// PrintText 'IEnumFORMATETC_Next'
	edx = 0;
	eax = pthis;
	eax = *(DWORD *)eax;
	ecx = ((IEnumFORMATETC *)eax)->ifmt;
	if(ecx<((IEnumFORMATETC *)eax)->ifmtmax)
	{
		edx++;
		ecx++;
		((IEnumFORMATETC *)eax)->ifmt = ecx;
		eax = rgelt;
		((FORMATETC *)eax)->cfFormat = CF_TEXT;
		((FORMATETC *)eax)->lptd = NULL;
		((FORMATETC *)eax)->dwAspect = DVASPECT_CONTENT;
		((FORMATETC *)eax)->lindex = -1;
		((FORMATETC *)eax)->tymed = TYMED_HGLOBAL;
	} // endif
	eax = pceltFetched;
	if(eax)
	{
		*(DWORD *)eax = edx;
	} // endif
	if(edx==celt)
	{
		eax = S_OK;
	}
	else
	{
		eax = S_FALSE;
	} // endif
	return eax;

} // IEnumFORMATETC_Next

REG_T IEnumFORMATETC_Skip(REG_T pthis, REG_T celt)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	// PrintText 'IEnumFORMATETC_Skip'
	eax = E_NOTIMPL;
	return eax;

} // IEnumFORMATETC_Skip

REG_T IEnumFORMATETC_Reset(REG_T pthis)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	// PrintText 'IEnumFORMATETC_Reset'
	eax = pthis;
	edx = *(DWORD *)eax;
	((IEnumFORMATETC *)edx)->ifmt = 0;
	eax = S_OK;
	return eax;

} // IEnumFORMATETC_Reset

REG_T IEnumFORMATETC_Clone(REG_T pthis, REG_T ppenum)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	// PrintText 'IEnumFORMATETC_Clone'
	eax = E_NOTIMPL;
	return eax;

} // IEnumFORMATETC_Clone


