#ifndef _TABD_PRESET_H
#define _TABD_PRESET_H

#include "util.h"
#include "tablet.h"

typedef struct {
    Vec2 center;
    Vec2 size;
    float rotation;
} ActiveArea; 

typedef struct {
    PCWSTR name;
    ActiveArea area;
} Preset;

extern const Preset g_presets[];
extern const int g_preset_count;

Vec2 MapTabletPointToScreen(const Preset *preset, const TabletInfo *tablet, Vec2 p);

#endif /* _TABD_PRESET_H */
