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
"""
    导航的启动文件
        集成 定位 与 导航核心launch文件
"""
def generate_launch_description():
    ld = LaunchDescription()

    ld.add_action(DeclareLaunchArgument('use_sim_time', default_value='True'))
    ld.add_action(DeclareLaunchArgument('map_filename', default_value='map/stage_map.yaml'))

    # 包含定位launch文件 amcl和map文件定位
    amcl_and_map = IncludeLaunchDescription(
        launch_description_source=PythonLaunchDescriptionSource(
            os.path.join(
                get_package_share_directory('sim_localization'),
                'launch',
                'sim_loca.launch.py'
            )
        ),
        launch_arguments={
            'use_sim_time': LaunchConfiguration('use_sim_time'),
            'yaml_filename': LaunchConfiguration('map_filename')
        }.items()
    )
    # ld.add_action(amcl_and_map)

    ##########################################

    # 不使用map文件和amcl进行定位,使用cartographer进行定位,包含cartographer的launch文件
    # src/slam/sim_slam_cartographer/launch/sim_cartographer.launch.py
    cartographer_node = IncludeLaunchDescription(
        launch_description_source=PythonLaunchDescriptionSource(
            os.path.join(
                get_package_share_directory('sim_slam_cartographer'),
                'launch',
                'sim_cartographer.launch.py'
            )
        ),
        launch_arguments={
            'param_name': 'stage.lua',
        }.items()
    )
    ld.add_action(cartographer_node)

    # slam_toolbox
    slam_toolbox_node = IncludeLaunchDescription(
        launch_description_source=PythonLaunchDescriptionSource(
            os.path.join(
                get_package_share_directory('sim_slam_slam_toolbox'),
                'launch',
                'online_sync.launch.py'
            )
        ),
        launch_arguments={
            'use_sim_time': LaunchConfiguration('use_sim_time'),
            # 'path_param': 'mapper_params_online_sync.yaml',
        }.items()
    )
    # ld.add_action(slam_toolbox_node)



    ##########################################

    # 包含导航launch
    nav2_pkg = get_package_share_directory('sim_navigation2')
    nav2_launch = IncludeLaunchDescription(
        launch_description_source=PythonLaunchDescriptionSource(
            os.path.join(nav2_pkg, 'launch', 'nav2.launch.py')
        ),
        launch_arguments={
            'use_sim_time': LaunchConfiguration('use_sim_time'),
        }.items()
    )
    ld.add_action(nav2_launch)


    return ld
