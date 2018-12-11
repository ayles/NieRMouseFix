#include <Windows.h>

#pragma pack(push, 4)

template<typename T>
struct CameraAxesValues {
    T vertical;
    T horizontal;
};

#pragma pack(pop)

extern "C" __declspec(dllexport)
void UpdateCameraRotation(CameraAxesValues<float> *axes_rotation, CameraAxesValues<int> *axes_sensitivity,
                          CameraAxesValues<bool> *axes_invert, bool lock_vertical_axes);


extern "C" __declspec(dllexport) BOOL WINAPI DllMain(_In_ HINSTANCE, _In_ DWORD, _In_ LPVOID);