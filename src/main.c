#include "std.h"
#include "log.h"

void _start() {
    if (!Log_Init()) {
        ExitProcess(1);
    }

    Log(L"1%ls%d", L"23", 456);

    ExitProcess(0);
}
