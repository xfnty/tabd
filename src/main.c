#include "std.h"

void _start() {
    int n;
    BOOL ok;
    DWORD err;
    HANDLE stdout;
    WCHAR buffer[1024];

    ok = AttachConsole(ATTACH_PARENT_PROCESS);
    err = GetLastError();
    if (!ok && err != ERROR_ACCESS_DENIED) {
        ExitProcess(err);
    }

    stdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if (stdout == INVALID_HANDLE_VALUE) {
        ExitProcess(2);
    }

    n = wnsprintfW(buffer, sizeof(buffer)-1, L"str=%ls int=%d\n", L"string", 123);
    if (n < 0) {
        ExitProcess(n);
    }

    if (!WriteConsoleW(stdout, buffer, n, 0, 0)) {
        ExitProcess(3);
    }

    ExitProcess(0);
}
