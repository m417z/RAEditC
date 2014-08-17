#include <windows.h>
#include "Data.h"

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif

DLL_EXPORT BOOL APIENTRY DllMain(HINSTANCE hInst, DWORD reason, LPVOID reserved1)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	if(reason==DLL_PROCESS_ATTACH)
	{
		InstallRAEdit(hInst, TRUE);
	}
	else if(reason==DLL_PROCESS_DETACH)
	{
		UnInstallRAEdit();
	} // endif
	eax = TRUE;
	return eax;

} // DllEntry

// NOTE: RadASM 1.2.0.5 uses this method.
// In RadASM.ini section [CustCtrl], x=CustCtrl.dll,y
// x is next free number.
// y is number of controls in the dll. In this case there is only one control.
//
// x=RAEdit.dll,1
// Copy RAEdit.dll to c:\windows\system
//
REG_T GetDef(DWORD nInx)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	eax = nInx;
	if(!eax)
	{
		// Get the toolbox bitmap
		// RadASM destroys it after use, so you don't have to worry about that.
		eax = LoadBitmap(hInstance, IDB_RAEDITBUTTON);
		ccdef.hbmp = eax;
		// Return pointer to inited struct
		eax = &ccdef;
	}
	else
	{
		eax = 0;
	} // endif
	return eax;

} // GetDef

REG_T GetDefEx(DWORD nInx)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	eax = nInx;
	if(!eax)
	{
		// Get the toolbox bitmap
		eax = LoadBitmap(hInstance, IDB_RAEDITBUTTON);
		ccdefex.hbmp = eax;
		// Return pointer to inited struct
		eax = &ccdefex;
	}
	else
	{
		eax = 0;
	} // endif
	return eax;

} // GetDefEx

