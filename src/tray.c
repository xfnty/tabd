#include "tray.h"

#include "log.h"

static struct {
    HANDLE thread;
    DWORD thread_id;
    NOTIFYICONDATAW icon;
} s_Tray;

static DWORD WINAPI TrayThreadProc(HANDLE thread_ready_event);

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

    PeekMessageW(&m, 0, WM_USER, WM_USER, PM_NOREMOVE);
    Log(L"Tray thread is ready");
    SetEvent(thread_ready_event);

    while (GetMessageW(&m, 0, 0, 0) > 0) {
    }

    Log(L"Tray thread stopped");
    return 0;
}
