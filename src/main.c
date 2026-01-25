#include "std.h"
#include "log.h"
#include "tray.h"

void _start() {
    Log_Init();
    Tray_Init();
    Log(L"Startup finished");

    Tray_Cleanup();
    ExitProcess(0);
}
