#include "utils.h"


#include <stdio.h>
#include <tchar.h>

#include <Windows.h>
#include <TlHelp32.h>

DWORD GetClientDllBaseAddress(HANDLE hSnapshot)
{
	MODULEENTRY32 module_entry32;
	module_entry32.dwSize = sizeof(module_entry32);

	BOOL was_copied = Module32First(hSnapshot, &module_entry32);

	while (was_copied)
	{
		if (_tcscmp(TEXT("client.dll"), module_entry32.szModule) == 0)
		{
			return (DWORD)module_entry32.modBaseAddr;
		}

		_tprintf(module_entry32.szModule);
		printf("\r\n");
		
		was_copied = Module32Next(hSnapshot, &module_entry32);
	}

	return 0;
}
