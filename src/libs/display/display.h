/* TODO: hide any OS-specific functions under OS-agnostic abstractions */
#if defined(WIN32)
#include "windows.h"
#endif

#ifndef XDISPLAY_H
#define XDISPLAY_H


enum method {
    METHOD_NONE = 0,
    METHOD_X11,
    METHOD_XRANDR,
    METHOD_WIN98BASE,
    METHOD_COREGRAPHICS
};

// Screen DPI and size info
class ScreenSizeInfo {
public:
    struct wxh {
        double      width = -1;
        double      height = -1;

        void clear(void) {
            width = height = -1;
        }
    };
    struct xvy {
        double      x = 0;
        double      y = 0;

        void clear(void) {
            x = y = 0;
        }
    };
public:
    xvy             screen_position_pixels;     // position of the screen on the "virtual" overall desktop
    wxh             screen_dimensions_pixels;   // size of the screen in pixels
    wxh             screen_dimensions_mm;       // size of the screen in mm
    wxh             screen_dpi;                 // DPI of the screen
    enum method     method = METHOD_NONE;
public:
    void clear(void) {
        screen_dpi.clear();
        screen_dimensions_mm.clear();
        screen_dimensions_pixels.clear();
        screen_position_pixels.clear();
        method = METHOD_NONE;
    }
};

void xd_res( int* w, int* h );

/* Get the DPI if afailable. Returns 0 if not. */
int  xd_dpi();

void xd_screeninfo
(   ScreenSizeInfo *info, 
    unsigned int   *cur_width,
    unsigned int   *cur_height,
    bool           *wmax
);
#if defined(WIN32)
    HWND xd_win_hwnd(void);
    HWND xd_win_surfwnd(void);
#endif

#endif
