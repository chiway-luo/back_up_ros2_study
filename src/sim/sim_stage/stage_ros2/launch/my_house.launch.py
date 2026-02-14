#!/usr/bin/python3
# -*- coding: utf-8 -*-
import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.substitutions import LaunchConfiguration, TextSubstitution
from launch.actions import DeclareLaunchArgument, OpaqueFunction, RegisterEventHandler, SetLaunchConfiguration, TimerAction
from launch.event_handlers import OnProcessStart
from launch_ros.actions import Node

from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource

from rclpy.parameter import ParameterValue
"""
    该launch文件是启动仿真环境的示例文件

    为适配远程开发环境的调试,增加分部 launch 防止服务被延迟,导致服务启动失败

    1 stage
    2 rviz
    3 导航
"""

def generate_launch_description():
    ld = LaunchDescription()

    this_directory = get_package_share_directory('stage_ros2')
    ld.add_action(DeclareLaunchArgument('use_sim_time',  default_value='True'))

    ld.add_action(DeclareLaunchArgument(
        'world',
        default_value=TextSubstitution(text='my_house'),
        description='World file relative to the project world file, without .world')
    )

    def stage_world_configuration(context):
        file = os.path.join(
            this_directory,
            'world',
            context.launch_configurations['world'] + '.world')
        return [SetLaunchConfiguration('world_file', file)]

    stage_world_configuration_arg = OpaqueFunction(function=stage_world_configuration)
    ld.add_action(stage_world_configuration_arg)

    stage_ros2_launch = Node(
            package='stage_ros2',
            executable='stage_ros2',
            name='stage',
            parameters=[
                {"world_file": [LaunchConfiguration('world_file')]},
                {'enable_gui': False},#在word文件中去除了摄像头
            ],
            remappings=[("/base_scan","/scan")]
    )
    rviz2 = Node(
            package='rviz2',
            executable='rviz2',
            name='rviz2',
            arguments=['-d', os.path.join(
            this_directory,
            # 'config/rviz/example.rviz')],
            'config/rviz/nav2_default_view.rviz')],
    )
    # 导航总实现
    sim_local_launch = IncludeLaunchDescription(
        launch_description_source=PythonLaunchDescriptionSource(
            os.path.join(
                get_package_share_directory('sim_navigation2'),
                'launch',
                'bringup.launch.py'
            )
        )
    )
    ld.add_action(stage_ros2_launch)
    ld.add_action(
        RegisterEventHandler(
            OnProcessStart(
                target_action=stage_ros2_launch,
                on_start=[
                    TimerAction(period=5.0, actions=[rviz2])
                ]
            )
        )
    )
    ld.add_action(
        RegisterEventHandler(
            OnProcessStart(
                target_action=rviz2,
                on_start=[sim_local_launch]
            )
        )
    )

    return ld
