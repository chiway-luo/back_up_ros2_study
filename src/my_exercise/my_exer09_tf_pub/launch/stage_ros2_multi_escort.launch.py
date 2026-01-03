#install(DIRECTORY config params launch DESTINATION share/${PROJECT_NAME}) #cmake配置

#from glob import glob #用于setup.py配置多个launch文件
#('share/' + package_name, glob('launch/*_launch.py')),
#('share/' + package_name, glob('launch/*_launch.xml')),
#('share/' + package_name, glob('launch/*_launch.yaml')),
"""
    三辆车 护航实现
    需求: 编写发布
        robot_1/odom 
        robot_0/odom 
        robot_2/odom
        相对于 map 的静态坐标系变换发布节点
          
    同时启动仿真环境(多机器人环境)

"""
from launch import LaunchDescription
from launch_ros.actions import Node
import os
# 封装终端指令相关类--------------
# from launch.actions import ExecuteProcess
# from launch.substitutions import FindExecutable   #FindExecutable(name="ros2")
# 参数声明与获取-----------------
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
# from launch.conditions import IfCondition #判断是否执行
# from launch.conditions import UnlessCondition #取反
from launch.substitutions import PythonExpression
# 文件包含相关-------------------
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
# 分组相关----------------------
# from launch_ros.actions import PushRosNamespace
# from launch.actions import GroupAction
# 事件相关----------------------
# from launch.event_handlers import OnProcessStart, OnProcessExit
# from launch.actions import ExecuteProcess, RegisterEventHandler,LogInfo
# 获取功能包下share目录路径-------
from ament_index_python.packages import get_package_share_directory
# urdf文件处理相关--------------
# from launch_ros.parameter_descriptions import ParameterValue
# from launch.substitutions import Command

def generate_launch_description():
    ld = LaunchDescription()

    #声明参数
    # middle_distance = 0.5#仿真地图中半个格子的距离
    middle_distance = DeclareLaunchArgument(name="middle_distance",default_value="0.5")
    ld.add_action(middle_distance)

    #包含stage_ros2中的my_house_multi.launch.py
    stage_ros2_multi = IncludeLaunchDescription(
        launch_description_source=PythonLaunchDescriptionSource(
            os.path.join(
                get_package_share_directory("stage_ros2"),"launch","my_house_multi_escort.launch.py"
            )
        )
    )
    ld.add_action(stage_ros2_multi)

    robot0_to_map = Node(
        package="tf2_ros",
        executable="static_transform_publisher",
        name="robot0_to_map_broadcaster",
        # arguments=["--frame-id","map","--child-frame-id","robot_0/odom","--x",str(middle_distance),"--y",str(middle_distance),"--yaw","0.7854"]
        arguments=[
            "--frame-id","map",
            "--child-frame-id","robot_0/odom",
            "--x",LaunchConfiguration("middle_distance"),
            "--y",LaunchConfiguration("middle_distance"),
            "--yaw","0.7854"
        ]
    )
    ld.add_action(robot0_to_map)

    robot1_to_map = Node(
        package="tf2_ros",
        executable="static_transform_publisher",
        name="robot1_to_map_broadcaster",
        # arguments=["--frame-id","map","--child-frame-id","robot_1/odom","--x",str(2 + middle_distance),"--y",str(2 + middle_distance)]
        arguments=[
            "--frame-id","map",
            "--child-frame-id","robot_1/odom",
            "--x",PythonExpression(['str(2 + float(',LaunchConfiguration("middle_distance"),'))']),
            "--y",PythonExpression(['str(2 + float(',LaunchConfiguration("middle_distance"),'))'])
        ]
    )
    ld.add_action(robot1_to_map)

    robot2_to_map = Node(
        package="tf2_ros",
        executable="static_transform_publisher",
        name="robot2_to_map_broadcaster",
        arguments=[
            "--frame-id","map",
            "--child-frame-id","robot_2/odom",
            "--x",LaunchConfiguration("middle_distance"),
            "--y",PythonExpression(['str(3 + float(',LaunchConfiguration("middle_distance"),'))']),
            "--yaw","-0.7854"
        ]
    )
    ld.add_action(robot2_to_map)


    return ld
