from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    turtle = Node(package='turtlesim',executable='turtlesim_node')
    n1 = Node(package='cpp07_exercise',executable='exer07_color_param')
    return LaunchDescription(
        [
            turtle,
            n1
        ]
    )