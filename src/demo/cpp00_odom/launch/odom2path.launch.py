#install(DIRECTORY config params launch DESTINATION share/${PROJECT_NAME}) #cmake配置
#<exec_depend>ros2launch</exec_depend> <!--package.xml配置-->
#from glob import glob #用于setup.py配置多个launch文件
#('share/' + package_name, glob('launch/*_launch.py')),
#('share/' + package_name, glob('launch/*_launch.xml')),
#('share/' + package_name, glob('launch/*_launch.yaml')),

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
# from launch.substitutions import PythonExpression #运行时计算表达式
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

    my_rviz_config = os.path.join(
        get_package_share_directory('cpp00_odom'),
        'config',
        'odom2path.rviz'
    )
    

    #stage仿真环境
    stage_launch = IncludeLaunchDescription(
        launch_description_source=PythonLaunchDescriptionSource(
            os.path.join(
                get_package_share_directory('stage_ros2'),
                'launch',
                'my_house.launch.py'
            )
        ),
        launch_arguments={
            'rviz_config': my_rviz_config
        }.items()
    )
    ld.add_action(stage_launch)

    #里程计转换路径节点
    odom2path_node = Node(
        package='cpp00_odom',
        executable='odom2path',
        name='odom2path',
        output='screen',
        # parameters=[
        #     {'path_frame_id': 'odom'},
        #     {'path_topic_name': '/odom_path'},
        #     {'odom_topic_name': '/odom'},
        #     {'publish_rate': 1.0},
        # ]
    )
    ld.add_action(odom2path_node)

    return ld


