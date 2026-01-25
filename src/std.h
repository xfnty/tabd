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
typedef uint8_t UCHAR;
typedef uint16_t WCHAR, *PWSTR, WORD, USHORT;
typedef const WCHAR *PCWSTR, *LPCWSTR;
typedef int32_t BOOL, LONG;
typedef uint32_t DWORD, *LPDWORD, UINT, ULONG;
typedef int64_t LONG_PTR, LPARAM;
typedef uint64_t ULONG_PTR, SIZE_T, WPARAM;
typedef void *PVOID, *LPVOID, *HANDLE, *HWND, *HICON;

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

typedef struct _GUID {
    ULONG   Data1;
    USHORT  Data2;
    USHORT  Data3;
    UCHAR   Data4[8];
} GUID;

typedef struct _NOTIFYICONDATAW {
    DWORD cbSize;
    HWND hWnd;
    UINT uID;
    UINT uFlags;
    UINT uCallbackMessage;
    HICON hIcon;
    WCHAR szTip[128];
    DWORD dwState;
    DWORD dwStateMask;
    WCHAR szInfo[256];
    union {
        UINT uTimeout;
        UINT uVersion;
    } _U;
    WCHAR szInfoTitle[64];
    DWORD dwInfoFlags;
    GUID guidItem;
    HICON hBalloonIcon;
} NOTIFYICONDATAW, *PNOTIFYICONDATAW;

typedef struct _SECURITY_ATTRIBUTES {
  DWORD  nLength;
  LPVOID lpSecurityDescriptor;
  BOOL   bInheritHandle;
} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;

typedef DWORD (WINAPI *PTHREAD_START_ROUTINE)(LPVOID lpThreadParameter);

typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;

typedef struct tagPOINT {
  LONG x;
  LONG y;
} POINT, *PPOINT, *NPPOINT, *LPPOINT;

typedef struct tagMSG {
  HWND   hwnd;
  UINT   message;
  WPARAM wParam;
  LPARAM lParam;
  DWORD  time;
  POINT  pt;
  DWORD  lPrivate;
} MSG, *PMSG, *NPMSG, *LPMSG;

#define ATTACH_PARENT_PROCESS ((DWORD)-1)
#define STD_OUTPUT_HANDLE     ((DWORD)-11)
#define INVALID_HANDLE_VALUE  ((HANDLE)(LONG_PTR)-1)
#define ERROR_ACCESS_DENIED   0x5
#define INFINITE              0xFFFFFFFF
#define WAIT_OBJECT_0         0x00000000L
#define WM_USER               0x0400
#define WM_QUIT               0x0012
#define PM_NOREMOVE           0x0000

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
HANDLE CreateThread(
    LPSECURITY_ATTRIBUTES   lpThreadAttributes,
    SIZE_T                  dwStackSize,
    LPTHREAD_START_ROUTINE  lpStartAddress,
    LPVOID                  lpParameter,
    DWORD                   dwCreationFlags,
    LPDWORD                 lpThreadId
);
HANDLE CreateEventW(PVOID lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCWSTR lpName);
BOOL SetEvent(HANDLE hEvent);
DWORD WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds);

/* shlwapi.dll */
int wnsprintfW(PWSTR buffer, int maxsize, PCWSTR format, ...); /* doesn't support %f or %p */
int wvnsprintfW(PWSTR buffer, int maxsize, PCWSTR format, va_list args);

/* user32.dll */
BOOL PeekMessageW(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);
BOOL GetMessageW(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax);
BOOL PostThreadMessageW(DWORD idThread, UINT Msg, WPARAM wParam, LPARAM lParam);

#endif
