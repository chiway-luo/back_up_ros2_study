#install(DIRECTORY launch DESTINATION share/PROJECT_NAME)
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
# from launch.event_handlers import OnProcessStart, OnProcessExit
# from launch.actions import ExecuteProcess, RegisterEventHandler,LogInfo
# 获取功能包下share目录路径-------
# from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    #laser相对于base_link的静态坐标系发布节点
    tf_pub1 = Node(
        package='tf2_ros',
        executable='static_transform_publisher',
        arguments=['0.4','0.0','0.2','0.0','0.0','0.0','base_link','laser']
    )
    tf_pub2 = Node(
        package='tf2_ros',
        executable='static_transform_publisher',
        arguments=['-0.5','0.0','0.4','0.0','0.0','0.0','base_link','camera']
    )
    #turtlesim节点
    turtlesim_node = ExecuteProcess(
        cmd=['ros2','run','turtlesim','turtlesim_node'],
        output='both'
    )
    #发布乌龟相对于窗体的位姿信息
    tf_turtle = Node(
        package='cpp03_tf_broadcaster',
        executable='demo02_dynamic_tf_broadcaster'
    )

    #还需要自行启动新的终端运行teleop键盘控制乌龟移动
    #ros2 run teleop_twist_keyboard teleop_twist_keyboard --ros-args --remap /cmd_vel:=/turtle1/cmd_vel

    return LaunchDescription(
        [
            tf_pub1,
            tf_pub2,
            turtlesim_node,
            tf_turtle
        ]
    )