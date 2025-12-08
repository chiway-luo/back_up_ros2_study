from launch import LaunchDescription
from launch_ros.actions import Node
"""
    ros2 service type /spawn 
turtlesim/srv/Spawn

 ros2 interface show turtlesim/srv/Spawn
float32 x
float32 y
float32 theta
string name # Optional.  A unique name will be created and returned if this is empty
---
string name
"""

def generate_launch_description():
    #启动一个turtlesim节点和一个自定义服务端节点
    n1 = Node(package='turtlesim',executable='turtlesim_node')

    n2 = Node(package='cpp07_exercise',executable='exer03_spawn_server')

    return LaunchDescription(
        [
            n1,
            n2
        ]
    )