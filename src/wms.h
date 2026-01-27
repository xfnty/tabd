#ifndef _WMS_H
#define _WMS_H

#include "std.h"

enum {
    WM_TRAY_EXIT = WM_USER+1,
    WM_TRAY_PRESET,           /* lParam: preset id */
    WM_PRESET_CHANGED,        /**/
    _WM_COUNT
};

#endif
