#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

#include <TlHelp32.h>
#include <Windows.h>

void error(const char *text) {
  fputs(text, stderr);
  exit(EXIT_FAILURE);
}

DWORD get_client_dll_base_address(HANDLE hSnapshot) {
  MODULEENTRY32 module_entry32;
  module_entry32.dwSize = sizeof(module_entry32);

  BOOL was_copied = Module32First(hSnapshot, &module_entry32);

  while (was_copied) {
    if (_tcscmp(TEXT("client.dll"), module_entry32.szModule) == 0) {
      return (DWORD)module_entry32.modBaseAddr;
    }

    was_copied = Module32Next(hSnapshot, &module_entry32);
  }

  return 0;
}
