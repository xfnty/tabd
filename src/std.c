#include "std.h"

int _fltused = 1;

#define isdigit(c) (c >= '0' && c <= '9')

double atof(const char *s)
{
    double a = 0;
    int e = 0, sign = 1;
    char c;

    c = *s;
    if (c && (c == '+' || c == '-')) {
        if (c == '-') sign = -1;
        s++;
    }

    while ((c = *s++) != '\0' && isdigit(c)) {
        a = a * 10.0 + (c - '0');
    }

    if (c == '.') {
        while ((c = *s++) != '\0' && isdigit(c)) {
            a = a * 10.0 + (c - '0');
            e--;
        }
    }

    while (e < 0) {
        a *= 0.1;
        e++;
    }

    a *= sign;

    return a;
}

int strcmp(const char *s1, const char *s2) {
  char r;
  while (!((r = *s1 - *s2++) != 0 || !*s1++));
  return r;
}

void *memcpy(void *dest, const void *src, size_t n) {
    RtlCopyMemory(dest, src, n);
    return dest;
}
