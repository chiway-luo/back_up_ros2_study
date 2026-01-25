第八阶段(虚拟机端仿真实现-仿真部分实现)
===
第八阶段 3.0.0 第三章仿真引言 开始
## Papar Information
- email : chiwayfpv@gmail.com

## Install & Dependence
- python
- cpp
- cmake

## Use method
- ros2 launch stage_ros2 my_house.launch.py 

## install stage
请先调用如下指令安装依赖：
```
sudo apt-get install git cmake g++ libjpeg8-dev libpng-dev libglu1-mesa-dev libltdl-dev libfltk1.1-dev
```
进入ROS2工作空间的src目录，调用如下指令下载相关仓库：(赵虚左制作)
```
git clone https://github.com/damuxt/Stage.git
git clone https://github.com/damuxt/stage_ros2.git
```

使用命令启动实例节点
```
ros2 launch stage_ros2 my_house.launch.py
```