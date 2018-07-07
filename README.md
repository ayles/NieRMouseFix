# NieRMouseFix

__How to:__ add this library as dependency to __NieRAutomata.exe__ or to any lib it uses.
I prepared one for you (__dxgi.dll__). You can find it [here](https://github.com/ayles/NieRMouseFix/releases).
Put `mouse_patch.dll` along with `dxgi.dll` in game folder and enjoy!

There is one function (not actually one, but...):

```c++
void UpdateCameraRotation(CameraAxesValues<float> *axes_rotation, CameraAxesValues<int> *axes_sensitivity,
                          CameraAxesValues<bool> *axes_invert, bool lock_vertical_axes);
```

### Parameters:

`axes_rotation` - structure with camera vertical and horizontal rotation (in radians).

`axes_sensitivity` - structure with `Vertical Rotation Speed` and `Horizontal Rotation Speed` settings.

`axes_invert` - structure with `Vertical Orientation` and `Horizontal Orientation` settings.

`lock_vertical_axes` - really I don't know what it is used for, so just use it like I do.

Note that camera smoothing still will be applied after, so use `Vertical Auto Adjust` and `Horizontal Auto Adjust` with max values if you want.

### Some info:
In `DllMain` library tries to find function signature in loaded game module. 
It relies on commands sequence so it __isn't really reliable__. 

Since I do not have money for the game so far, I rely only on an unofficial copy, so I do not know if it will work for you. 
Open issue if something does not work.

You can compile it and just put in your game folder (with my `dxgi.dll` or your own). 

Note, you need to compile it as __64-bit__ and it will work only with __Visual Studio compiler__.

For better experience set `Vertical Auto Adjust`, `Horizontal Auto Adjust` and `Pursuit Speed` to their max values.