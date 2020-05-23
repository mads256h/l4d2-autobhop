#ifndef UPDATETHREAD_H
#define UPDATETHREAD_H
#include <Windows.h>

/// <summary>
/// Updates the PlayerBase pointer.
/// </summary>
/// <param name="lpParameter">A update_pointer_info pointer</param>
/// <returns>Never returns</returns>
DWORD WINAPI update_pointer_thread(_In_ LPVOID lpParameter);

/// <summary>
/// Holds pointers for the UpdatePointerThread.
/// </summary>
struct update_pointer_info {
  // In
  HANDLE l4d2_process;
  DWORD player_base;

  // Out
  volatile DWORD
      player_base_pointer; // update_pointer_thread writes to this pointer.
};

#endif
