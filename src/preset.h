#ifndef _PRESET_H
#define _PRESET_H

#include "std.h"

#define PRESET_NAME_LENGTH 64

#define PRESET_ERROR_NOT_FOUND -1
#define PRESET_ERROR_TOO_MANY  -2

typedef struct {
    WCHAR name[PRESET_NAME_LENGTH];
    float center[2];
    float size[2];
    float rotation;
    bool ink;
} Preset;

int LoadPresetFolder(LPCWSTR path, Preset *array, int max);

#endif
