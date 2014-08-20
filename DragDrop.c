#include "DragDrop.h"

#include "ClipBoard.h"
#include "Memory.h"
#include "Position.h"

// IDropTarget

static const IDropTargetVtbl vtIDropTarget = {
	IDropTarget_QueryInterface,
	IDropTarget_AddRef,
	IDropTarget_Release,
	IDropTarget_DragEnter,
	IDropTarget_DragOver,
	IDropTarget_DragLeave,
	IDropTarget_Drop
};

IDropTargetImpl DropTarget = { &vtIDropTarget, 0, 0, 0, 0 };

// IDropSource

static const IDropSourceVtbl vtIDropSource = {
	IDropSource_QueryInterface,
	IDropSource_AddRef,
	IDropSource_Release,
	IDropSource_QueryContinueDrag,
	IDropSource_GiveFeedback
};

IDropSourceImpl DropSource = { &vtIDropSource, 0 };

// IDataObject

static const IDataObjectVtbl vtIDataObject = {
	IDO_QueryInterface,
	IDO_AddRef,
	IDO_Release,
	IDO_GetData,
	IDO_GetDataHere,
	IDO_QueryGetData,
	IDO_GetCanonicalFormatEtc,
	IDO_SetData,
	IDO_EnumFormatEtc,
	IDO_DAdvise,
	IDO_DUnadvise,
	IDO_EnumDAdvise
};

IDataObjectImpl DataObject = { &vtIDataObject, 0 };

// IEnumFORMATETC

static const IEnumFORMATETCVtbl vtIEnumFORMATETCVtbl = {
	IEnumFORMATETC_QueryInterface,
	IEnumFORMATETC_AddRef,
	IEnumFORMATETC_Release,
	IEnumFORMATETC_Next,
	IEnumFORMATETC_Skip,
	IEnumFORMATETC_Reset,
	IEnumFORMATETC_Clone
};

typedef struct
{
	const IEnumFORMATETCVtbl *lpVtbl;
	DWORD refcount;
	DWORD ifmt;
	DWORD ifmtmax;
} IEnumFORMATETCImpl;

static IEnumFORMATETCImpl EnumFORMATETC = { &vtIEnumFORMATETCVtbl, 0, 0, 1 };

/*
REG_T IsEqualGUID(REG_T rguid1, REG_T rguid2)
{
	REG_T eax = 0, ecx, ecx, esi, edi;

	eax = 0;
	esi = rguid1;
	edi = rguid2;
	ecx = sizeof(GUID)/4;
	eax = memcmp(esi, edi, ecx*4) == 0;
	return eax;

} // IsEqualGUID
*/
// IDropTarget methods
REG_T WINAPI IDropTarget_QueryInterface(REG_T pthis, REG_T iid, REG_T ppvObject)
{
	REG_T eax = 0, edx;

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
		*(REG_T *)edx = eax;
		edx = eax;
		eax = ((IDropTargetImpl *)edx)->lpVtbl->AddRef(eax);
		eax = S_OK;
	}
	else
	{
		*(REG_T *)edx = 0;
		eax = E_NOINTERFACE;
	} // endif
	return eax;

} // IDropTarget_QueryInterface

REG_T WINAPI IDropTarget_AddRef(REG_T pthis)
{
	REG_T eax = 0, edx;

	// PrintText 'IDropTarget_AddRef'
	eax = pthis;
	edx = eax;
	((IDropTargetImpl *)edx)->refcount++;
	eax = ((IDropTargetImpl *)edx)->refcount;
	return eax;

} // IDropTarget_AddRef

REG_T WINAPI IDropTarget_Release(REG_T pthis)
{
	REG_T eax = 0, edx;

	// PrintText 'IDropTarget_Release'
	eax = pthis;
	edx = eax;
	if(((IDropTargetImpl *)edx)->refcount)
	{
		((IDropTargetImpl *)edx)->refcount--;
	} // endif
	eax = ((IDropTargetImpl *)edx)->refcount;
	return eax;

} // IDropTarget_Release

