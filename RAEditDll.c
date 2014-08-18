#include "RAEditDll.h"

DLL_EXPORT BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID reserved1)
{
	REG_T eax = 0;

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
