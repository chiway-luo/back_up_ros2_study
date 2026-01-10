案例00: 里程计数据转换路径显示
===
## Use
- for run
  ```
  该launch文件集成了stage_ros2仿真环境和里程计转换路径节点(包含rviz配置文件)
  ros2 launch cpp00_odom odom2path.launch.py
  ```
- for control
  ```
  ros2 run teleop_twist_keyboard teleop_twist_keyboard
  ```
- for param
  ```
  rqt  调节param
  ```
## Code Details
### odom2path.cpp

```
订阅里程计话题,转换为路径信息并发布
```

#### 持续发布模式
```
持续订阅里程计话题,每次收到消息后更新路径并发布
```

#### 距离触发式发布模式
```
订阅里程计话题,每次收到消息后计算与上次发布位置的距离,如果是第一次发布(poses容器为空),则直接发布路径,否则如果距离大于设定的阈值,则更新路径并发布
```

#### 路径存储数量限制(0-无限制,>0-限制数量)
```
在类继承时,设置动态参数可以修改
Node(str1,rclcpp::NodeOptions().use_intra_process_comms(true))
在回调函数中,判断路径的poses容器大小是否超过设定的最大值,如果超过则删除最早的位姿信息

```
---
## 参数信息
- sub_topic : 订阅的里程计话题名称
- pub_topic : 发布的路径话题名称
- frame_id : 路径相对于的坐标系
- pub_mode_distance : 发布模式,0-持续发布,>0-距离触发式发布
- path_size : 路径最大存储数量,0-不限制
## Install & Dependence
- launch(python)
- cpp
- cmake



## Directory Hierarchy
```
|—— CMakeLists.txt
|—— config
|    |—— odom2path.rviz
|—— include
|    |—— cpp00_odom
|—— launch
|    |—— odom2path.launch.py
|—— package.xml
|—— src
|    |—— odom2path.cpp
```

---