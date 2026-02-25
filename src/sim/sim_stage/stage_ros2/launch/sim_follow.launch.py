#!/usr/bin/python3
# -*- coding: utf-8 -*-
import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.substitutions import LaunchConfiguration, TextSubstitution
from launch.actions import DeclareLaunchArgument, OpaqueFunction, SetLaunchConfiguration
from launch_ros.actions import Node

from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
"""
    yellow robot_1 main
    blue robot_2 follower
    red robot_0 follower

    启动仿真环境
    启动地图服务
    启动每一辆车的定位功能

"""

def generate_launch_description():
    ld = LaunchDescription()

    #参数声明
    ld.add_action(DeclareLaunchArgument('use_sim_time', default_value='true'))
    ld.add_action(DeclareLaunchArgument('map_filename', default_value='map/stage_map.yaml'))




    this_directory = get_package_share_directory('stage_ros2')

    ld.add_action(DeclareLaunchArgument(
        'world',
        default_value=TextSubstitution(text='sim_follow'),
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
    ld.add_action(
        Node(
            package='stage_ros2',
            executable='stage_ros2',
            name='stage',
            parameters=[{
                "world_file": [LaunchConfiguration('world_file')]}],
            remappings=[
                ("robot_0/base_scan", "robot_0/scan"),
                ("robot_1/base_scan", "robot_1/scan"),
                ("robot_2/base_scan", "robot_2/scan")
            ]
        )
    )

    rviz2 = Node(
            package='rviz2',
            executable='rviz2',
            name='rviz2',
            arguments=['-d', os.path.join(
            this_directory,
            # 'config/rviz/example.rviz')],
            'config/rviz/follow_car.rviz')],
    )
    ld.add_action(rviz2)



    # 上方为仿真环境的启动
    # 下方为导航和编队的功能实现

    # 定位实现 在导航launch中包含了
    sim_local_launch = IncludeLaunchDescription(
        launch_description_source=PythonLaunchDescriptionSource(
            os.path.join(
                get_package_share_directory('sim_localization'),
                'launch',
                'sim_multi_follow_loca.launch.py'
            )
        ),
        launch_arguments={
            'use_sim_time': LaunchConfiguration('use_sim_time'),
            'yaml_filename': LaunchConfiguration('map_filename')
        }.items()
    )
    ld.add_action(sim_local_launch)

    robots = ['robot_0', 'robot_1', 'robot_2']
    # for robot in robots:
    #     ld.add_action(
    #         IncludeLaunchDescription(
    #             PythonLaunchDescriptionSource(
    #                 os.path.join(
    #                     get_package_share_directory('sim_localization'),
    #                     'launch',
    #                     'sim_multi_follow_loca.launch.py'
    #                 )
    #             ),
    #             launch_arguments={
    #                 'car_num': robot,
    #                 'use_sim_time': True
    #             }.items()
    #         )
    #     )

    # 导航实现
    for robot in robots:
        ld.add_action(
            IncludeLaunchDescription(
                PythonLaunchDescriptionSource(
                    os.path.join(
                        get_package_share_directory('sim_navigation2'),
                        'launch',
                        'multi',
                        'bringup_multi.launch.py'
                    )
                ),
                launch_arguments={
                    'car_num': robot,
                    'use_sim_time': LaunchConfiguration('use_sim_time'),
                }.items()
            )
        )

    # 跟随实现
    convoy_node = IncludeLaunchDescription(
        launch_description_source=PythonLaunchDescriptionSource(
            os.path.join(
                get_package_share_directory('mycar_multi'),
                'launch',
                'convoy.launch.py'
            )
        )
    )
    ld.add_action(convoy_node)




    # 发布目标点(静态变换)
    pub_tf_red_goal = Node(
        package='tf2_ros',
        executable='static_transform_publisher',
        name='pub_tf_red_goal',
        arguments=[
            '--frame-id', 'robot_1/base_link',
            '--child-frame-id', 'robot_0/goal',
            '--x', '0.0',
            '--y', '0.0',
            '--z', '0.5',
            '--roll', '0.0',
            '--pitch', '0.0',
            '--yaw', '0.0'
        ]
    )
    # ld.add_action(pub_tf_red_goal)

    pub_tf_blue_goal = Node(
        package='tf2_ros',
        executable='static_transform_publisher',
        name='pub_tf_blue_goal',
        arguments=[
            '--frame-id', 'robot_1/base_link',
            '--child-frame-id', 'robot_2/goal',
            '--x', '0.0',
            '--y', '0.0',
            '--z', '0.5',
            '--roll', '0.0',
            '--pitch', '0.0',
            '--yaw', '0.0'
        ]
    )
    # ld.add_action(pub_tf_blue_goal)

    return ld
