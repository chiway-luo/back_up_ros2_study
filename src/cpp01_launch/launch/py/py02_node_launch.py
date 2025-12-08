#install(DIRECTORY launch DESTINATION share/PROJECT_NAME)
#<exec_depend>ros2launch</exec_depend>
#from glob import glob 用于setup.py配置多个launch文件
#('share/' + package_name, glob('launch/py/*_launch.py')),

from launch import LaunchDescription
from launch_ros.actions import Node
import os
from ament_index_python.packages import get_package_share_directory
# 封装终端指令相关类--------------
from launch.actions import ExecuteProcess
# from launch.substitutions import FindExecutable
# 参数声明与获取-----------------
# from launch.actions import DeclareLaunchArgument
# from launch.substitutions import LaunchConfiguration
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
    需求: 演示node的使用
    构造函数参数说明
    :param: package 被执行的程序所属的功能包
    :param: executable 可执行程序
    :param: name 节点名称
    :param: namespace 设置命名空间
    :param: exec_name 设置程序标签 默认 节点名称-编号
    :param: parameters 设置参数
    :param: remappings 实现话题重映射 remappings=[("/turtle1/cmd_vel","/sdy_control")]  a to b
    :param: ros_arguments 为节点传参 --ros-args xx yy zz
    :param: arguments 为节点传参 xx yy zz --ros-args
"""

def generate_launch_description():
    turtle1 = Node(
        package='turtlesim',
        executable='turtlesim_node',
        name='sdy_turtle1',
        exec_name='sdy_name',
        ros_arguments=["--remap","__ns:=/sdy_namespace"],
        )
    """
        ros2 param list 
            /sdy_namespace/sdy_turtle1:
            background_b
            background_g
            background_r
            qos_overrides./parameter_events.publisher.depth
            qos_overrides./parameter_events.publisher.durability
            qos_overrides./parameter_events.publisher.history
            qos_overrides./parameter_events.publisher.reliability
            use_sim_time
    """
    turtle2 = Node(
        package='turtlesim',
        executable='turtlesim_node',
        name='turtlesim',
        #方式1 直接设置参数
        # parameters=[{"background_r": 0}, {"background_g": 0}, {"background_b": 0}],
        #方式2 使用yaml文件设置参数 更常用
        # ros2 param dump /turtlesim --output-dir src/cpp01_launch/config/
        # parameters=[f"{get_package_share_directory('cpp01_launch')}/config/turtlesim.yaml"],
        parameters=[os.path.join(get_package_share_directory('cpp01_launch'),'config','turtlesim.yaml')],
        respawn=True#节点挂掉后自动重启
    )
    # test = ExecuteProcess(
    #     cmd=['pwd'],
    #     output='screen'
    # )
    return LaunchDescription(
        [
            # turtle1,
            turtle2
            # test
        ]
    )
