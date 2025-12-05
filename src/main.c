#include "windows.h"

#define countof(_a) (sizeof(_a)/sizeof((_a)[0]))

typedef struct {
    float x, y;
    float pressure;
    BYTE down : 1;
    BYTE buttons[2];
} PenInput;

void _start(void);
static void WriteConsoleAFormat(HANDLE hConsole, const char *format, ...);
static HANDLE FindHID(USHORT vid, USHORT pid);
static BOOL ReadPenInput(HANDLE device, PenInput *input);

void _start(void) {
    AttachConsole(ATTACH_PARENT_PROCESS);
    HANDLE hcon = GetStdHandle(STD_OUTPUT_HANDLE);

    HANDLE device = FindHID(1386, 891);
    if (device == INVALID_HANDLE_VALUE) {
        WriteConsoleAFormat(hcon, "Tablet not connected.\n");
        ExitProcess(1);
    }

    if (!HidD_SetFeature(device, (BYTE[]){ 0x02, 0x02 }, 2)) {
        WriteConsoleAFormat(hcon, "Failed to set features on device (%d).\n", GetLastError());
        ExitProcess(2);
    }

    for (PenInput pi = {0}, ppi = {0}; ReadPenInput(device, &pi); ppi = pi) {
        INPUT input = {
            .type = INPUT_MOUSE,
            .mi = (MOUSEINPUT){
                .dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE
                    | ((pi.down && !ppi.down) ? (MOUSEEVENTF_LEFTDOWN) : (0))
                    | ((!pi.down && ppi.down) ? (MOUSEEVENTF_LEFTUP) : (0)),
                .dx = (LONG)(pi.x * 65535),
                .dy = (LONG)(pi.y * 65535),
            }
        };
        SendInput(1, &input, sizeof(input));
    }

    CloseHandle(device);
    ExitProcess(0);
}

void WriteConsoleAFormat(HANDLE hConsole, const char *format, ...) {
    if (hConsole == INVALID_HANDLE_VALUE)
        return;

    char buffer[2048];
    va_list args;
    va_start(args, format);
    int l = __stdio_common_vsnprintf_s(
        0,
        buffer,
        sizeof(buffer),
        sizeof(buffer) - 1,
        format,
        0,
        args
    );
    va_end(args);

    WriteConsoleA(hConsole, buffer, l, 0, 0);
}

HANDLE FindHID(USHORT vid, USHORT pid) {
    HANDLE dev = INVALID_HANDLE_VALUE;

    HDEVINFO devs = SetupDiGetClassDevsA(
        &GUID_DEVINTERFACE_HID,
        0,
        0,
        DIGCF_DEVICEINTERFACE | DIGCF_PRESENT
    );
    if (devs == INVALID_HANDLE_VALUE)
        return INVALID_HANDLE_VALUE;

    for (int i = 0; ; i++) {
        SP_DEVICE_INTERFACE_DATA iface_data = { .cbSize = sizeof(SP_DEVICE_INTERFACE_DATA) };
        SetupDiEnumDeviceInterfaces(devs, 0, &GUID_DEVINTERFACE_HID, i, &iface_data);
        if (GetLastError() == ERROR_NO_MORE_ITEMS)
            break;

        char details_buffer[1024] = {0};
        PSP_DEVICE_INTERFACE_DETAIL_DATA_A details = (void*)details_buffer;
        details->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_A);
        SetupDiGetDeviceInterfaceDetailA(devs, &iface_data, details, 1023, 0, 0);

        dev = CreateFileA(
            details->DevicePath,
            GENERIC_READ,
            0,
            0,
            OPEN_EXISTING,
            FILE_FLAG_OVERLAPPED,
            0
        );
        if (dev == INVALID_HANDLE_VALUE)
            goto Continue;

        HIDD_ATTRIBUTES attrs = { .Size = sizeof(attrs) };
        if (!HidD_GetAttributes(dev, &attrs))
            goto Continue;

        if (attrs.VendorID == vid && attrs.ProductID == pid)
            break;

        Continue:
        CloseHandle(dev);
        dev = INVALID_HANDLE_VALUE;
    }

    return dev;
}

BOOL ReadPenInput(HANDLE device, PenInput *input) {
    Start:
    unsigned char buffer[10] = {0};

    OVERLAPPED ol = {0};
    DWORD bytes_read = 0;
    BOOL ok = ReadFile(device, buffer, sizeof(buffer), &bytes_read, &ol);
    DWORD err = GetLastError();
    if (!ok && err != ERROR_IO_PENDING)
        return 0;

    GetOverlappedResult(device, &ol, &bytes_read, 1);

    if (buffer[0] != 0x02 || buffer[1] == 0x00 || buffer[1] == 0x80)
        goto Start;

    *input = (PenInput){
        .x = (float)(*(unsigned short*)(buffer + 2)) / 21600.0f,
        .y = (float)(*(unsigned short*)(buffer + 4)) / 13500.0f,
        .pressure = *(unsigned short*)(buffer + 6),
        .down = buffer[1] & 0x01,
        .buttons = { buffer[1] & 0x02, buffer[1] & 0x04 },
    };
    return 1;
}
