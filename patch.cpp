#define _USE_MATH_DEFINES
#define NOMINMAX

#include "patch.h"

#include <distorm.h>

#include <string>
#include <cmath>
#include <regex>
#include <psapi.h>
#include <windows.h>

typedef unsigned char ubyte;

static HWND game_window_handle;

BOOL EnumerateWindows(HWND hwnd, LPARAM lparam) {
    DWORD window_process_id;
    GetWindowThreadProcessId(hwnd, &window_process_id);
    // There are 4 windows, wee need first
    if (GetCurrentProcessId() == window_process_id) {
        game_window_handle = hwnd;
        return false;
    }
    return true;
}

void Initialize() {
    static bool initialized = false;
    if (initialized) return;
    EnumWindows(EnumerateWindows, 0);
    initialized = true;
}

void UpdateCameraRotation(CameraAxesValues<float> *axes_rotation, CameraAxesValues<int> *axes_sensitivity,
                          CameraAxesValues<bool> *axes_invert, bool lock_vertical_axes) {
    Initialize();

    static CameraAxesValues<float> axes_rotation_saved = *axes_rotation;

    RECT window_rect;
    GetWindowRect(game_window_handle, &window_rect);

    auto x_window_center = window_rect.right - (window_rect.right - window_rect.left) / 2;
    auto y_window_center = window_rect.bottom - (window_rect.bottom - window_rect.top) / 2;

    POINT cursor_lock_position = {
        x_window_center,
        y_window_center
    };

    // This calculation is wrong. It isn't center of game window,
    // but it is how it calculated in game, so we should follow
    ClientToScreen(game_window_handle, &cursor_lock_position);

    POINT cursor_position;

    // Each frame cursor moved from lock position, so we can
    // calculate delta. After that game will place cursor back to lock position
    GetCursorPos(&cursor_position);

    POINT delta_position {
        cursor_position.x - cursor_lock_position.x,
        cursor_position.y - cursor_lock_position.y
    };

    // Calculate delta angle
    auto delta_angle_x = delta_position.x * (0.001f + 0.003f * axes_sensitivity->horizontal / 20.f);
    auto delta_angle_y = delta_position.y * (0.001f + 0.003f * axes_sensitivity->vertical / 20.f);

    // Rotate camera
    // (It's ok to save x and y angles to prevent game from adjusting our camera).
    axes_rotation_saved.horizontal += axes_invert->horizontal ? delta_angle_x : -delta_angle_x;
    axes_rotation->horizontal = axes_rotation_saved.horizontal;

    if (!lock_vertical_axes) {
        axes_rotation_saved.vertical += axes_invert->vertical ? delta_angle_y : -delta_angle_y;

        // clamp between [-0.4 * PI, 0.4 * PI] (as in game code)
        axes_rotation_saved.vertical = std::min(axes_rotation_saved.vertical, (float)(M_PI * 0.4));
        axes_rotation_saved.vertical = std::max(axes_rotation_saved.vertical, -(float)(M_PI * 0.4));

        axes_rotation->vertical = axes_rotation_saved.vertical;
    }
}

long pattern_instructions[] {
        0x8B, -1, -1, -1, -1, -1,                           
        0xF3, -1, 0x0F, 0x10, -1, -1, -1, -1, -1,
        0x83, -1, 0x0A,
        0x7E, -1,
        0x83, -1, 0xF6,
        0x66, 0x0F, 0x6E, -1,
        0x0F, 0x5B, -1,
        0xF3, 0x0F, -1, -1, -1, -1, -1, -1,
        0xF3, 0x0F, -1, -1,
        0xEB, -1
};

ubyte inject_instructions[] {
        0xEB, 0x10,                                         // jmp 0x10             (jump over data)
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     //                      (sensitivity_struct pointer)
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     //                      (UpdateCameraRotation pointer)
        0x50,                                               // push rax             (save registers)
        0x51,                                               // push rcx
        0x52,                                               // push rdx
        0x41, 0x50,                                         // push r8
        0x41, 0x51,                                         // push r9
        0x48, 0x8D, 0x05, 0xE2, 0xFF, 0xFF, 0xFF,           // lea rax, [rip-0x1e]  (mov data pointer to rax)
        0x48, 0x8D, 0x8B, 0xF0, 0x03, 0x00, 0x00,           // lea rcx, [rbx+0x3f0] (1 param)
        0x48, 0x8B, 0x10,                                   // mov rdx, [rax]       (2 param)
        0x4C, 0x8D, 0x83, 0xD4, 0x05, 0x00, 0x00,           // lea r8, [rbx+0x5d4]  (3 param)
        0x41, 0x83, 0xFE, 0x00,                             // cmp r14d, 0x0        (assume r14 holds inverted y lock)
        0x41, 0x0F, 0x94, 0xC6,                             // setz r14b            (invert it)
        0x4D, 0x89, 0xF1,                                   // mov r9, r14          (4 param)
        0x48, 0x83, 0xEC, 0x20,                             // sub rsp, 0x20        (create 32 byte shadow space)
        0x48, 0x83, 0xC0, 0x08,                             // add rax, 0x8         (move rax to func ptr)
        0xFF, 0x10,                                         // call [rax]           (call it)
        0x48, 0x83, 0xC4, 0x20,                             // add rsp, 0x20        (clean up stack)
        0x41, 0x59,                                         // pop r9               (restore registers)
        0x41, 0x58,                                         // pop r8
        0x5A,                                               // pop rdx
        0x59,                                               // pop rcx
        0x58,                                               // pop rax
        0xF3, 0x0F, 0x10, 0x83, 0xF4, 0x03, 0x00, 0x00      // movss xmm0, [rbx+0x3f4]  (let game clamp rotation to [-PI; PI])
};

