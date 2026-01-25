#include "log.h"

#define LOG_MESSAGE_BUFFER_SIZE 1024

static struct {
    HANDLE stdout;
} s_Log;

void Log_Init() {
    AttachConsole(ATTACH_PARENT_PROCESS);
    s_Log.stdout = GetStdHandle(STD_OUTPUT_HANDLE);
}

void Log(LPCWSTR format, ...) {
    va_list args;
    int n;
    WCHAR b[LOG_MESSAGE_BUFFER_SIZE];

    va_start(args, format);
    n = wnsprintfW(b, sizeof(b) - 2, L"%04X ", GetCurrentThreadId());
    n += wvnsprintfW(b + n, sizeof(b) - n - 2, format, args);
    b[n++] = L'\n';
    WriteConsoleW(s_Log.stdout, b, n, 0, 0);
}
