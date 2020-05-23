#include "utils.h"

#include <stdlib.h>
#include <stdio.h>

#include <Windows.h>
#include <TlHelp32.h>


DWORD WINAPI update_pointer_thread(_In_ LPVOID lpParameter);

/// <summary>
/// Holds pointers for the UpdatePointerThread.
/// </summary>
struct update_pointer_info
{
	HANDLE l4d2_process;
	DWORD player_base;
	volatile DWORD player_base_pointer;
};

/// <summary>
/// Writes a string to stderr and exits.
/// </summary>
/// <param name="text">The string to print.</param>
void error(const char* text)
{
	fputs(text, stderr);

	exit(EXIT_FAILURE);
}

int main(const int argc, char* argv[])
{
	// Print syntax and exit program if we don't have the required amounts of arguments.
	if (argc != 3)
	{
		fprintf(stderr, "Syntax: %s <playerbase> <mFlags offset>\r\n", argv[0]);

		return EXIT_FAILURE;
	}

	// Get the player base offset from the first argument.
	// If it fails we print an error and exits.
	DWORD player_base_offset = 0;
	if (sscanf_s(argv[1], "%lX", &player_base_offset) != 1)
	{
		error("You did not specify playerbase correctly!");
	}

	// Get the mFlags offset from the second argument.
	// If it fails we print an error and exits.
	DWORD m_flags_offset = 0;
	if (sscanf_s(argv[2], "%lX", &m_flags_offset) != 1)
	{
		error("You did not specify mFlags offset correctly!");
	}
	

	printf("Using:\r\n  PlayerBase offset = %lX\r\n  mFlags offset = %lX\r\n", player_base_offset, m_flags_offset);

	puts("Start the game now.");

	// Keep looping until we find the l4d2 window.
	HWND l4d2_hwnd = NULL;
	while((l4d2_hwnd = FindWindow(NULL, TEXT("Left 4 Dead 2"))) == NULL)
	{
		Sleep(1000);
	}

	puts("Game found!");

	// Get the process id from the window.
	DWORD l4d2_process_id = 0;
	GetWindowThreadProcessId(l4d2_hwnd, &l4d2_process_id);

	// Get a handle to the process with virtual memory read permissions.
	HANDLE l4d2_process = OpenProcess(PROCESS_VM_READ, FALSE, l4d2_process_id);
	if (l4d2_process == NULL || l4d2_process_id == 0)
	{
		error("Could not open process or get process id!");
	}

	// Get a handle for looping through loaded modules in the process.
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, l4d2_process_id);
	if (hSnapshot == NULL)
	{
		error("Could not create snapshot!");
	}

	// Get the client.dll base address.
	const DWORD client_base_address = get_client_dll_base_address(hSnapshot);
	CloseHandle(hSnapshot);
	if (client_base_address == 0)
	{
		error("Could not get client.dll base address!");
	}

	struct update_pointer_info update_pointer_info;
	update_pointer_info.l4d2_process = l4d2_process;
	update_pointer_info.player_base = client_base_address + player_base_offset;
	update_pointer_info.player_base_pointer = 0;


	// Create and start the update pointer thread.
	HANDLE update_pointer_thread_handle = CreateThread(NULL, 0, update_pointer_thread, &update_pointer_info, 0, NULL);

	// Set the update pointer thread to lowest priority.
	if (SetThreadPriority(update_pointer_thread_handle, THREAD_PRIORITY_LOWEST) == FALSE)
	{
		fprintf(stderr, "Could not set thread priority. Error: 0x%lX\r\nContinuing anyway!\r\n", GetLastError());
	}

	// Wait for the update pointer thread to update the player base pointer.
	while(!update_pointer_info.player_base_pointer)
	{
		puts("Waiting for base pointer to be updated!");
		Sleep(1000);
	}

	puts("Autobhop is now functional!");

	// Bhop loop
	DWORD m_flags = 0;
	while (TRUE)
	{
		// Check if the space key is held down.
		if (GetAsyncKeyState(' ') & 0x8000)
		{
			// Read the mFlags variable from l4d2
			ReadProcessMemory(l4d2_process, (char*)update_pointer_info.player_base_pointer + m_flags_offset, &m_flags, sizeof(m_flags),
			                  NULL);
			
			// I have no idea what these values mean i just copied them from another autobhop program.
			if (m_flags != 0x80 && m_flags != 0x82 && m_flags != 0x280 && m_flags != 0x282)
			{
				SendMessage(l4d2_hwnd, WM_KEYDOWN, ' ', 0x390000);
			}
			else if (m_flags == 0x80 || m_flags == 0x82 || m_flags == 0x280 || m_flags == 0x282)
			{
				SendMessage(l4d2_hwnd, WM_KEYUP, ' ', 0x390000);
			}
		}
		// Yield so we don't take up all cpu resources.
		Sleep(0);
	}
}

/// <summary>
/// Updates the PlayerBase pointer.
/// </summary>
/// <param name="lpParameter">A UpdatePointerInfo pointer</param>
/// <returns>Never returns</returns>
DWORD WINAPI update_pointer_thread(_In_ LPVOID lpParameter)
{
	struct update_pointer_info* info = lpParameter;

	DWORD player_base_pointer = 0;
	while (TRUE)
	{
		ReadProcessMemory(info->l4d2_process, (void*)info->player_base, &player_base_pointer, sizeof(player_base_pointer),
		                  NULL);
		info->player_base_pointer = player_base_pointer;

		// Sleep a long time so we don't take any cpu.
		Sleep(10000);
	}
}
