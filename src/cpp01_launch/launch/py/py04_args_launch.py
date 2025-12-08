#install(DIRECTORY launch DESTINATION share/PROJECT_NAME)
#<exec_depend>ros2launch</exec_depend>
#from glob import glob 用于setup.py配置多个launch文件
#('share/' + package_name, glob('launch/py/*_launch.py')),

from launch import LaunchDescription
from launch_ros.actions import Node
# 封装终端指令相关类--------------
from launch.actions import ExecuteProcess
# from launch.substitutions import FindExecutable
# 参数声明与获取-----------------
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
# 文件包含相关-------------------
# from launch.actions import IncludeLaunchDescription
# from launch.launch_description_sources import PythonLaunchDescriptionSource
# 分组相关----------------------
# from launch_ros.actions import PushRosNamespace
# from launch.actions import GroupAction
# 事件相关----------------------
# from launch.event_handlers import OnProcessStart, OnProcessExit
# from launch.actions import ExecuteProcess, RegisterEventHandler,LogInfo
# 获取功能包下share目录路径-------
# from ament_index_python.packages import get_package_share_directory

"""
    需求:在launch文件启动时动态的设置turtlesim_node 的背景色
    实现:
        1.声明参数 (变量)
        2.调用参数
        3.执行launch文件时动态导入参数
"""

def generate_launch_description():

    rgb_r = DeclareLaunchArgument(name="sdy_r",default_value="255")
    rgb_g = DeclareLaunchArgument(name="sdy_g",default_value="255")
    rgb_b = DeclareLaunchArgument(name="sdy_b",default_value="255")
    #调用参数
    turtle1 = Node(
        package='turtlesim',
        executable='turtlesim_node',
        parameters=[
            {"background_r": LaunchConfiguration("sdy_r"),
            "background_g": LaunchConfiguration("sdy_g"),
            "background_b": LaunchConfiguration("sdy_b")}
            ]
        )

    return LaunchDescription(
        [
            rgb_r,
            rgb_g,
            rgb_b,
            turtle1
        ]
    )
# ros2 launch cpp01_launch py04_args_launch.py sdy_r:=0 sdy_g:=10 sdy_b:=100