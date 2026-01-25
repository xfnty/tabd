#include "log.h"

#define LOG_MESSAGE_BUFFER_SIZE 1024

static struct {
    HANDLE stdout;
} s_Log;

bool Log_Init() {
    BOOL ok;

    ok = AttachConsole(ATTACH_PARENT_PROCESS);
    if (!ok && GetLastError() != ERROR_ACCESS_DENIED) {
        return false;
    }

    s_Log.stdout = GetStdHandle(STD_OUTPUT_HANDLE);
    return true;
}

void Log(const WCHAR *format, ...) {
    va_list args;
    int n;
    WCHAR b[LOG_MESSAGE_BUFFER_SIZE];

    va_start(args, format);
    n = wvnsprintfW(b, sizeof(b)-1, format, args);
    WriteConsoleW(s_Log.stdout, b, n, 0, 0);
}
