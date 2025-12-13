#include "util.h"

int swprintf_s(wchar_t *buffer, size_t size, const wchar_t *format, ...) {
    va_list args;
    va_start(args, format);
    return __stdio_common_vsnwprintf_s(0, buffer, size, size-1, format, 0, args);
}

int vswprintf_s(wchar_t *buffer, size_t size, const wchar_t *format, va_list args) {
    return __stdio_common_vsnwprintf_s(0, buffer, size, size-1, format, 0, args);
}
