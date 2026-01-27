#ifndef _TRAY_H
#define _TRAY_H

#include "std.h"

#define WM_TRAY_EXIT   (WM_USER+1)
#define WM_TRAY_PRESET (WM_USER+2) /* lParam: preset id */

#define TRAY_ERROR_NAME_TOO_LONG    -1
#define TRAY_ERROR_TOO_MANY_PRESETS -2

void Tray_Init(DWORD reciever_thread_id);
void Tray_Cleanup();

int  Tray_AddPreset(LPCWSTR name); /* >=0: preset id, <0: error */
void Tray_SetActivePreset(int id);
void Tray_RemovePreset(int id);

#endif
