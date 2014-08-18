#include <windows.h>
#include "Data.h"

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID reserved1)
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
