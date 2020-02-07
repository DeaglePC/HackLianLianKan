# HackLianLianKan
秒杀连连看

## 背景描述
在知乎上看到个大佬写了个 [Python版本的连连看外挂](https://github.com/TheThreeDog/Auto-Lianliankan)

然后参考他的解决方法，写了个C++版本的，用 windows API实现，图像处理用 GDI

## 使用方法
1. [点我下载](https://github.com/DeaglePC/HackLianLianKan/releases/download/1.1/HackLLK.exe)
2. 右键，管理员运行`HackLLK.exe`
3. 打开连连看，开始游戏
4. 在`输入每步执行间隔(ms)，-1表示结束:`后面输入一个整数表示每次消除方块的时间间隔，时间单位毫秒。比如：0（秒杀），1000左右模仿真人
5. 外挂运行中，可以看到方块正在被自动消除了。

## 此版本特点
1. 不用安装任何环境，双击即可运行（高版本win10安全性较高，可能需要以管理员身份运行）  
2. 游戏窗口可以最小化，因为是直接向窗体发鼠标消息的，不是模拟鼠标点击。也就是不受其他窗体影响。  
3. 无解会自动使用道具继续来.  

只适用于QQ游戏的连连看，
其他版本的的连连看要调整窗口和图片参数。。。

## 注意
本程序只用于技术交流，请勿大肆传播和用作商业用途

## 效果展示
![this is result](https://github.com/DeaglePC/HackLianLianKan/raw/master/picture/1.png)
