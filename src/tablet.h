#ifndef _TABD_TABLETS_H
#define _TABD_TABLETS_H

#include "util.h"

#define TABLET_REPORT_POINTER_DOWN 0x01
#define TABLET_REPORT_BUTTON_DOWN(_n) (1 << (_n) + 1)

typedef struct {
    Vec2 point;
    DWORD flags;
} TabletReport;

typedef struct {
    const WCHAR *name;
    USHORT vid, pid;
    Vec2 measurements;
    BYTE features[64];
    DWORD features_size;
    bool (*Parse)(BYTE *packet, DWORD size, TabletReport *report);
} TabletInfo;

bool FindTabletInfo(USHORT vid, USHORT pid, TabletInfo *info);

#endif /* _TABD_TABLETS_H */
