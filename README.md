# NieRMouseFix

How it works: I patched the [NierAutomata.exe](https://drive.google.com/open?id=1crEX-hEdkDV8ny7-iBLaCtB7qGPXcrfX) file, now it loads this library and uses it to change rotation of the camera.

There is one function:

```c
  void get_delta_angle(long x_sens, float *x_delta_angle, long y_sens, float *y_delta_angle);
```

`x_sens` and `y_sens` are `Horizontal Rotation Speed` and `Vertical Rotation Speed` settings respectively.

`x_delta_angle` and `y_delta_angle` are pointers to values which will be used to change rotation of the camera (in radians).

You can recompile it and just put in your game folder (with patched `NierAutomata.exe`, of course). Note, you need to compile it as 64-bit. I tested only with the Visual Studio compiler.

For better experience set `Vertical Auto Adjust`, `Horizontal Auto Adjust` and `Pursuit Speed` to max values.
