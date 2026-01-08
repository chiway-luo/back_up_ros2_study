第六阶段(虚拟机端)
===
## Papar Information
- email : chiwayfpv@gmail.com

## Install & Dependence
- python
- cpp
- cmake


## Use 待添加
- for train
  ```
  python train.py
  ```
- for test
  ```
  python test.py
  ```



## Directory Hierarchy
```
|—— src
|    |—— Stage
|
|    |—— my_exercise
|        |—— exer_from_gpt01
|            |—— CMakeLists.txt
|            |—— include
|                |—— exer_from_gpt01
|            |—— package.xml
|            |—— src
|                |—— Twist_guard.cpp
|        |—— my_exer01_topic_pub
|            |—— CMakeLists.txt
|            |—— include
|                |—— my_exer01_topic_pub
|            |—— package.xml
|            |—— src
|                |—— pub_vel.cpp
|        |—— my_exer02_topic_sub
|            |—— CMakeLists.txt
|            |—— include
|                |—— my_exer02_topic_sub
|            |—— package.xml
|            |—— src
|                |—— sub_odom.cpp
|        |—— my_exer03_service_server
|            |—— CMakeLists.txt
|            |—— include
|                |—— my_exer03_service_server
|            |—— package.xml
|            |—— src
|                |—— cru_server.cpp
|        |—— my_exer04_service_client
|            |—— CMakeLists.txt
|            |—— include
|                |—— my_exer04_service_client
|            |—— package.xml
|            |—— src
|                |—— cru_client.cpp
|        |—— my_exer05_action_server
|            |—— CMakeLists.txt
|            |—— include
|                |—— my_exer05_action_server
|            |—— package.xml
|            |—— src
|                |—— nav_server.cpp
|        |—— my_exer06_action_client
|            |—— CMakeLists.txt
|            |—— include
|                |—— my_exer06_action_client
|            |—— main流程图.mmd
|            |—— package.xml
|            |—— src
|                |—— nav_client.cpp
|                |—— nav_client_study.cpp
|        |—— my_exer07_log
|            |—— CMakeLists.txt
|            |—— include
|                |—— my_exer07_log
|            |—— package.xml
|            |—— src
|                |—— log_demo.cpp
|        |—— my_exer08_launch
|            |—— CMakeLists.txt
|            |—— include
|                |—— my_exer08_launch
|            |—— launch
|                |—— exer00_bringup.launch.py
|                |—— exer01_topic_pub.launch.py
|                |—— exer02_topic_sub.launch.py
|                |—— exer03_service_server.launch.py
|                |—— exer04_action_server.launch.py
|            |—— package.xml
|            |—— src
|        |—— my_exer09_tf_pub
|            |—— CMakeLists.txt
|            |—— include
|                |—— my_exer09_tf_pub
|            |—— launch
|                |—— stage_ros2_multi.launch.py
|                |—— stage_ros2_multi_escort.launch.py
|            |—— package.xml
|            |—— src
|        |—— my_exer10_tf_sub
|            |—— CMakeLists.txt
|            |—— include
|                |—— my_exer10_tf_sub
|            |—— launch
|                |—— car_escort.launch.py
|                |—— car_follow.launch.py
|            |—— package.xml
|            |—— src
|                |—— tf_listener.cpp
|        |—— my_exer_interfaces
|            |—— CMakeLists.txt
|            |—— action
|                |—— Nav.action
|            |—— include
|                |—— my_exer_interfaces
|            |—— package.xml
|            |—— src
|                |—— sdy_node.cpp
|            |—— srv
|                |—— Cru.srv
|        |—— 问题说明
|    |—— mycar_description
|        |—— CMakeLists.txt
|        |—— include
|            |—— mycar_description
|        |—— launch
|            |—— mycar_desc.launch.py
|        |—— mesh
|            |—— mycar_2w.stl
|            |—— mycar_4w.stl
|        |—— package.xml
|        |—— src
|        |—— urdf
|            |—— arduino.urdf
|            |—— stm32_2w.urdf
|            |—— stm32_4w.urdf
|    |—— mycar_description_detailed
|        |—— CMakeLists.txt
|        |—— config
|            |—— description_2.rviz
|        |—— include
|            |—— mycar_description
|        |—— launch
|            |—— mycar_desc.launch.py
|            |—— mycar_desc_sim.launch.py
|        |—— mesh
|            |—— diff_2w
|                |—— base_link.STL
|                |—— camera_link.STL
|                |—— laser_link.STL
|                |—— left_link.STL
|                |—— right_link.STL
|            |—— diff_4w
|                |—— base_link.STL
|                |—— bracket_link.STL
|                |—— bracket_urdf.urdf
|                |—— camera_link.STL
|                |—— laser_link.STL
|                |—— left_former_link.STL
|                |—— left_rear_link.STL
|                |—— right_former_link.STL
|                |—— right_rear_link.STL
|        |—— package.xml
|        |—— src
|        |—— urdf
|            |—— arduino.urdf
|            |—— stm32_2w.urdf
|            |—— stm32_4w.urdf
|            |—— stm32_4w_change.urdf

|    |—— ros2_arduino_bridge
|      
|    |—— ros2_stm32_bridge
|        
|    |—— stage_ros2
|        
```
