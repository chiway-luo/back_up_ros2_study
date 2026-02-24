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

## 仿真案例启动
- ign-gazbeo
```
ros2 launch demo_gazebo_sim gazebo_sim_car.launch.py 
```
```
ros2 run teleop_twist_keyboard teleop_twist_keyboard 
```
- stage multi_laser_car
```
ros2 launch demo_stage_sim sim.launch.py 
```
- stage house
```
ros2 launch stage_ros2 my_house.launch.py 
```

## 安装 slam_toolbox
```
cd src/
git clone https://github.com/SteveMacenski/slam_toolbox.git -b humble
```
或者
```
sudo apt install ros-$ROS_DISTRO-slam-toolbox
```

### 说明
```
ros2 pkg executables slam_toolbox 
```
其中包括以下节点：
- 用于异步SLAM的async_slam_toolbox_node

        slam_toolbox async_slam_toolbox_node

- 用于定位的localization_slam_toolbox_node

        slam_toolbox localization_slam_toolbox_node

- 用于同时进行建图和定位的map_and_localization_slam_toolbox_node

        slam_toolbox map_and_localization_slam_toolbox_node
- 用于合并运动学地图的merge_maps_kinematic

        slam_toolbox merge_maps_kinematic

- 用于同步SLAM的sync_slam_toolbox_node

        slam_toolbox sync_slam_toolbox_node

## 安装cartographer
- 核心库
```
sudo apt install ros-$ROS_DISTRO-cartographer     
```
- 桥接库
```
sudo apt install ros-$ROS_DISTRO-cartographer-ros
```

### 说明
cartographer_node
```
主要负责订阅来自各种传感器的数据,并基于这些数据实时构建丢,采用子图的方法来逐步构建和更新地图
```
cartographer_occupancy_grid_node
```
主要负责接收cartographer_node生成的子图列表(/submap_list)并将其拼接成完整的占用栅格地图(/map)
```

## 多车编队启动
其中包含导航和跟随实现
```
ros2 launch stage_ros2 sim_follow.launch.py 
```