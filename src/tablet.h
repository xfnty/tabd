#ifndef _TABD_TABLETS_H
#define _TABD_TABLETS_H

#include "util.h"

typedef struct {
    Vec2 point;
    DWORD flags;
} TabletReport;

typedef struct {
    const WCHAR *name;
    USHORT vid, pid;
    float aspect_ratio;
    BYTE features[64];
    DWORD features_size;
    bool (*Parse)(BYTE *packet, DWORD size, TabletReport *report);
} TabletInfo;

bool FindTabletInfo(USHORT vid, USHORT pid, TabletInfo *info);

#endif /* _TABD_TABLETS_H */
