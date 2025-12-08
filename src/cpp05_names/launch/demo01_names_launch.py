from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():

    #修改节点名称
    # return LaunchDescription([
    #     Node(package="turtlesim",executable="turtlesim_node",name="sdy"),
    #     Node(package="turtlesim",executable="turtlesim_node",namespace="sdy"),
    #     Node(package="turtlesim",executable="turtlesim_node",name="sdy_turtle1",namespace="sdy")
    # ])

    #修改话题名称
    return LaunchDescription([
        # Node(package="turtlesim",executable="turtlesim_node",namespace="sdy"),
        Node(package="turtlesim",executable="turtlesim_node",
             remappings=[("/turtle1/cmd_vel","/sdy_control")]),
        # Node(package="teleop_twist_keyboard",executable="teleop_twist_keyboard",
        #      remappings=[("/cmd_vel","sdy_control")])
    ])




"""
chiway@chiway-ros2-humble:~$ ros2 node list
/sdy
/sdy/sdy_turtle1
/sdy/turtlesim
"""