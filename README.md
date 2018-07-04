# NieRMouseFix

How it works: I patched the [NieRAutomata.exe](https://drive.google.com/open?id=1QKWGeUwlmyrZCz3OKd7nNDJbKRup2r3J) file, now it loads this library and uses it to change rotation of the camera.

There is one function:

```c++
  rotate_camera(int x_sensitivity, float *x_camera_angle, bool x_invert, int y_sensitivity, float *y_camera_angle,
                bool y_invert, bool y_lock);
```

`x_sensitivity` and `y_sensitivity` are `Horizontal Rotation Speed` and `Vertical Rotation Speed` settings respectively.

`x_camera_angle` and `y_camera_angle` are pointers to camera rotation values (in radians). 
(Note that camera smoothing still will be applied after, so use `Vertical Auto Adjust` and `Horizontal Auto Adjust` with max values if you want)
`x_invert` and `y_invert` - flags indicating whether to invert the value or not (`Horizontal Orientation` and `Vertical Orientation`).
`y_lock` - i do not know what it is used for. Probably, for limiting vertical rotation.

You can recompile it and just put in your game folder (with patched `NieRAutomata.exe`, of course). Note, you need to compile it as 64-bit. I tested only with Visual Studio compiler.

For better experience set `Vertical Auto Adjust`, `Horizontal Auto Adjust` and `Pursuit Speed` to their max values.
