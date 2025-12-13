#include "tablet.h"

static bool WacomCTL672PacketParser(BYTE *packet, DWORD size, TabletReport *report);

static const TabletInfo s_tablet_infos[] = {
    { L"Wacom CTL-672", 1386, 891, 216.0f / 135.0f, { 0x02, 0x02 }, 2, WacomCTL672PacketParser },
};

bool FindTabletInfo(USHORT vid, USHORT pid, TabletInfo *info) {
    for (unsigned int i = 0; i < COUNTOF(s_tablet_infos); i++) {
        if (s_tablet_infos[i].vid == vid && s_tablet_infos[i].pid == pid) {
            *info = s_tablet_infos[i];
            return true;
        }
    }
    return false;
}

bool WacomCTL672PacketParser(BYTE *packet, DWORD size, TabletReport *report) {
    if (size != 10 || packet[0] != 0x02 || (packet[1] == 0x00 || packet[1] == 0x80))
        return false;

    report->point.x = *(USHORT*)(packet + 2) / (float)0x5460;
    report->point.y = *(USHORT*)(packet + 4) / (float)0x34BC;
    return true;
}
