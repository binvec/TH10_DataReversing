#include "util.h"

bool GetProcessIdByName(const char * exeFileName, int& pid)
{
	pid = 0;
	PROCESSENTRY32 pe = { 0 };
	pe.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	Process32First(hsnap, &pe);
	do
	{
		if (!strcmp(exeFileName, pe.szExeFile))
		{
			pid = pe.th32ProcessID;
			break;
		}
	} while (Process32Next(hsnap, &pe));
	if (!pid)
	{
		return false;
	}
	return true;
}

HANDLE OpenGameProcess(const char * exeFileName)
{
	int pid;
	if (!GetProcessIdByName(exeFileName, pid))
	{
		return 0;
	}
	return OpenProcess(PROCESS_VM_READ, true, pid);
}
