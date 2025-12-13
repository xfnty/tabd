#ifndef _TABD_PRESET_H
#define _TABD_PRESET_H

#include "util.h"

typedef struct {
    PCWSTR name;
    Rect area;
} Preset;

extern const Preset g_presets[];
extern const int g_preset_count;

Vec2 MapTabletPointToScreen(const Preset *preset, Vec2 p);

#endif /* _TABD_PRESET_H */
