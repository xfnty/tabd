#include "std.h"
#include "log.h"
#include "wms.h"
#include "tray.h"
#include "preset.h"

#define MAIN_MAX_PRESETS    32
#define MAIN_PRESETS_FOLDER L"presets"

void _start() {
    int exit_code = 0;
    MSG m;
    DWORD tid;
    Preset presets[MAIN_MAX_PRESETS];
    int i, preset_count, preset_ids[MAIN_MAX_PRESETS];

    tid = GetCurrentThreadId();
    PeekMessageW(&m, 0, WM_USER, WM_USER, PM_NOREMOVE);

    Log_Init();
    Tray_Init(tid);

    preset_count = LoadPresetFolder(MAIN_PRESETS_FOLDER, presets, MAIN_MAX_PRESETS);
    if (preset_count == PRESET_ERROR_NOT_FOUND) {
        Log(L"Failed to load presets from \"%ls\" folder", MAIN_PRESETS_FOLDER);
    } else if (preset_count == PRESET_ERROR_TOO_MANY) {
        Log(L"Too many presets in \"%ls\" folder", MAIN_PRESETS_FOLDER);
        exit_code = preset_count;
        goto Exit;
    }

    for (i = 0; i < preset_count; i++) {
         Log(
            L"%ls: cx=%d cy=%d w=%d h=%d r=%d ink=%d",
            presets[i].name,
            (int)presets[i].center[0],
            (int)presets[i].center[1],
            (int)presets[i].size[0],
            (int)presets[i].size[1],
            (int)presets[i].rotation,
            presets[i].ink
        );
        ASSERT((preset_ids[i] = Tray_AddPreset(presets[i].name)) >= 0);
    }
    if (preset_count > 0) {
        Tray_SetActivePreset(preset_ids[0]);
    }

    while (GetMessageW(&m, 0, 0, 0) > 0 && m.message != WM_TRAY_EXIT) {
        if (m.message == WM_TRAY_PRESET) {
            Tray_SetActivePreset((int)m.lParam);
        }
    }

    Exit:
    Tray_Cleanup();
    ExitProcess(exit_code);
}
