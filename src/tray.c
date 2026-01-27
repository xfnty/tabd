#include "tray.h"

#include "log.h"
#include "wms.h"
#include "res.h"
#include "preset.h"

#define TRAY_MAX_PRESET_COUNT   64
#define TRAY_WNDCLASSNAME       L"tabd/tray_window"
#define WM_TRAY_WM              (WM_USER+1)
#define WM_TRAY_MENU            (WM_USER+2)
#define TRAY_MENU_PRESET_ITEM_0 100
#define TRAY_MENU_EXIT_ITEM     1

static struct {
    HANDLE thread;
    DWORD thread_id;
    DWORD reciever_thread_id;
    HANDLE hwnd;
    HMENU menu;
    NOTIFYICONDATAW icon;

    CRITICAL_SECTION cs;
    WCHAR preset_names[PRESET_NAME_LENGTH][TRAY_MAX_PRESET_COUNT];
    int preset_ids[TRAY_MAX_PRESET_COUNT];
    int active_preset_id;
    int preset_id_counter;
} s_Tray;

static DWORD WINAPI TrayThreadProc(HANDLE thread_ready_event);
static LRESULT WINAPI TrayWindowEventHandler(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

void Tray_Init(DWORD reciever_thread_id) {
    HANDLE thread_ready_event;

    InitializeCriticalSection(&s_Tray.cs);
    s_Tray.reciever_thread_id = reciever_thread_id;
    thread_ready_event = CreateEventW(0, false, false, 0);
    s_Tray.thread = CreateThread(0, 0, TrayThreadProc, thread_ready_event, 0, &s_Tray.thread_id);
    ASSERT(WaitForSingleObject(thread_ready_event, INFINITE) == WAIT_OBJECT_0);
}

void Tray_Cleanup() {
    PostThreadMessageW(s_Tray.thread_id, WM_QUIT, 0, 0);
    WaitForSingleObject(s_Tray.thread, INFINITE);
}

int Tray_AddPreset(LPCWSTR name) {
    int i, l, id = TRAY_ERROR_TOO_MANY_PRESETS;

    l = lstrlenW(name);
    if (l > PRESET_NAME_LENGTH - 1) {
        return TRAY_ERROR_NAME_TOO_LONG;
    }

    EnterCriticalSection(&s_Tray.cs);
    for (i = 0; i < TRAY_MAX_PRESET_COUNT; i++) {
        if (s_Tray.preset_names[i][0] == L'\0') {
            ASSERT(lstrcpynW(s_Tray.preset_names[i], name, l + 1));
            id = s_Tray.preset_ids[i] = s_Tray.preset_id_counter;
            Log(L"Added \"%ls\" preset", s_Tray.preset_names[i]);
            s_Tray.preset_id_counter++;
            break;
        }
    }
    LeaveCriticalSection(&s_Tray.cs);

    return id;
}

void Tray_SetActivePreset(int id) {
    int i = 0;

    EnterCriticalSection(&s_Tray.cs);
    if (id != s_Tray.active_preset_id) {
        for (; i < TRAY_MAX_PRESET_COUNT; i++) {
            if (s_Tray.preset_names[i][0] != L'\0' && s_Tray.preset_ids[i] == id) {
                s_Tray.active_preset_id = id;
                Log(L"Activated \"%ls\" preset", s_Tray.preset_names[i]);
                break;
            }
        }
    }
    LeaveCriticalSection(&s_Tray.cs);

    ASSERT(i < TRAY_MAX_PRESET_COUNT);
}

void Tray_RemovePreset(int id) {
    int i = 0;

    EnterCriticalSection(&s_Tray.cs);
    if (s_Tray.active_preset_id == id) {
        s_Tray.active_preset_id = 0;
    }
    for (; i < TRAY_MAX_PRESET_COUNT; i++) {
        if (s_Tray.preset_names[i][0] != L'\0' && s_Tray.preset_ids[i] == id) {
            Log(L"Removed \"%ls\" preset", s_Tray.preset_names[i]);
            s_Tray.preset_names[i][0] = L'\0';
            s_Tray.preset_ids[i] = 0;
            break;
        }
    }
    LeaveCriticalSection(&s_Tray.cs);

    ASSERT(i < TRAY_MAX_PRESET_COUNT);
}

DWORD WINAPI TrayThreadProc(HANDLE thread_ready_event) {
    MSG m;
    WNDCLASSEXW wndclass;

    PeekMessageW(&m, 0, WM_USER, WM_USER, PM_NOREMOVE);

    wndclass.cbSize = sizeof(wndclass);
    wndclass.hInstance = GetModuleHandleW(0);
    wndclass.lpszClassName = TRAY_WNDCLASSNAME;
    wndclass.lpfnWndProc = TrayWindowEventHandler;
    ASSERT(RegisterClassExW(&wndclass));

    s_Tray.hwnd = CreateWindowExW(0, TRAY_WNDCLASSNAME, 0, 0, 0, 0, 0, 0, 0, 0, wndclass.hInstance, 0);
    ASSERT(s_Tray.hwnd);

    s_Tray.icon.cbSize = sizeof(s_Tray.icon);
    s_Tray.icon.hWnd = s_Tray.hwnd;
    s_Tray.icon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    s_Tray.icon.uCallbackMessage = WM_TRAY_WM;
    s_Tray.icon.hIcon = LoadIconW(wndclass.hInstance, MAKEINTRESOURCEW(IDI_TRAYICON_OFFLINE));
    wnsprintfW(s_Tray.icon.szTip, sizeof(s_Tray.icon.szTip), L"tabd");
    ASSERT(Shell_NotifyIconW(NIM_ADD, &s_Tray.icon));

    SetEvent(thread_ready_event);

    while (GetMessageW(&m, 0, 0, 0) > 0) {
        if (m.hwnd) {
            TranslateMessage(&m);
            DispatchMessageW(&m);
        } else if (m.message == WM_TRAY_MENU) {
            POINT cursor;
            UINT flags;
            int i, choice, id;
            bool has_presets = false;

            DestroyMenu(s_Tray.menu);
            s_Tray.menu = CreatePopupMenu();
            EnterCriticalSection(&s_Tray.cs);
            for (i = 0; i < TRAY_MAX_PRESET_COUNT; i++) {
                if (s_Tray.preset_names[i][0] != L'\0') {
                    has_presets = true;
                    flags = MF_STRING;
                    if (s_Tray.preset_ids[i] == s_Tray.active_preset_id) {
                        flags |= MF_CHECKED;
                    }
                    AppendMenuW(
                        s_Tray.menu,
                        flags,
                        TRAY_MENU_PRESET_ITEM_0 + s_Tray.preset_ids[i],
                        s_Tray.preset_names[i]
                    );
                }
            }
            LeaveCriticalSection(&s_Tray.cs);
            if (!has_presets) {
                AppendMenuW(s_Tray.menu, MF_STRING | MF_GRAYED | MF_DISABLED, 0, L"No presets");
            }
            AppendMenuW(s_Tray.menu, MF_SEPARATOR, 0, 0);
            AppendMenuW(s_Tray.menu, MF_STRING, TRAY_MENU_EXIT_ITEM, L"Exit");

            GetCursorPos(&cursor);
            SetForegroundWindow(s_Tray.hwnd);
            choice = TrackPopupMenuEx(
                s_Tray.menu, TPM_RETURNCMD | TPM_NONOTIFY, cursor.x, cursor.y, s_Tray.hwnd, 0
            );
            id = choice - TRAY_MENU_PRESET_ITEM_0;

            if (choice == TRAY_MENU_EXIT_ITEM) {
                PostThreadMessageW(s_Tray.reciever_thread_id, WM_TRAY_EXIT, 0, 0);
            } else if (choice >= TRAY_MENU_PRESET_ITEM_0 && id != s_Tray.active_preset_id) {
                PostThreadMessageW(s_Tray.reciever_thread_id, WM_TRAY_PRESET, 0, id);
            }
        }
    }

    EnterCriticalSection(&s_Tray.cs);
    DestroyMenu(s_Tray.menu);
    Shell_NotifyIconW(NIM_DELETE, &s_Tray.icon);
    DestroyWindow(s_Tray.hwnd);
    UnregisterClassW(TRAY_WNDCLASSNAME, wndclass.hInstance);
    LeaveCriticalSection(&s_Tray.cs);

    DeleteCriticalSection(&s_Tray.cs);
    return 0;
}

LRESULT WINAPI TrayWindowEventHandler(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_TRAY_WM && (LOWORD(lp) == WM_LBUTTONDOWN || LOWORD(lp) == WM_RBUTTONDOWN)) {
        PostThreadMessageW(s_Tray.thread_id, WM_TRAY_MENU, 0, 0);
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}
