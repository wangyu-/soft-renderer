# mini-soft-renderer

![image](https://user-images.githubusercontent.com/4922024/107134415-d2d2ba80-68bf-11eb-8453-500f37cd30ea.png)

A software 3D model renderer of 2k lines of code, features include:

* Reading in OBJ model
* Ray tracing
* Phong smoothing
* Bump mapping

All features are implemented from scratch, `xlib` is the only dependent library. `xlib` is used only for drawing pixels.

# How to use

##### 1 build with make:
```
make
```
(you might need to install `libx11-dev` before this step)
##### 2 run the program:
```
./render model/teapot.obj 800x600               # 800x600 is the resolution, can be omitted
```

##### 3 controls
```
use `wsad` and `q` `e` to rotate about 3 axes
use ARROW keys to move
use `+` `-` to scale
use `r` to toggle ray tracing
use `p` to toggle phong soomthing
use `b` to toggle bump mapping
use 'z' to toggle between wireframe/z-buffer mode
use 'v' to toggle between orthogonal/perspective view
```

# Screenshots
![image](https://user-images.githubusercontent.com/4922024/107134810-6fe32280-68c3-11eb-891f-b994babae3c5.png)
![image](https://user-images.githubusercontent.com/4922024/107134779-272b6980-68c3-11eb-8239-e6289b14e30d.png)
![image](https://user-images.githubusercontent.com/4922024/107134849-bdf82600-68c3-11eb-9b9e-ac679a939b37.png)

![image](https://user-images.githubusercontent.com/4922024/107134926-5393b580-68c4-11eb-8c12-80e6a03fedd2.png)
![image](https://user-images.githubusercontent.com/4922024/107134791-4629fb80-68c3-11eb-9c75-9f57a1c3dc57.png)


![image](https://user-images.githubusercontent.com/4922024/107134822-7ffb0200-68c3-11eb-9e5a-6f3e4d515e30.png)
![image](https://user-images.githubusercontent.com/4922024/107134825-8f7a4b00-68c3-11eb-92fe-561d1359ef46.png)

![image](https://user-images.githubusercontent.com/4922024/107134830-9acd7680-68c3-11eb-8324-a3ecf33d2791.png)


![image](https://user-images.githubusercontent.com/4922024/107134853-c7818e00-68c3-11eb-9520-bb60d25d461a.png)
![image](https://user-images.githubusercontent.com/4922024/107134863-d5cfaa00-68c3-11eb-99a0-43569b0b21ea.png)
![image](https://user-images.githubusercontent.com/4922024/107134884-00216780-68c4-11eb-8ce3-99572ae77a81.png)
![image](https://user-images.githubusercontent.com/4922024/107134871-e4b65c80-68c3-11eb-9ad8-d1b545e3098b.png)


