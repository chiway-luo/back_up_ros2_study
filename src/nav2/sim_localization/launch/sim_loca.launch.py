#install(DIRECTORY config params launch DESTINATION share/${PROJECT_NAME}) #cmake配置
#<exec_depend>ros2launch</exec_depend> <!--package.xml配置-->
#from glob import glob #用于setup.py配置多个launch文件
#('share/' + package_name + '/launch', glob('launch/*launch.py')),
#('share/' + package_name + '/launch', glob('launch/*launch.xml')),
#('share/' + package_name + '/launch', glob('launch/*launch.yaml')),
#(os.path.join('share', package_name, 'launch'), glob('launch/*.launch.py')),

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

    # 参数
    ld.add_action(
        DeclareLaunchArgument(
            'use_sim_time',
            default_value='True'
        )
    )
    ld.add_action(
        DeclareLaunchArgument(
            'yaml_filename',
            default_value='map/ign_map.yaml'
        )
    )

    # 加载地图数据
    map_server_node = IncludeLaunchDescription(
        launch_description_source=PythonLaunchDescriptionSource(
            os.path.join(
                get_package_share_directory('map_server'),
                'launch',
                'sim_map_server.launch.py'
            )
        ),
        launch_arguments={
            'use_sim_time': LaunchConfiguration('use_sim_time'),
            'yaml_filename': LaunchConfiguration('yaml_filename')
        }.items()
    )
    ld.add_action(map_server_node)

    # 启动amcl节点
    amcl_node = Node(
        package='nav2_amcl',
        executable='amcl',
        name='sim_amcl',
        parameters=[
            os.path.join(
                get_package_share_directory('sim_localization'),
                'params',
                'amcl.yaml'
            ),
            {'use_sim_time': LaunchConfiguration('use_sim_time')}
        ]
    )
    ld.add_action(amcl_node)

    # 启动生命周期管理器
    manager_lifecycle_node = Node(
        package='nav2_lifecycle_manager',
        executable='lifecycle_manager',
        name='sim_lifecycle_manager',
        parameters=[
            {'use_sim_time': LaunchConfiguration('use_sim_time')},
            {'autostart': True},
            {'node_names': ['sim_amcl']}
        ]
    )
    ld.add_action(manager_lifecycle_node)

    return ld
