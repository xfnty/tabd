#ifndef _STD_H
#define _STD_H

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

/* stdlib types */
#if defined(_MSC_VER)
    typedef __int8  int8_t;
    typedef __int16 int16_t;
    typedef __int32 int32_t;
    typedef __int64 int64_t;
    typedef unsigned __int8  uint8_t;
    typedef unsigned __int16 uint16_t;
    typedef unsigned __int32 uint32_t;
    typedef unsigned __int64 uint64_t;
    typedef void *va_list;
    #define va_start(_list, _arg) __va_start(&(_list), (_arg))
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
    typedef char *va_list;
    #define va_start(_list, _arg) __builtin_va_start((_list), (_arg))
#endif

typedef uint8_t bool;
#define true 1
#define false 0

#define ASSERT(_expr) do { if (!(_expr)) ExitProcess(-1); } while(0)

/* WinAPI types */
typedef uint16_t WCHAR, *PWSTR, WORD;
typedef const WCHAR *PCWSTR;
typedef int32_t BOOL, LONG;
typedef uint32_t DWORD, *LPDWORD;
typedef int64_t LONG_PTR;
typedef uint64_t ULONG_PTR;
typedef void *LPVOID, *HANDLE;

typedef struct _LIST_ENTRY _LIST_ENTRY;

typedef struct _LIST_ENTRY {
   struct _LIST_ENTRY *Flink;
   struct _LIST_ENTRY *Blink;
} LIST_ENTRY, *PLIST_ENTRY, *PRLIST_ENTRY;

typedef struct _RTL_CRITICAL_SECTION _RTL_CRITICAL_SECTION;

typedef struct _RTL_CRITICAL_SECTION_DEBUG {
    WORD Type;
    WORD CreatorBackTraceIndex;
    struct _RTL_CRITICAL_SECTION *CriticalSection;
    LIST_ENTRY ProcessLocksList;
    DWORD EntryCount;
    DWORD ContentionCount;
    DWORD Flags;
    WORD  CreatorBackTraceIndexHigh;
    WORD  SpareWORD;
} RTL_CRITICAL_SECTION_DEBUG, *PRTL_CRITICAL_SECTION_DEBUG;

#pragma pack(push, 8)
typedef struct _RTL_CRITICAL_SECTION {
    PRTL_CRITICAL_SECTION_DEBUG DebugInfo;
    LONG LockCount;
    LONG RecursionCount;
    HANDLE OwningThread;
    HANDLE LockSemaphore;
    ULONG_PTR SpinCount;
} RTL_CRITICAL_SECTION, *PRTL_CRITICAL_SECTION;

typedef RTL_CRITICAL_SECTION CRITICAL_SECTION, *LPCRITICAL_SECTION;
#pragma pack(pop)

#define ATTACH_PARENT_PROCESS ((DWORD)-1)
#define STD_OUTPUT_HANDLE     ((DWORD)-11)
#define INVALID_HANDLE_VALUE  ((HANDLE)(LONG_PTR)-1)
#define ERROR_ACCESS_DENIED   0x5

/* kernel32.dll */
DWORD GetLastError();
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
void InitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
void EnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
void LeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
void DeleteCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
DWORD GetCurrentThreadId();

/* shlwapi.dll */
int wnsprintfW(PWSTR buffer, int maxsize, PCWSTR format, ...); /* NOTE: doesn't support %f or %p */
int wvnsprintfW(PWSTR buffer, int maxsize, PCWSTR format, va_list args);

#endif
