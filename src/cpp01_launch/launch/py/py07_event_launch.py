#install(DIRECTORY launch DESTINATION share/PROJECT_NAME)
#<exec_depend>ros2launch</exec_depend>
#from glob import glob 用于setup.py配置多个launch文件
#('share/' + package_name, glob('launch/py/*_launch.py')),

from launch import LaunchDescription
from launch_ros.actions import Node
# 封装终端指令相关类--------------
from launch.actions import ExecuteProcess
from launch.substitutions import FindExecutable 
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
    需求:为turtlesim_node 绑定事件,节点启动时执行生成新的小乌龟的程序,节点关闭时执行日志 打印节点退出消息
"""
def generate_launch_description():
    turtle = Node(
        package='turtlesim',
        executable='turtlesim_node',
        namespace='sdy_namespace'
    )
    # ros2 service call /launch_test/spawn turtlesim/srv/Spawn "{'x': 8,'y': 8,'theta': 3.14}"
    spawn_turtle = ExecuteProcess(
        cmd=[FindExecutable(name='ros2'),'service','call','/sdy_namespace/spawn','turtlesim/srv/Spawn',"\"{'x': 8,'y': 8,'theta': 3.14}\""],
        output='both',
        shell=True
    )
    #注册节点启动事件
    start_event_handler = RegisterEventHandler(
        event_handler=OnProcessStart(
            target_action=turtle,
            on_start=[spawn_turtle]
        )
    )
    #注册节点关闭事件
    exit_event_handler = RegisterEventHandler(
        event_handler=OnProcessExit(
            target_action=turtle,
            on_exit=[
                LogInfo(msg="sdy_namespace/turtlesim_node节点已退出")
            ]
        )
    )


    return LaunchDescription(
        [
            turtle,
            # spawn_turtle,
            start_event_handler,
            exit_event_handler,
        ]
    )