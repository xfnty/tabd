#include "std.h"

void _start() {
    BOOL ok;
    DWORD err;
    HANDLE stdout;

    ok = AttachConsole(ATTACH_PARENT_PROCESS);
    err = GetLastError();
    if (!ok && err != ERROR_ACCESS_DENIED) {
        ExitProcess(err);
    }

    stdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if (stdout == INVALID_HANDLE_VALUE) {
        ExitProcess(2);
    }

    if (!WriteConsoleW(stdout, L"Hello, World!\n", 14, 0, 0)) {
        ExitProcess(3);
    }

    ExitProcess(0);
}
