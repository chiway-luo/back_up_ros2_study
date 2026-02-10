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
    导航服务器的核心节点实现
        1.基础部分实现
            1-1生命周期管理器节点
                在nav2中所有的节点都是具有生命周期的节点
                通过生命周期管理器节点来管理所有节点的状态转换
            1-2行为树服务器
                控制导航的执行流程
        2.规划器实现
            2-1planner_server规划器服务节点
            2-2全局代价地图服务节点
            规划器节点依赖于全局代价地图
"""
def generate_launch_description():
    ld = LaunchDescription()
    # 声明参数
    ld.add_action(DeclareLaunchArgument('use_sim_time', default_value='True'))

    # 获取行为树yaml文件路径并声明参数
    this_pkg_path = get_package_share_directory('sim_navigation2')

    bt_yaml = os.path.join(this_pkg_path, 'params', 'bt_navigator.yaml')
    ld.add_action(DeclareLaunchArgument('bt_filename', default_value=bt_yaml))
    # 获取行为树xml文件路径并声明参数
    #单点
    nav2_pose = os.path.join(this_pkg_path, 'bts', 'nav2_pose.xml')
    ld.add_action(DeclareLaunchArgument('nav2_pose', default_value=nav2_pose))
    #多点
    nav2_poses = os.path.join(this_pkg_path, 'bts', 'nav2_poses.xml')
    ld.add_action(DeclareLaunchArgument('nav2_poses', default_value=nav2_poses))

    # 规划器yaml文件
    planner_yaml = os.path.join(this_pkg_path, 'params', 'planner.yaml')
    ld.add_action(DeclareLaunchArgument('planner_yaml', default_value=planner_yaml))


    # 规划器节点 依赖于全局代价地图
    planner_server_node = Node(
        package='nav2_planner',
        executable='planner_server',
        name='planner_server',
        parameters=[
            {'use_sim_time': LaunchConfiguration('use_sim_time')},
            LaunchConfiguration('planner_yaml')
        ]
    )
    ld.add_action(planner_server_node)

    

    # nav2_bt_navigator bt_navigator 节点 
    bt_navigator_node = Node(
        package='nav2_bt_navigator',
        executable='bt_navigator',
        name='bt_navigator_node',
        parameters=[
            {'use_sim_time': LaunchConfiguration('use_sim_time')},
            #自定义行为树
            {'default_nav_to_pose_bt_xml': LaunchConfiguration('nav2_pose')},
            {'default_nav_through_poses_bt_xml': LaunchConfiguration('nav2_poses')},
            #加载yaml文件
            LaunchConfiguration('bt_filename')
        ]   
    )
    ld.add_action(bt_navigator_node)

    # 生命周期管理器节点
    lifecycle_manager_node = Node(
        package='nav2_lifecycle_manager',
        executable='lifecycle_manager',
        name='lifecycle_manager_navigation',
        output='screen',
        parameters=[{
            'use_sim_time': LaunchConfiguration('use_sim_time'),
            'autostart': True,
            'node_names': [# 被托管的节点列表
                # 'map_server', 
                # 'amcl', 
                'planner_server', 
                # 'controller_server', 
                # 'recoveries_server', 
                'bt_navigator_node' 
            ]
        }]
    )
    ld.add_action(lifecycle_manager_node)

    return ld
