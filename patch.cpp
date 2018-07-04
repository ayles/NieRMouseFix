#include "patch.h"

#include <windows.h>

static bool initialized = false;
static HWND main_window_handle;
static DWORD current_process_id;

BOOL enum_windows(HWND hwnd, LPARAM lparam) {
    DWORD windowPID;
    GetWindowThreadProcessId(hwnd, &windowPID);
    if (current_process_id == windowPID) {
        main_window_handle = hwnd;
        return false;
    }
    return true;
}

void initialize() {
    current_process_id = GetCurrentProcessId();
    EnumWindows(enum_windows, 0);
    initialized = true;
}

void rotate_camera(int x_sensitivity, float *x_camera_angle, bool x_invert, int y_sensitivity, float *y_camera_angle,
                   bool y_invert, bool y_lock) {
    if (!initialized) initialize();
    RECT rect;
    GetWindowRect(main_window_handle, &rect);
    auto x_window_center = static_cast<int>(rect.right - (rect.right - rect.left) * 0.5f);
    auto y_window_center = static_cast<int>(rect.bottom - (rect.bottom - rect.top) * 0.5f);
    POINT point = { x_window_center, y_window_center };
    ClientToScreen(main_window_handle, &point);
    int x_center = point.x;
    int y_center = point.y;
    GetCursorPos(&point);
    auto delta_x = point.x - x_center;
    auto delta_y = point.y - y_center;
    auto x_delta_angle = delta_x * (0.001f + 0.002f * x_sensitivity / 20.f);
    auto y_delta_angle = delta_y * (0.001f + 0.002f * y_sensitivity / 20.f);
    *x_camera_angle += x_invert ? x_delta_angle : -x_delta_angle;
    if (y_lock)
        return;
    *y_camera_angle += y_invert ? y_delta_angle : -y_delta_angle;
}