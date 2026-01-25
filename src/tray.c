#include "tray.h"

#include "log.h"

#define TRAY_WNDCLASSNAME L"tabd/tray_window"

static struct {
    HANDLE thread;
    DWORD thread_id;
    HANDLE hwnd;
    NOTIFYICONDATAW icon;
} s_Tray;

static DWORD WINAPI TrayThreadProc(HANDLE thread_ready_event);
static LRESULT WINAPI TrayWindowEventHandler(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

void Tray_Init() {
    HANDLE thread_ready_event;

    thread_ready_event = CreateEventW(0, false, false, 0);
    s_Tray.thread = CreateThread(0, 0, TrayThreadProc, thread_ready_event, 0, &s_Tray.thread_id);
    ASSERT(WaitForSingleObject(thread_ready_event, INFINITE) == WAIT_OBJECT_0);
}

void Tray_Cleanup() {
    Log(L"Requesting tray thread to stop");
    PostThreadMessageW(s_Tray.thread_id, WM_QUIT, 0, 0);
    WaitForSingleObject(s_Tray.thread, INFINITE);
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

    Log(L"Tray thread is ready");
    SetEvent(thread_ready_event);

    while (GetMessageW(&m, 0, 0, 0) > 0) {
        if (m.hwnd) {
            TranslateMessage(&m);
            DispatchMessageW(&m);
        }
    }

    DestroyWindow(s_Tray.hwnd);
    UnregisterClassW(TRAY_WNDCLASSNAME, wndclass.hInstance);
    Log(L"Tray thread stopped");
    return 0;
}

LRESULT WINAPI TrayWindowEventHandler(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    return DefWindowProcW(hwnd, msg, wp, lp);
}
