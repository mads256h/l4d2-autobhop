#include "utils.h"

#include <stdlib.h>
#include <stdio.h>

#include <Windows.h>
#include <TlHelp32.h>


static DWORD player_base_pointer = 0;

DWORD WINAPI UpdatePointerThread(_In_ LPVOID lpParameter);

struct UpdatePointerInfo
{
	HANDLE l4d2_process;

	DWORD player_base;
};

void error(const char* text)
{
	fputs(text, stderr);

	exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		fprintf(stderr, "Syntax: %s <playerbase> <mFlags offset>\r\n", argv[0]);

		return EXIT_FAILURE;
	}

	DWORD player_base_offset = strtoul(argv[1], NULL, 16);
	DWORD mFlags_offset = strtoul(argv[2], NULL, 16);

	printf("Using:\r\n  PlayerBase offset = %lX\r\n  mFlags offset = %lX\r\n", player_base_offset, mFlags_offset);

	puts("Autobhop is running!");
	puts("Start the game now.");

	HWND l4d2_hwnd = NULL;

	do
	{
		l4d2_hwnd = FindWindow(NULL, TEXT("Left 4 Dead 2"));
		Sleep(1000);
	}
	while (l4d2_hwnd == NULL);

	puts("Game found!");

	Sleep(5000);

	DWORD l4d2_process_id = 0;

	GetWindowThreadProcessId(l4d2_hwnd, &l4d2_process_id);

	HANDLE l4d2_process = OpenProcess(PROCESS_VM_READ, FALSE, l4d2_process_id);

	if (l4d2_process == NULL || l4d2_process_id == 0)
	{
		error("Could not open process or get process id!");
	}

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, l4d2_process_id);

	if (hSnapshot == NULL)
	{
		error("Could not create snapshot!");
	}

	const DWORD client_base_address = GetClientDllBaseAddress(hSnapshot);

	if (client_base_address == 0)
	{
		error("Could not get client.dll base address!");
	}

	struct UpdatePointerInfo* update_pointer_info = malloc(sizeof(struct UpdatePointerInfo));
	update_pointer_info->l4d2_process = l4d2_process;
	update_pointer_info->player_base = client_base_address + player_base_offset;


	
	HANDLE update_pointer_thread_handle = CreateThread(NULL, 0, UpdatePointerThread, update_pointer_info, 0, NULL);

	if (SetThreadPriority(update_pointer_thread_handle, THREAD_PRIORITY_LOWEST) == FALSE)
	{
		fprintf(stderr, "Could not set thread priority. Error: %lX\r\nContinuing anyway!\r\n", GetLastError());
	}

	DWORD m_flags = 0;
	while (TRUE)
	{
		if (GetAsyncKeyState(' ') & 0x8000)
		{
			ReadProcessMemory(l4d2_process, (char*)player_base_pointer + mFlags_offset, &m_flags, sizeof(m_flags),
			                  NULL);

			if (m_flags != 0x80 && m_flags != 0x82 && m_flags != 0x280 && m_flags != 0x282)
			{
				SendMessage(l4d2_hwnd, WM_KEYDOWN, ' ', 0x390000);
			}
			else if (m_flags == 0x80 || m_flags == 0x82 || m_flags == 0x280 || m_flags == 0x282)
			{
				SendMessage(l4d2_hwnd, WM_KEYUP, ' ', 0x390000);
			}
		}
		Sleep(0);
	}

	return EXIT_SUCCESS;
}

DWORD WINAPI UpdatePointerThread(_In_ LPVOID lpParameter)
{
	struct UpdatePointerInfo* info = lpParameter;

	while (TRUE)
	{
		ReadProcessMemory(info->l4d2_process, (void*)info->player_base, &player_base_pointer, sizeof(player_base_pointer),
		                  NULL);

		Sleep(10000);
	}
}