ubyte nop_instruction = 0x90;

const ubyte *FindPattern(const long *pattern, const int pattern_size, const ubyte *start, const size_t size) {
    const ubyte *end = start + size;
    for (const ubyte *p = start; p < end; ++p) {
        if ((const ubyte)pattern[0] != *p) continue;
        const ubyte *temp_p = p;
        for (int i = 1; i < pattern_size; ++i) {
            ++temp_p;
            if (pattern[i] >= 0 && (const ubyte)pattern[i] != *temp_p) break;
            if (i == pattern_size - 1) return p;
        }
    }
    return nullptr;
}

BOOL WINAPI DllMain(_In_ HINSTANCE, _In_ DWORD reason, _In_ LPVOID) {
    // Inject only on first load
    if (reason != DLL_PROCESS_ATTACH)
        return true;

    // Query information about our .exe
    MODULEINFO module_info;
    GetModuleInformation(GetCurrentProcess(), GetModuleHandle("NieRAutomata.exe"), &module_info, sizeof(module_info));

    auto module_base_ptr = static_cast<const ubyte *>(module_info.lpBaseOfDll);
    auto module_end_ptr = module_base_ptr + module_info.SizeOfImage;
    auto current_ptr = module_base_ptr;

    const ubyte *pattern_instructions_ptr = nullptr;

    // Find our wildcard in .exe memory
    while (current_ptr < module_end_ptr) {
        MEMORY_BASIC_INFORMATION memory_info;
        VirtualQueryEx(GetCurrentProcess(), current_ptr, &memory_info, sizeof(memory_info));

        current_ptr = static_cast<const ubyte *>(memory_info.BaseAddress) + memory_info.RegionSize;

        if (memory_info.State == MEM_COMMIT && memory_info.Type == MEM_IMAGE &&
            memory_info.Protect == PAGE_EXECUTE_WRITECOPY) {

            pattern_instructions_ptr = FindPattern(pattern_instructions, sizeof(pattern_instructions) / sizeof(long),
                                                   static_cast<const ubyte *>(memory_info.BaseAddress),
                                                   memory_info.RegionSize);
            if (pattern_instructions_ptr != nullptr) {
                break;
            }
        }
    }

    if (pattern_instructions_ptr == nullptr) return false;

    // Use disassembler to find some values and first call instruction
    const unsigned int instructions_count = 200;
    _DecodedInst decoded_instructions[instructions_count];
    unsigned int decoded_instructions_count = 0;
    distorm_decode(0, pattern_instructions_ptr, 10000000, Decode64Bits, decoded_instructions,
                              instructions_count, &decoded_instructions_count);

    auto sensitivity_struct_ptr = pattern_instructions_ptr + decoded_instructions[1].offset +
            *reinterpret_cast<int *>(const_cast<ubyte *>(&pattern_instructions_ptr[2]));

    // Set function pointer and sensitivity_struct pointer
    auto inject_instructions_data_ptr = reinterpret_cast<uint64_t *>(&inject_instructions[2]);
    inject_instructions_data_ptr[0] = reinterpret_cast<uint64_t>(sensitivity_struct_ptr);
    inject_instructions_data_ptr[1] = reinterpret_cast<uint64_t>(UpdateCameraRotation);

    // Inject instruction to process
    WriteProcessMemory(GetCurrentProcess(), (LPVOID)pattern_instructions_ptr, inject_instructions,
                       sizeof(inject_instructions), nullptr);

    auto w2s = [](_WString w_string) -> std::string {
        return std::string(reinterpret_cast<const char *>(w_string.p), w_string.length);
    };

    const unsigned char *call_instruction_ptr = nullptr;

    // Find first call instruction
    for (auto i = 0u; i < decoded_instructions_count; ++i) {
        if (w2s(decoded_instructions[i].mnemonic).find("CALL") != std::string::npos) {
            call_instruction_ptr = pattern_instructions_ptr + decoded_instructions[i].offset;
            break;
        }
    }

    if (call_instruction_ptr == nullptr) return false;

    // Fill all bytes before call instruction with NOPes
    for (auto i = pattern_instructions_ptr + sizeof(inject_instructions); i < call_instruction_ptr; ++i) {
        *const_cast<unsigned char *>(i) = nop_instruction;
    }

    return true;
}