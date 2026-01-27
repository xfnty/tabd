#include "std.h"
#include "log.h"
#include "tray.h"

void _start() {
    MSG m;
    DWORD tid;

    tid = GetCurrentThreadId();
    PeekMessageW(&m, 0, WM_USER, WM_USER, PM_NOREMOVE);

    Log_Init();
    Tray_Init(tid);
    Log(L"Startup finished");

    while (GetMessageW(&m, 0, 0, 0) > 0 && m.message != WM_TRAY_EXIT) {
    }

    Tray_Cleanup();
    ExitProcess(0);
}