REG_T WINAPI IDropTarget_DragEnter(REG_T pthis, REG_T lpDataObject, DWORD grfKeyState, POINT pt, REG_T lpdwEffect)
{
	REG_T eax = 0, edx, ebx, esi, edi;
	REG_T temp1;
	STGMEDIUM medium;
	FORMATETC fmte;
	EDIT *pMem;

	// PrintText 'IDropTarget_DragEnter'
	esi = lpDataObject;
	ebx = lpdwEffect;
	edi = pthis;

	*(DWORD *)ebx = DROPEFFECT_NONE;
	((IDropTargetImpl *)edi)->valid = FALSE;
	eax = E_INVALIDARG;
	if(esi)
	{
		eax = WindowFromPoint(pt);
		temp1 = eax;
		edx = eax;
		eax = ScreenToClient(edx, &pt);
		eax = temp1;
		eax = ChildWindowFromPoint(eax, pt);
		ebx = eax;
		eax = GetWindowLongPtr(ebx, 0);
		if(eax)
		{
			((IDropTargetImpl *)edi)->hwnd = ebx;
			pMem = eax;
			if(!(pMem->fstyle & STYLE_READONLY))
			{
				fmte.cfFormat = CF_TEXT;
				fmte.ptd = NULL;
				fmte.dwAspect = DVASPECT_CONTENT;
				fmte.lindex = -1;
				fmte.tymed = TYMED_HGLOBAL;
				edx = esi;
				eax = ((IDataObjectImpl *)edx)->lpVtbl->GetData(esi, &fmte, &medium);
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
					((IDropTargetImpl *)edi)->valid = TRUE;
					eax = medium.pUnkForRelease;
					if(eax)
					{
						edx = eax;
						eax = ((IDataObjectImpl *)edx)->lpVtbl->Release(eax);
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

REG_T WINAPI IDropTarget_DragOver(REG_T pthis, DWORD grfKeyState, POINT pt, REG_T lpdwEffect)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1;
	RECT rect;
	EDIT *pMem;

	// PrintText 'IDropTarget_DragOver'
	edi = pthis;
	edx = lpdwEffect;
	*(DWORD *)edx = DROPEFFECT_NONE;
	if(((IDropTargetImpl *)edi)->valid)
	{
		eax = WindowFromPoint(pt);
		temp1 = eax;
		edx = eax;
		eax = ScreenToClient(edx, &pt);
		eax = temp1;
		eax = ChildWindowFromPoint(eax, pt);
		ebx = eax;
		eax = GetWindowLongPtr(ebx, 0);
		if(eax)
		{
			((IDropTargetImpl *)edi)->hwnd = ebx;
			pMem = eax;
			eax = SetFocus(((IDropTargetImpl *)edi)->hwnd);
			eax = ((IDropTargetImpl *)edi)->hwnd;
			if(eax==pMem->edta.hwnd)
			{
				esi = &pMem->edta;
			}
			else
			{
				esi = &pMem->edtb;
			} // endif
			eax = GetClientRect(((IDropTargetImpl *)edi)->hwnd, &rect);
			ecx = pMem->fntinfo.fntht;
			ecx /= 2;
			eax = pt.y;
			edx = eax+ecx;
			if(eax<ecx)
			{
				eax = SendMessage(((IDropTargetImpl *)edi)->hwnd, WM_VSCROLL, SB_LINEUP, ((RAEDT *)esi)->hvscroll);
			}
			else if(edx>rect.bottom)
			{
				eax = SendMessage(((IDropTargetImpl *)edi)->hwnd, WM_VSCROLL, SB_LINEDOWN, ((RAEDT *)esi)->hvscroll);
			}
			else
			{
				eax = pt.x;
				edx = eax+32;
				ecx = pMem->selbarwt;
				ecx += pMem->linenrwt;
				if(eax<ecx)
				{
					eax = SendMessage(((IDropTargetImpl *)edi)->hwnd, WM_HSCROLL, SB_LINEUP, pMem->hhscroll);
				}
				else if(edx>rect.right)
				{
					eax = SendMessage(((IDropTargetImpl *)edi)->hwnd, WM_HSCROLL, SB_LINEDOWN, pMem->hhscroll);
				} // endif
			} // endif
			eax = GetCharFromPos(pMem, ((RAEDT *)esi)->cpy, pt.x, pt.y);
			edx = eax;
			((IDropTargetImpl *)edi)->cp = eax;
			eax = GetPosFromChar(pMem, edx, &pt);
			eax = pt.x;
			eax -= pMem->cpx;
			edx = pt.y;
			edx -= ((RAEDT *)esi)->cpy;
			eax = SetCaretPos(eax, edx);
			eax = ShowCaret(((IDropTargetImpl *)edi)->hwnd);
			pMem->fCaretHide = FALSE;
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

REG_T WINAPI IDropTarget_DragLeave(REG_T pthis)
{
	REG_T eax = 0, edi;
	EDIT *pMem;

	// PrintText 'IDropTarget_DragLeave'
	edi = pthis;
	if(((IDropTargetImpl *)edi)->hwnd)
	{
		eax = GetWindowLongPtr(((IDropTargetImpl *)edi)->hwnd, 0);
		if(eax)
		{
			pMem = eax;
			if(!pMem->fCaretHide)
			{
				eax = HideCaret(((IDropTargetImpl *)edi)->hwnd);
				pMem->fCaretHide = TRUE;
			} // endif
		} // endif
		((IDropTargetImpl *)edi)->hwnd = 0;
	} // endif
	eax = S_OK;
	return eax;

} // IDropTarget_DragLeave

REG_T WINAPI IDropTarget_Drop(REG_T pthis, REG_T lpDataObject, DWORD grfKeyState, POINT pt, REG_T lpdwEffect)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	STGMEDIUM medium;
	FORMATETC fmte;
	EDIT *pMem;

	// PrintText 'IDropTarget_Drop'
	esi = lpDataObject;
	ebx = lpdwEffect;
	edi = pthis;
	eax = E_INVALIDARG;
	*(DWORD *)ebx = DROPEFFECT_NONE;
	((IDropTargetImpl *)edi)->valid = FALSE;
	if(esi && ((IDropTargetImpl *)edi)->hwnd)
	{
		eax = GetWindowLongPtr(((IDropTargetImpl *)edi)->hwnd, 0);
		if(eax)
		{
			pMem = eax;
			fmte.cfFormat = CF_TEXT;
			fmte.ptd = NULL;
			fmte.dwAspect = DVASPECT_CONTENT;
			fmte.lindex = -1;
			fmte.tymed = TYMED_HGLOBAL;
			edx = esi;
			eax = ((IDataObjectImpl *)edx)->lpVtbl->GetData(esi, &fmte, &medium);
			if(eax==S_OK)
			{
				eax = ((IDropTargetImpl *)edi)->cp;
				edx = ((IDropTargetImpl *)edi)->hwnd;
				edx = hDragWin;
				// Test if Drop is on top of Drag
				if((edx!=pMem->edta.hwnd && edx!=pMem->edtb.hwnd) || eax<=cpDragSource.cpMin || eax>=cpDragSource.cpMax)
				{
					pMem->cpMin = eax;
					pMem->cpMax = eax;
					if(eax<=cpDragSource.cpMin && (edx==pMem->edta.hwnd || edx==pMem->edtb.hwnd))
					{
						ecx = cpDragSource.cpMax;
						ecx -= cpDragSource.cpMin;
						cpDragSource.cpMin += ecx;
						cpDragSource.cpMax += ecx;
					} // endif
					eax = Paste(pMem, ((IDropTargetImpl *)edi)->hwnd, medium.hGlobal);
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
				((IDropTargetImpl *)edi)->valid = TRUE;
				eax = medium.pUnkForRelease;
				if(eax)
				{
					edx = eax;
					eax = ((IDataObjectImpl *)edx)->lpVtbl->Release(eax);
				}
				else
				{
					eax = GlobalFree(medium.hGlobal);
				} // endif
				eax = ((IDropTargetImpl *)edi)->lpVtbl->DragLeave(pthis);
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
REG_T WINAPI IDropSource_QueryInterface(REG_T pthis, REG_T iid, REG_T ppvObject)
{
	REG_T eax = 0, edx;

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
		*(REG_T *)edx = eax;
		edx = eax;
		eax = ((IDropSourceImpl *)edx)->lpVtbl->AddRef(eax);
		eax = S_OK;
	}
	else
	{
		*(REG_T *)edx = 0;
		eax = E_NOINTERFACE;
	} // endif
	return eax;

} // IDropSource_QueryInterface

REG_T WINAPI IDropSource_AddRef(REG_T pthis)
{
	REG_T eax = 0, edx;

	// PrintText 'IDropSource_AddRef'
	eax = pthis;
	edx = eax;
	((IDropSourceImpl *)edx)->refcount++;
	eax = ((IDropSourceImpl *)edx)->refcount;
	return eax;

} // IDropSource_AddRef

REG_T WINAPI IDropSource_Release(REG_T pthis)
{
	REG_T eax = 0, edx;

	// PrintText 'IDropSource_Release'
	eax = pthis;
	edx = eax;
	if(((IDropTargetImpl *)edx)->refcount)
	{
		((IDropTargetImpl *)edx)->refcount--;
	} // endif
	eax = ((IDropTargetImpl *)edx)->refcount;
	return eax;

} // IDropSource_Release

REG_T WINAPI IDropSource_QueryContinueDrag(REG_T pthis, DWORD fEscapePressed, DWORD grfKeyState)
{
	REG_T eax = 0;

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

REG_T WINAPI IDropSource_GiveFeedback(REG_T pthis, DWORD dwEffect)
{
	REG_T eax = 0;

	// PrintText 'IDropSource_GiveFeedback'
	eax = DRAGDROP_S_USEDEFAULTCURSORS;
	return eax;

} // IDropSource_GiveFeedback

// IDataObject methods
REG_T WINAPI IDO_QueryInterface(REG_T pthis, REG_T iid, REG_T ppvObject)
{
	REG_T eax = 0, edx;

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
		*(REG_T *)edx = eax;
		edx = eax;
		eax = ((IDataObjectImpl *)edx)->lpVtbl->AddRef(eax);
		eax = S_OK;
	}
	else
	{
		*(REG_T *)edx = 0;
		eax = E_NOINTERFACE;
	} // endif
	return eax;

} // IDO_QueryInterface

REG_T WINAPI IDO_AddRef(REG_T pthis)
{
	REG_T eax = 0, edx;

	// PrintText 'IDataObject_AddRef'
	eax = pthis;
	edx = eax;
	((IDataObjectImpl *)edx)->refcount++;
	eax = ((IDataObjectImpl *)edx)->refcount;
	return eax;

} // IDO_AddRef

REG_T WINAPI IDO_Release(REG_T pthis)
{
	REG_T eax = 0, edx;

	// PrintText 'IDataObject_Release'
	eax = pthis;
	edx = eax;
	if(((IDataObjectImpl *)edx)->refcount)
	{
		((IDataObjectImpl *)edx)->refcount--;
	} // endif
	eax = ((IDataObjectImpl *)edx)->refcount;
	return eax;

} // IDO_Release

REG_T WINAPI IDO_GetData(REG_T pthis, REG_T pFormatetc, REG_T pmedium)
{
	REG_T eax = 0, edx, esi;
	REG_T hCMem;
	EDIT *pMem;

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
					pMem = hDragSourceMem;
					eax = pMem->cpMin;
					eax -= pMem->cpMax;
					if(R_SIGNED(eax) < 0)
					{
						eax = -eax;
					} // endif
					eax *= 2;
					eax++;
					eax = xGlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE | GMEM_ZEROINIT, eax);
					hCMem = eax;
					eax = GlobalLock(hCMem);
					eax = EditCopy(pMem, eax);
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

REG_T WINAPI IDO_GetDataHere(REG_T pthis, REG_T pFormatetc, REG_T pmedium)
{
	REG_T eax = 0;

	// PrintText 'IDataObject_GetDataHere'
	eax = E_NOTIMPL;
	return eax;

} // IDO_GetDataHere

REG_T WINAPI IDO_QueryGetData(REG_T pthis, REG_T pFormatetc)
{
	REG_T eax = 0, ebx;

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

REG_T WINAPI IDO_GetCanonicalFormatEtc(REG_T pthis, REG_T pFormatetcIn, REG_T pFormatetcOut)
{
	REG_T eax = 0, ecx, esi, edi;

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
	((FORMATETC *)edi)->ptd = NULL;
	eax = DATA_S_SAMEFORMATETC;
	return eax;

} // IDO_GetCanonicalFormatEtc

REG_T WINAPI IDO_SetData(REG_T pthis, REG_T pFormatetc, REG_T pmedium, DWORD fRelease)
{
	REG_T eax = 0;

	// PrintText 'IDataObject_SetData'
	eax = E_NOTIMPL;
	return eax;

} // IDO_SetData

REG_T WINAPI IDO_EnumFormatEtc(REG_T pthis, DWORD dwDirection, REG_T ppenumFormatetc)
{
	REG_T eax = 0, edx;

	// PrintText 'IDataObject_EnumFormatEtc'
	if(dwDirection==DATADIR_GET)
	{
		eax = &EnumFORMATETC;
		edx = ppenumFormatetc;
		*(REG_T *)edx = eax;
		eax = S_OK;
	}
	else
	{
		eax = E_NOTIMPL;
	} // endif
	return eax;

} // IDO_EnumFormatEtc

REG_T WINAPI IDO_DAdvise(REG_T pthis, REG_T pFormatetc, DWORD advf, REG_T pAdvSink, REG_T pdwConnection)
{
	REG_T eax = 0;

	// PrintText 'IDataObject_DAdvise'
	eax = E_NOTIMPL;
	return eax;

} // IDO_DAdvise

REG_T WINAPI IDO_DUnadvise(REG_T pthis, DWORD dwConnection)
{
	REG_T eax = 0;

	// PrintText 'IDataObject_DUnadvise'
	eax = E_NOTIMPL;
	return eax;

} // IDO_DUnadvise

REG_T WINAPI IDO_EnumDAdvise(REG_T pthis, REG_T ppenumAdvise)
{
	REG_T eax = 0;

	// PrintText 'IDataObject_EnumDAdvise'
	eax = E_NOTIMPL;
	return eax;

} // IDO_EnumDAdvise

// IEnumFORMATETC	methods
REG_T WINAPI IEnumFORMATETC_QueryInterface(REG_T pthis, REG_T iid, REG_T ppvObject)
{
	REG_T eax = 0, edx;

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
		*(REG_T *)edx = eax;
		edx = eax;
		eax = ((IEnumFORMATETCImpl *)edx)->lpVtbl->AddRef(eax);
		eax = S_OK;
	}
	else
	{
		*(REG_T *)edx = 0;
		eax = E_NOINTERFACE;
	} // endif
	return eax;

} // IEnumFORMATETC_QueryInterface

REG_T WINAPI IEnumFORMATETC_AddRef(REG_T pthis)
{
	REG_T eax = 0, edx;

	// PrintText 'IEnumFORMATETC_AddRef'
	eax = pthis;
	edx = eax;
	((IEnumFORMATETCImpl *)edx)->refcount++;
	eax = ((IEnumFORMATETCImpl *)edx)->refcount;
	return eax;

} // IEnumFORMATETC_AddRef

REG_T WINAPI IEnumFORMATETC_Release(REG_T pthis)
{
	REG_T eax = 0, edx;

	// PrintText 'IEnumFORMATETC_Release'
	eax = pthis;
	edx = eax;
	if(((IEnumFORMATETCImpl *)edx)->refcount)
	{
		((IEnumFORMATETCImpl *)edx)->refcount--;
	} // endif
	if(!((IEnumFORMATETCImpl *)edx)->refcount)
	{
		((IEnumFORMATETCImpl *)edx)->ifmt = 0;
	} // endif
	eax = ((IEnumFORMATETCImpl *)edx)->refcount;
	return eax;

} // IEnumFORMATETC_Release

REG_T WINAPI IEnumFORMATETC_Next(REG_T pthis, DWORD celt, REG_T rgelt, REG_T pceltFetched)
{
	REG_T eax = 0, ecx, edx;

	// PrintText 'IEnumFORMATETC_Next'
	edx = 0;
	eax = pthis;
	ecx = ((IEnumFORMATETCImpl *)eax)->ifmt;
	if(ecx<((IEnumFORMATETCImpl *)eax)->ifmtmax)
	{
		edx++;
		ecx++;
		((IEnumFORMATETCImpl *)eax)->ifmt = ecx;
		eax = rgelt;
		((FORMATETC *)eax)->cfFormat = CF_TEXT;
		((FORMATETC *)eax)->ptd = NULL;
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

REG_T WINAPI IEnumFORMATETC_Skip(REG_T pthis, DWORD celt)
{
	REG_T eax = 0;

	// PrintText 'IEnumFORMATETC_Skip'
	eax = E_NOTIMPL;
	return eax;

} // IEnumFORMATETC_Skip

REG_T WINAPI IEnumFORMATETC_Reset(REG_T pthis)
{
	REG_T eax = 0, edx;

	// PrintText 'IEnumFORMATETC_Reset'
	eax = pthis;
	edx = eax;
	((IEnumFORMATETCImpl *)edx)->ifmt = 0;
	eax = S_OK;
	return eax;

} // IEnumFORMATETC_Reset

REG_T WINAPI IEnumFORMATETC_Clone(REG_T pthis, REG_T ppenum)
{
	REG_T eax = 0;

	// PrintText 'IEnumFORMATETC_Clone'
	eax = E_NOTIMPL;
	return eax;

} // IEnumFORMATETC_Clone


