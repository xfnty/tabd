#include "preset.h"

const Preset g_presets[] = {
    { L"Osu",     { 0, 0, 1, 1 } }, /* default */
    { L"Drawing", { 0.75f, 0.25f, 0.25f, 0.75f } },
};

const int g_preset_count = COUNTOF(g_presets);

Vec2 MapTabletPointToScreen(const Preset *preset, Vec2 p) {
    // p.x = (preset->area.right - preset->area.left) * p.x; if (_dsign(p.x)) p.x = 1 + p.x;
    // p.y = (preset->area.bottom - preset->area.top) * p.y; if (_dsign(p.y)) p.y = 1 + p.y;
    return p;
}
