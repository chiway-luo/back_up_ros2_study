#!/usr/bin/python3
# -*- coding: utf-8 -*-
import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.substitutions import LaunchConfiguration, TextSubstitution
from launch.actions import DeclareLaunchArgument, OpaqueFunction, SetLaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():

    this_directory = get_package_share_directory('stage_ros2')

    stage_world_arg = DeclareLaunchArgument(
        'world',
        default_value=TextSubstitution(text='my_house_multi'),
        description='World file relative to the project world file, without .world')

    def stage_world_configuration(context):
        file = os.path.join(
            this_directory,
            'world',
            context.launch_configurations['world'] + '.world')
        return [SetLaunchConfiguration('world_file', file)]

    stage_world_configuration_arg = OpaqueFunction(function=stage_world_configuration)

    #添加两台机器人相对于map的静态坐标系变换 在功能包中单独实现my_exer09_tf_pub
    # robot0_to_map = Node(
    #     package="tf2_ros",
    #     executable="static_transform_publisher",
    #     name="robot0_to_map_broadcaster",
    #     arguments=["--frame-id","map","--child-frame-id","robot_0/odom","--x","1","--y","1","--yaw","0.7854"]
    # )
    # robot1_to_map = Node(
    #     package="tf2_ros",
    #     executable="static_transform_publisher",
    #     name="robot1_to_map_broadcaster",
    #     arguments=["--frame-id","map","--child-frame-id","robot_1/odom","--x","3","--y","3"]
    # )

    return LaunchDescription([
        stage_world_arg,
        stage_world_configuration_arg,
        Node(
            package='stage_ros2',
            executable='stage_ros2',
            name='stage',
            parameters=[{
                "world_file": [LaunchConfiguration('world_file')]}],
            remappings=[("/base_scan","/scan")]
        ),
        Node(#rviz2可视化
            package='rviz2',
            executable='rviz2',
            name='rviz2',
            arguments=['-d', os.path.join(
            this_directory,
            'config/rviz/example_2robot.rviz')],
        ),
        # robot0_to_map,
        # robot1_to_map
    ])
