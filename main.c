#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

#include <TlHelp32.h>
#include <Windows.h>

int main(const int argc, char *argv[]) {
  // Print syntax and exit program if we don't have the required amounts of
  // arguments.
  if (argc != 3) {
    fprintf(stderr, "Syntax: %s <playerbase> <mFlags offset>\r\n", argv[0]);

    return EXIT_FAILURE;
  }

  // Get the player base offset from the first argument.
  // If it fails we print an error and exits.
  DWORD player_base_offset = 0;
  if (sscanf_s(argv[1], "%lX", &player_base_offset) != 1) {
    error("You did not specify playerbase correctly!");
  }

  // Get the mFlags offset from the second argument.
  // If it fails we print an error and exits.
  DWORD m_flags_offset = 0;
  if (sscanf_s(argv[2], "%lX", &m_flags_offset) != 1) {
    error("You did not specify mFlags offset correctly!");
  }

  printf("Using:\r\n  PlayerBase offset = %lX\r\n  mFlags offset = %lX\r\n",
         player_base_offset, m_flags_offset);

  puts("Start the game now.");

  // Keep looping until we find the l4d2 window.
  HWND l4d2_hwnd = NULL;
  while ((l4d2_hwnd = FindWindow(NULL, TEXT("Left 4 Dead 2"))) == NULL) {
    Sleep(1000);
  }

  puts("Game found!");

  // Get the process id from the window.
  DWORD l4d2_process_id = 0;
  GetWindowThreadProcessId(l4d2_hwnd, &l4d2_process_id);

  // Get a handle to the process with virtual memory read permissions.
  HANDLE l4d2_process = OpenProcess(PROCESS_VM_READ, FALSE, l4d2_process_id);
  if (l4d2_process == NULL || l4d2_process_id == 0) {
    error("Could not open process or get process id!");
  }

  // Get a handle for looping through loaded modules in the process.
  HANDLE hSnapshot = CreateToolhelp32Snapshot(
      TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, l4d2_process_id);
  if (hSnapshot == NULL) {
    error("Could not create snapshot!");
  }

  // Get the client.dll base address.
  const DWORD client_base_address = get_client_dll_base_address(hSnapshot);
  CloseHandle(hSnapshot);
  if (client_base_address == 0) {
    error("Could not get client.dll base address!");
  }

  puts("Autobhop is now functional!");

  // Bhop loop
  DWORD player_base_pointer = 0;
  DWORD m_flags = 0;
  while (TRUE) {
    // Check if the space key is held down.
    if (GetAsyncKeyState(' ') & 0x8000) {
      // Read player base pointer
      ReadProcessMemory(
          l4d2_process, (char *)client_base_address + player_base_offset,
          &player_base_pointer, sizeof(player_base_pointer), NULL);
      // Read the mFlags variable from l4d2
      ReadProcessMemory(l4d2_process,
                        (char *)player_base_pointer + m_flags_offset, &m_flags,
                        sizeof(m_flags), NULL);

      // I have no idea what these values mean i just copied them from another
      // autobhop program.
      if (m_flags != 0x80 && m_flags != 0x82 && m_flags != 0x280 &&
          m_flags != 0x282) {
        SendMessage(l4d2_hwnd, WM_KEYDOWN, ' ', 0x390000);
      } else if (m_flags == 0x80 || m_flags == 0x82 || m_flags == 0x280 ||
                 m_flags == 0x282) {
        SendMessage(l4d2_hwnd, WM_KEYUP, ' ', 0x390000);
      }
    }
    // Yield so we don't take up all cpu resources.
    Sleep(0);
  }
}
