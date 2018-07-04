#include "mouse_patch.h"
#include <windows.h>
#include <windowsx.h>

static BOOL initialized = FALSE;
static HWND main_window_handle;
static DWORD current_process_id;

static int delta_x;
static int delta_y;

int counter = 0;

BOOL enum_windows(HWND hwnd, LPARAM lparam) {
    DWORD windowPID;
    GetWindowThreadProcessId(hwnd, &windowPID);
    if (current_process_id == windowPID) {
        counter++;
        main_window_handle = hwnd;
    }

    return counter != 1;
}

void initialize() {
    current_process_id = GetCurrentProcessId();
    EnumWindows(enum_windows, 0);
    initialized = TRUE;
}

void get_delta_angle(long x_sens, float *x_delta_angle, long y_sens, float *y_delta_angle) {
    if (!initialized) initialize();
    RECT rect;
    GetWindowRect(main_window_handle, &rect);
    int xc = rect.left + (rect.right - rect.left) / 2;
    int yc = rect.top + (rect.bottom - rect.top) / 2;
    POINT point;
    GetCursorPos(&point);
    delta_x = point.x - xc;
    delta_y = point.y - yc;
    *x_delta_angle = delta_x * (0.001f + 0.002f * x_sens / 20.f);
    *y_delta_angle = delta_y * (0.001f + 0.002f * y_sens / 20.f);
    delta_x = 0;
    delta_y = 0;
}