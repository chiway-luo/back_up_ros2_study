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
    use_sim_time = LaunchConfiguration('use_sim_time',  default='true')

    stage_world_arg = DeclareLaunchArgument(#地图参数
        'world',
        default_value=TextSubstitution(text='my_house'),
        description='World file relative to the project world file, without .world')

    def stage_world_configuration(context):
        file = os.path.join(
            this_directory,
            'world',
            context.launch_configurations['world'] + '.world')
        return [SetLaunchConfiguration('world_file', file)]

    stage_world_configuration_arg = OpaqueFunction(function=stage_world_configuration)

    #添加map相对于odom的静态变换
    map_to_odom = Node(
        package="tf2_ros",
        executable="static_transform_publisher",
        name="map_to_odom",
        arguments=["--frame-id","map","--child-frame-id","odom","--x","1","--y","1","--yaw","0.786"]
    )
    
    # rviz 参数（关键：把 -d 的路径变成可传参）
    rviz_config_arg = DeclareLaunchArgument(
        'rviz_config',
        default_value=TextSubstitution(
            text=os.path.join(this_directory, 'config', 'rviz', 'example.rviz')
        ),
        description='Absolute path to rviz config file'
    )

    return LaunchDescription([
        stage_world_arg,
        stage_world_configuration_arg,
        rviz_config_arg,
        Node(
            package='stage_ros2',
            executable='stage_ros2',
            name='stage',
            parameters=[{
                "world_file": [LaunchConfiguration('world_file')]}],
            remappings=[("/base_scan","/scan")]
        ),
        Node(
            package='rviz2',
            executable='rviz2',
            name='rviz2',
            arguments=['-d',LaunchConfiguration('rviz_config')],
        ),
        map_to_odom
    ])
