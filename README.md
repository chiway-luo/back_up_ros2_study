第八阶段(虚拟机端导航实现)
===
第八阶段 4.1.0 导航概述
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
sudo apt install ros-$ROS_DISTRO-navigation2
sudo apt install ros-$ROS_DISTRO-nav2-bringup
```

### 仿真案例启动
```
ros2 launch demo_gazebo_sim gazebo_sim_car.launch.py 
```
```
ros2 run teleop_twist_keyboard teleop_twist_keyboard 
```