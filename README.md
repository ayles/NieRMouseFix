# NieRMouseFix

How it works: I patched the [NieRAutomata.exe](https://drive.google.com/open?id=1QKWGeUwlmyrZCz3OKd7nNDJbKRup2r3J) file, now it loads this library and uses it to change rotation of the camera.

There is one function:

```c++
rotate_camera(int x_sensitivity, float *x_camera_angle, bool x_invert, 
              int y_sensitivity, float *y_camera_angle, bool y_invert, bool y_lock);
```

###Parameters:

`x_sensitivity` and `y_sensitivity` are `Horizontal Rotation Speed` and `Vertical Rotation Speed` settings respectively.

`x_camera_angle` and `y_camera_angle` are pointers to camera rotation values (in radians). 
(Note that camera smoothing still will be applied after, so use `Vertical Auto Adjust` and `Horizontal Auto Adjust` with max values if you want)

`x_invert` and `y_invert` - flags indicating whether to invert the value or not (`Horizontal Orientation` and `Vertical Orientation`).

`y_lock` - it is used for limiting vertical camera rotation i think.

### Some info:
You can recompile it and just put in your game folder (with patched `NieRAutomata.exe`, of course). 
Note, you need to compile it as 64-bit. It was tested only with Visual Studio compiler.

For better experience set `Vertical Auto Adjust`, `Horizontal Auto Adjust` and `Pursuit Speed` to their max values.

###Thoughts&Plans:

Actually, saving `x_camera_angle` is very useful when playing,
but due to the fact that the camera rotates before smoothing, in cutscenes our camera can still be controlled by the game,
and after cutscenes saved `x_camera_angle` will be set and camera will jerk.

There are 2 possible solutions to solve this:
            
1. Modify actual camera rotation and write custom smooth
(i don't want to cause i'm so lazy and blah-blah-blah)
2. Pass smth like `cutscene_playing` flag to function. It is easier but still little difficult

Also i would like to make it possible to save `y_camera_angle` because otherwise "run and look around" is not very convenient.
I will work at it first.