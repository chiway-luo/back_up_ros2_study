#install(DIRECTORY config params launch DESTINATION share/${PROJECT_NAME}) #cmake配置
#<exec_depend>ros2launch</exec_depend> #package.xml配置
#from glob import glob #用于setup.py配置多个launch文件
#('share/' + package_name, glob('launch/*_launch.py')),
#('share/' + package_name, glob('launch/*_launch.xml')),
#('share/' + package_name, glob('launch/*_launch.yaml')),
"""
    特殊说明:课程中使用电脑链接实体机器人进行本地控制或仿真环境启动,但是我使用的是树莓派链接小车远程控制,
    所以本代码没有涉及本地启动实体机器人的相关代码,只启动仿真环境和本地控制节点
"""
from launch import LaunchDescription
from launch_ros.actions import Node
# 封装终端指令相关类--------------
from launch.actions import ExecuteProcess
# from launch.substitutions import FindExecutable   #FindExecutable(name="ros2")
# 参数声明与获取-----------------
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch.conditions import IfCondition
from launch.conditions import UnlessCondition #取反判断
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
"""
    需求:
        在该launch文件汇总,需要桥洞仿真环境或者实体机器人,然后再启动运动控制节点
    分析:
        判断当前环境是仿真还是实体
        本地还是远程
"""
import os
def generate_launch_description():
    ld = LaunchDescription()

    exer00_bringup = IncludeLaunchDescription(
        launch_description_source=PythonLaunchDescriptionSource(
            os.path.join(
                get_package_share_directory('my_exer08_launch'),'launch','exer00_bringup.launch.py' 
            )
        )
    )
    #运动控制
    pub_vel_node = Node(
        package='my_exer01_topic_pub',
        executable='pub_vel',
    )


    
    return LaunchDescription([
        exer00_bringup,
        pub_vel_node,
    ])