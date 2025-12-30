#install(DIRECTORY config params launch DESTINATION share/${PROJECT_NAME}) #cmake配置

#from glob import glob #用于setup.py配置多个launch文件
#('share/' + package_name, glob('launch/*_launch.py')),
#('share/' + package_name, glob('launch/*_launch.xml')),
#('share/' + package_name, glob('launch/*_launch.yaml')),
"""
    需求: 编写发布robot_1/odom 与 robot_0/odom 相对于 map 的静态坐标系变换发布节点
          
    同时启动仿真环境(多机器人环境)

"""
from launch import LaunchDescription
from launch_ros.actions import Node
import os
# 封装终端指令相关类--------------
# from launch.actions import ExecuteProcess
# from launch.substitutions import FindExecutable   #FindExecutable(name="ros2")
# 参数声明与获取-----------------
# from launch.actions import DeclareLaunchArgument
# from launch.substitutions import LaunchConfiguration
# from launch.conditions import IfCondition #判断是否执行
# from launch.conditions import UnlessCondition #取反
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

    #包含stage_ros2中的my_house_multi.launch.py
    stage_ros2_multi = IncludeLaunchDescription(
        launch_description_source=PythonLaunchDescriptionSource(
            os.path.join(
                get_package_share_directory("stage_ros2"),"launch","my_house_multi.launch.py"
            )
        )
    )
    ld.add_action(stage_ros2_multi)

    robot_0_to_map = Node(
        package="tf2_ros",
        executable="static_transform_publisher",
        name="robot0_to_map_broadcaster",
        arguments=["--frame-id","map","--child-frame-id","robot_0/odom","--x","1","--y","1","--yaw","0.7854"]
    )

    robot1_to_map = Node(
        package="tf2_ros",
        executable="static_transform_publisher",
        name="robot1_to_map_broadcaster",
        arguments=["--frame-id","map","--child-frame-id","robot_1/odom","--x","3","--y","3"]
    )
    ld.add_action(robot_0_to_map)
    ld.add_action(robot1_to_map)


    return ld
