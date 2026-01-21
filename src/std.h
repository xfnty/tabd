#ifndef _WINDOWS_H
#define _WINDOWS_H

#if !defined(_MSC_VER) && !defined(__GNUC__) && !defined(__TINYC__)
    #error Only MSVC, GCC and TCC compilers are supported
#endif

#if !((defined(_MSC_VER) && defined(_M_X64)) || defined(__x86_64__))
    #error Only 64-bit architecture is supported
#endif

#ifdef _MSC_VER
    #define WINAPI __stdcall
#else
    #define WINAPI __attribute__((stdcall))
#endif

#if defined(_MSC_VER)
    typedef __int8  int8_t;
    typedef __int16 int16_t;
    typedef __int32 int32_t;
    typedef __int64 int64_t;
    typedef unsigned __int8  uint8_t;
    typedef unsigned __int16 uint16_t;
    typedef unsigned __int32 uint32_t;
    typedef unsigned __int64 uint64_t;
#else
    typedef char  int8_t;
    typedef short int16_t;
    typedef int   int32_t;
    typedef unsigned char  uint8_t;
    typedef unsigned short uint16_t;
    typedef unsigned int   uint32_t;
    #if defined(__GNUC__)
        __extension__ typedef long long int64_t;
        __extension__ typedef unsigned long long uint64_t;
    #elif defined(__TINYC__)
        typedef long long int64_t;
        typedef unsigned long long uint64_t;
    #endif
#endif

typedef int32_t BOOL;
typedef uint32_t DWORD, *LPDWORD;
typedef int64_t LONG_PTR;
typedef void *LPVOID, *HANDLE;
typedef uint16_t *PCWSTR;

#define ATTACH_PARENT_PROCESS ((DWORD)-1)
#define STD_OUTPUT_HANDLE     ((DWORD)-11)
#define INVALID_HANDLE_VALUE  ((HANDLE)(LONG_PTR)-1)
#define ERROR_ACCESS_DENIED   0x5

DWORD GetLastError(void);
BOOL WINAPI AttachConsole(DWORD dwProcessId);
HANDLE WINAPI GetStdHandle(DWORD nStdHandle);
BOOL WINAPI WriteConsoleW(
    HANDLE  hConsoleOutput,
    PCWSTR  buffer,
    DWORD   length,
    LPDWORD written,
    LPVOID  reserved
);
int ExitProcess(int code);

#endif
