#install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})
#<exec_depend>ros2launch</exec_depend>
#from glob import glob 用于setup.py配置多个launch文件
#('share/' + package_name, glob('launch/py/*_launch.py')),

from launch import LaunchDescription
from launch_ros.actions import Node
# 封装终端指令相关类--------------
# from launch.actions import ExecuteProcess
# from launch.substitutions import FindExecutable   #FindExecutable(name="ros2")
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
from launch.event_handlers import OnProcessStart, OnProcessExit
from launch.actions import ExecuteProcess, RegisterEventHandler,LogInfo
# 获取功能包下share目录路径-------
# from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    #优化坐标系的设置
    #将param参数抽取为变量
    frame_id = DeclareLaunchArgument(
        name='frame_id',
        default_value='turtle3',
        description='Name of the second turtle'
    )
    child_frame_id = DeclareLaunchArgument(
        name='child_frame_id',
        default_value='turtle1',
        description='Name of the first turtle'
    )

    #乌龟gui节点
    turtle_node = Node(
        package='turtlesim',
        executable='turtlesim_node',
    )
    #乌龟生成客户端节点
    spawn_node = Node(
        package='cpp05_exercise',
        executable='exer01_spawn',
        parameters=[{'turtle_name':LaunchConfiguration('frame_id')}]
    )

    #分别广播两只乌龟相对于base_link的坐标变换
    broadcaster1 = Node(
        package='cpp05_exercise',
        executable='exer02_tf_broadcaster',
        name='tf_broadcaster1',
        parameters=[{'turtle':LaunchConfiguration('frame_id')}]
    )
    broadcaster2 = Node(
        package='cpp05_exercise',
        executable='exer02_tf_broadcaster',
        name='tf_broadcaster2',
        parameters=[{'turtle':LaunchConfiguration('child_frame_id')}]
    )
    #监听坐标系变换并发布速度指令节点
    vel_node = Node(
        package='cpp05_exercise',
        executable='exer03_tf_listener',
        name='tf_listener',
        parameters=[{'frame_id':LaunchConfiguration('frame_id')},{'child_frame_id':LaunchConfiguration('child_frame_id')}]
    )
    
    step1 = RegisterEventHandler(
        event_handler=OnProcessStart(
            target_action=turtle_node,
            on_start=[spawn_node]
        )
    )
    step2 = RegisterEventHandler(
        event_handler=OnProcessExit(
            target_action=spawn_node,
            on_exit=[vel_node]
        )
    )

    return LaunchDescription(
        [
            frame_id,
            child_frame_id,
            turtle_node,
            step1,
            step2,
            broadcaster1,
            broadcaster2,
            # vel_node,
        ]
    )