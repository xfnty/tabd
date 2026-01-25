#ifndef _TRAY_H
#define _TRAY_H

#include "std.h"

#define WM_TRAY_EXIT   (WM_USER+1)
#define WM_TRAY_PRESET (WM_USER+2)

void Tray_Init();
void Tray_Cleanup();

#endif
