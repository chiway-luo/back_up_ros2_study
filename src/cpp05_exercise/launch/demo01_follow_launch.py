#install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})
#<exec_depend>ros2launch</exec_depend>
#from glob import glob 用于setup.py配置多个launch文件
#('share/' + package_name, glob('launch/py/*_launch.py')),

from launch import LaunchDescription
from launch_ros.actions import Node
# 封装终端指令相关类--------------
from launch.actions import ExecuteProcess
# from launch.substitutions import FindExecutable   #FindExecutable(name="ros2")
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
from launch.event_handlers import OnProcessStart, OnProcessExit
from launch.actions import ExecuteProcess, RegisterEventHandler,LogInfo
# 获取功能包下share目录路径-------
# from ament_index_python.packages import get_package_share_directory

"""
    需求:启动turtlesim节点 并生成turtle1和turtle2 两个乌龟 启动demo01_turtle_follow节点 实现turtle2跟随turtle1

    turtle_node->cmd_spawn->demo01_turtle_follow_node
"""

def generate_launch_description():
    turtle_node = Node(
        package="turtlesim",
        executable="turtlesim_node",
    )
    cmd_spawn = ExecuteProcess(
        cmd=["ros2", "service", "call", "/spawn", "turtlesim/srv/Spawn", '{"x":8.0, "y":9.0, "theta":3.14, "name":"turtle2"}'],
        output="screen",
    )
    demo01_turtle_follow_node = Node(
        package='cpp05_exercise',
        executable='demo01_turtle_follow',
    )
    demo02_turtle_tf_broadcaster_node = Node(
        package='cpp05_exercise',
        executable='demo02_turtle_tf_broadcaster',
    )
    rviz_node = ExecuteProcess(
        cmd=["rviz2"]  
    )

    step1 = RegisterEventHandler(
        event_handler=OnProcessStart(
            target_action=turtle_node,
            on_start=[cmd_spawn],
        )
    )
    step2 = RegisterEventHandler(
        event_handler=OnProcessStart(
            target_action=cmd_spawn,
            on_start=[demo01_turtle_follow_node, demo02_turtle_tf_broadcaster_node, rviz_node],
        )
    )

    return LaunchDescription(
        [
            turtle_node,
            step1,
            step2,
            
        ]
    )