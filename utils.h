#ifndef UTILS_H
#define UTILS_H
#include <Windows.h>

/// <summary>
/// Gets the client dll base address from the l4d2 process.
/// </summary>
/// <param name="hSnapshot">A handle created from CreateToolhelp32Snapshot.</param>
/// <returns>The address if successful or NULL if not.</returns>
DWORD get_client_dll_base_address(HANDLE hSnapshot);

#endif
