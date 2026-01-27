#include "std.h"
#include "log.h"
#include "tray.h"

void _start() {
    MSG m;
    DWORD tid;
    int preset_id;

    tid = GetCurrentThreadId();
    PeekMessageW(&m, 0, WM_USER, WM_USER, PM_NOREMOVE);

    Log_Init();
    Tray_Init(tid);

    ASSERT((preset_id = Tray_AddPreset(L"Drawing")) >= 0);
    ASSERT((preset_id = Tray_AddPreset(L"Osu")) >= 0);
    Tray_SetActivePreset(preset_id);

    while (GetMessageW(&m, 0, 0, 0) > 0 && m.message != WM_TRAY_EXIT) {
        if (m.message == WM_TRAY_PRESET) {
            preset_id = (int)m.lParam;
            Tray_SetActivePreset(preset_id);
        }
    }

    Tray_Cleanup();
    ExitProcess(0);
}
