#include "updatethread.h"

DWORD WINAPI update_pointer_thread(_In_ LPVOID lpParameter) {
  struct update_pointer_info *info = lpParameter;

  DWORD player_base_pointer = 0;
  while (TRUE) {
    // Read the player base pointer from the l4d2 process.
    ReadProcessMemory(info->l4d2_process, (void *)info->player_base,
                      &player_base_pointer, sizeof(player_base_pointer), NULL);
    info->player_base_pointer = player_base_pointer;

    // Sleep a long time so we don't take any cpu.
    Sleep(10000);
  }
}