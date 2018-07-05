#include "patch.h"

#include <windows.h>

static bool initialized = false;
static HWND main_window_handle;
static DWORD current_process_id;

static float x_camera_angle_saved;

BOOL enum_windows(HWND hwnd, LPARAM lparam) {
    DWORD window_process_id;
    GetWindowThreadProcessId(hwnd, &window_process_id);
    // There are 4 windows, wee need only first
    if (current_process_id == window_process_id) {
        main_window_handle = hwnd;
        return false;
    }
    return true;
}

void initialize(const float *x_camera_angle) {
    current_process_id = GetCurrentProcessId();
    EnumWindows(enum_windows, 0);
    x_camera_angle_saved = *x_camera_angle;
    initialized = true;
}

void rotate_camera(int x_sensitivity, float *x_camera_angle, bool x_invert,
                   int y_sensitivity, float *y_camera_angle, bool y_invert, bool y_lock) {

    if (!initialized) initialize(x_camera_angle);

    RECT window_rect;
    GetWindowRect(main_window_handle, &window_rect);

    auto x_window_center = static_cast<int>(window_rect.right - (window_rect.right - window_rect.left) * 0.5f);
    auto y_window_center = static_cast<int>(window_rect.bottom - (window_rect.bottom - window_rect.top) * 0.5f);

    POINT point = {
            x_window_center,
            y_window_center
    };

    // This calculation is wrong. It isn't center of game window,
    // but it is how it calculated in game, so we should follow
    ClientToScreen(main_window_handle, &point);

    int x_mouse_lock = point.x;
    int y_mouse_lock = point.y;

    // Each frame cursor moved from lock position, so we can
    // calculate delta. Then game places cursor back to lock position
    GetCursorPos(&point);

    auto x_delta = point.x - x_mouse_lock;
    auto y_delta = point.y - y_mouse_lock;

    // Calculate delta angle
    auto x_delta_angle = x_delta * (0.001f + 0.003f * x_sensitivity / 20.f);
    auto y_delta_angle = y_delta * (0.001f + 0.003f * y_sensitivity / 20.f);

    // Rotate camera
    // (It's ok to save x angle to prevent game from adjusting our camera,
    // but don't save y angle cause game can lock y rotation forever).
    x_camera_angle_saved += x_invert ? x_delta_angle : -x_delta_angle;
    if (!y_lock) *y_camera_angle += y_invert ? y_delta_angle : -y_delta_angle;
    *x_camera_angle = x_camera_angle_saved;

    // Actually, saving x rotation is very useful when playing,
    // but due to the fact that the camera rotates before smoothing, in cutscenes our camera can still be controlled by the game,
    // and after cutscenes saved x rotation will be set and camera will jerk.
    // There are 2 possible solutions:

    // 1. Modify actual camera rotation and write custom smooth
    // (i don't want do it cause then i will have to watch in disassemled code blah-blah-blah and i'm so lazy)
    // 2. Pass smth like 'cutscene_playing' flag to function. It is easier but still little difficult

    // Also i would like to make it possible to save y angle cause
    // because otherwise run and look around is not very convenient.
    // I will work at it first.
}