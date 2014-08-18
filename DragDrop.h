#pragma once

#include <windows.h>
#include "Data.h"

// IDropTarget

typedef struct
{
    const IDropTargetVtbl *lpVtbl;
	DWORD refcount;
	DWORD valid;
	DWORD hwnd;
	DWORD cp;
} IDropTargetImpl;

extern IDropTargetImpl DropTarget;

// IDropSource

typedef struct
{
    const IDropSourceVtbl *lpVtbl;
	DWORD refcount;
} IDropSourceImpl;

extern IDropSourceImpl DropSource;

// IDataObject

typedef struct
{
    const IDataObjectVtbl *lpVtbl;
	DWORD refcount;
} IDataObjectImpl;

extern IDataObjectImpl DataObject;

//REG_T IsEqualGUID(REG_T rguid1, REG_T rguid2);
REG_T WINAPI IDropTarget_QueryInterface(REG_T pthis, REG_T iid, REG_T ppvObject);
REG_T WINAPI IDropTarget_AddRef(REG_T pthis);
REG_T WINAPI IDropTarget_Release(REG_T pthis);
REG_T WINAPI IDropTarget_DragEnter(REG_T pthis, REG_T lpDataObject, REG_T grfKeyState, POINT pt, REG_T lpdwEffect);
REG_T WINAPI IDropTarget_DragOver(REG_T pthis, REG_T grfKeyState, POINT pt, REG_T lpdwEffect);
REG_T WINAPI IDropTarget_DragLeave(REG_T pthis);
REG_T WINAPI IDropTarget_Drop(REG_T pthis, REG_T lpDataObject, REG_T grfKeyState, POINT pt, REG_T lpdwEffect);
REG_T WINAPI IDropSource_QueryInterface(REG_T pthis, REG_T iid, REG_T ppvObject);
REG_T WINAPI IDropSource_AddRef(REG_T pthis);
REG_T WINAPI IDropSource_Release(REG_T pthis);
REG_T WINAPI IDropSource_QueryContinueDrag(REG_T pthis, REG_T fEscapePressed, REG_T grfKeyState);
REG_T WINAPI IDropSource_GiveFeedback(REG_T pthis, REG_T dwEffect);
REG_T WINAPI IDO_QueryInterface(REG_T pthis, REG_T iid, REG_T ppvObject);
REG_T WINAPI IDO_AddRef(REG_T pthis);
REG_T WINAPI IDO_Release(REG_T pthis);
REG_T WINAPI IDO_GetData(REG_T pthis, REG_T pFormatetc, REG_T pmedium);
REG_T WINAPI IDO_GetDataHere(REG_T pthis, REG_T pFormatetc, REG_T pmedium);
REG_T WINAPI IDO_QueryGetData(REG_T pthis, REG_T pFormatetc);
REG_T WINAPI IDO_GetCanonicalFormatEtc(REG_T pthis, REG_T pFormatetcIn, REG_T pFormatetcOut);
REG_T WINAPI IDO_SetData(REG_T pthis, REG_T pFormatetc, REG_T pmedium, REG_T fRelease);
REG_T WINAPI IDO_EnumFormatEtc(REG_T pthis, REG_T dwDirection, REG_T ppenumFormatetc);
REG_T WINAPI IDO_DAdvise(REG_T pthis, REG_T pFormatetc, REG_T advf, REG_T pAdvSink, REG_T pdwConnection);
REG_T WINAPI IDO_DUnadvise(REG_T pthis, REG_T dwConnection);
REG_T WINAPI IDO_EnumDAdvise(REG_T pthis, REG_T ppenumAdvise);
REG_T WINAPI IEnumFORMATETC_QueryInterface(REG_T pthis, REG_T iid, REG_T ppvObject);
REG_T WINAPI IEnumFORMATETC_AddRef(REG_T pthis);
REG_T WINAPI IEnumFORMATETC_Release(REG_T pthis);
REG_T WINAPI IEnumFORMATETC_Next(REG_T pthis, REG_T celt, REG_T rgelt, REG_T pceltFetched);
REG_T WINAPI IEnumFORMATETC_Skip(REG_T pthis, REG_T celt);
REG_T WINAPI IEnumFORMATETC_Reset(REG_T pthis);
REG_T WINAPI IEnumFORMATETC_Clone(REG_T pthis, REG_T ppenum);
