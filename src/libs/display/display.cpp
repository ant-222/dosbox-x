/* -------------- Cross platform access to display properties --------------- */
#include "display.h"
#include "SDL_syswm.h"

#if defined(WIN32)
#include "windows.h"
#endif

/* ---------------------- Platform-specific functions ----------------------- */
/* Notation: win_ for Windows, osx_ for Max OS X, lin_ for Linux              */
/* When the conversion is complete, all these functions must be private, i.e. */
/* not published via display.h .                                              */

#if defined(WIN32)
typedef unsigned int (*dpi_for_win_proc)( HWND window );
typedef unsigned int (*dpi_for_sys_proc)( void );

FARPROC win_getproc( char* name )
{   FARPROC res;
    HMODULE mod;

    res = NULL;
    mod = GetModuleHandle("USER32.DLL");
    if( mod != NULL )
    {   res = GetProcAddress( mod, name );  }
    return res;
}

void win_screeninfo(ScreenSizeInfo &info ) {
    info.clear();

# if !defined(HX_DOS)
    int dpi;
    HMONITOR mon;
    HWND hwnd;

    info.method = METHOD_WIN98BASE;

    hwnd = xd_win_hwnd();
    if (hwnd == NULL) return;

    mon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if (mon == NULL) mon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY);
    if (mon == NULL) return;

    MONITORINFO mi;
    memset(&mi,0,sizeof(mi));
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfo(mon,&mi)) return;

    info.screen_position_pixels.x        = mi.rcMonitor.left;
    info.screen_position_pixels.y        = mi.rcMonitor.top;

    info.screen_dimensions_pixels.width  = mi.rcMonitor.right - mi.rcMonitor.left;
    info.screen_dimensions_pixels.height = mi.rcMonitor.bottom - mi.rcMonitor.top;

    dpi = xd_dpi();
    if (dpi != 0) {
        info.screen_dpi.width  = dpi;
        info.screen_dpi.height = dpi;

        info.screen_dimensions_mm.width  = (25.4 * info.screen_dimensions_pixels.width) / dpi;
        info.screen_dimensions_mm.height = (25.4 * info.screen_dimensions_pixels.height) / dpi;
    }
           
# endif !HXDOS
}

# if defined(HX_DOS) || !defined(C_SDL2)
HWND xd_win_hwnd(void) {
    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);

    if(!SDL_GetWMInfo(&wmi)) {
        return NULL;
    }
    return wmi.window;
}

HWND xd_win_surfwnd(void) {
    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);

    if (!SDL_GetWMInfo(&wmi)) {
        return NULL;
    }
    return wmi.child_window;
}
#else
HWND xd_win_hwnd()
{
    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    if (sdl.window == NULL)
        return nullptr;
    if (!SDL_GetWindowWMInfo(sdl.window, &wmi))
        return nullptr;
    return wmi.info.win.window;
}

HWND xd_win_surfwnd()
{
    return GetHWND();
}
#endif

#endif WIN32

/* ------------------------ Cross-platform functions ------------------------ */

void xd_res( int* w, int* h )
{
#if defined(WIN32)
    *w = GetSystemMetrics( 0 );
    *h = GetSystemMetrics( 1 );
#endif
}

int xd_dpi()
{   int              res;
#if defined(WIN32)
    dpi_for_sys_proc for_system;
    dpi_for_win_proc for_window;
    
    for_window = (dpi_for_win_proc)win_getproc( (char*)"GetDpiForWindow" );
    if( for_window != NULL )
    {   res = for_window( xd_win_hwnd() );
        goto Done;
    }

    for_system = (dpi_for_sys_proc)win_getproc( (char*)"GetDpiForSystem" );
    if( for_system != NULL )
    {   res = for_system();
        goto Done;
    }

    res = 0;
#endif
    Done: return res;
}

void xd_screeninfo
(   ScreenSizeInfo *info, 
    unsigned int   *cur_width,
    unsigned int   *cur_height,
    bool           *wmax
)
{   
#if defined(WIN32)
    // When maximized, SDL won't actually tell us our new dimensions, so get it ourselves.
    // FIXME: Instead of xd_win_gethwnd() we need to track our own handle or add something to SDL 1.x
    //        to provide the handle!
    RECT r = { 0 };

    GetClientRect(xd_win_hwnd(), &r);
    *cur_width  = r.right;
    *cur_height = r.bottom;
    *wmax = IsZoomed(xd_win_hwnd());
    win_screeninfo( *info );
#endif
}
