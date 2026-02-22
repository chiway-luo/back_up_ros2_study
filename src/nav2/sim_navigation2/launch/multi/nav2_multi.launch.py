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
from launch.actions import SetLaunchConfiguration
from launch.actions import OpaqueFunction
# 文件包含相关-------------------
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
# 分组相关----------------------
from launch_ros.actions import PushRosNamespace
from launch.actions import GroupAction
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
        3.运动控制节点
            3-1运动控制节点
            3-2局部地图(随意调节)
            运动控制节点依赖于局部代价地图

        4.恢复行为实现
            当机器人陷入困境时,自行脱困

        5.路点跟踪
            设置一系列的目标点集合，机器人可以一次到达这些目标点

        6.平滑路径
            平滑路径，使机器人运行更流畅，安全且可以减少硬件磨损

        7.速度平滑
            平滑速度，使机器人运行更流畅，安全可以减少硬件磨损
"""
def make_path(context):
    # 获取行为树yaml文件路径并声明参数
    this_pkg_path = get_package_share_directory('sim_navigation2')


    # 获取车辆参数
    car_num = LaunchConfiguration('car_num').perform(context)

    # 设置行为树yaml文件路径参数
    bt_yaml = os.path.join(this_pkg_path, 'params', car_num, 'bt_navigator.yaml')

    # 获取行为树xml文件路径并声明参数
    #单点
    nav2_pose = os.path.join(this_pkg_path, 'bts', 'nav2_pose.xml')
    #多点
    nav2_poses = os.path.join(this_pkg_path, 'bts', 'nav2_poses.xml')

    # 规划器yaml文件
    planner_yaml = os.path.join(this_pkg_path, 'params', car_num, 'planner.yaml')

    # 运动控制节点
    controller_yaml = os.path.join(this_pkg_path, 'params', car_num, 'controller.yaml')

    # 恢复行为节点文件
    behavior_yaml = os.path.join(this_pkg_path, 'params', car_num, 'behavior.yaml')

    # 路点跟踪yaml文件
    waypoint_yaml = os.path.join(this_pkg_path, 'params', car_num, 'waypoint.yaml')

    # 路径平滑yaml文件
    smoother_yaml = os.path.join(this_pkg_path, 'params', car_num, 'smoother.yaml')
    
    # 速度平滑yaml文件
    velocity_smoother_yaml = os.path.join(this_pkg_path, 'params', car_num, 'velocity_smoother.yaml')

    return [
        SetLaunchConfiguration('bt_filename', bt_yaml),
        SetLaunchConfiguration('nav2_pose', nav2_pose),
        SetLaunchConfiguration('nav2_poses', nav2_poses),
        SetLaunchConfiguration('planner_yaml', planner_yaml),
        SetLaunchConfiguration('controller_yaml', controller_yaml),
        SetLaunchConfiguration('behavior_yaml', behavior_yaml),
        SetLaunchConfiguration('waypoint_yaml', waypoint_yaml),
        SetLaunchConfiguration('velocity_smoother_yaml', velocity_smoother_yaml),
        SetLaunchConfiguration('smoother_yaml', smoother_yaml)
    ]


def generate_launch_description():
    group_ld = LaunchDescription()# 外层列表,因为调用的参数不需要使用分组功能
    ld = LaunchDescription() # 里层列表

    group_ld.add_action(DeclareLaunchArgument('car_num', default_value='robot_0'))

    # 调用函数
    group_ld.add_action(DeclareLaunchArgument('car_num', default_value='robot_0'))
    group_ld.add_action(OpaqueFunction(function=make_path))
    
    # 声明参数
    group_ld.add_action(DeclareLaunchArgument('use_sim_time', default_value='True'))


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

    # 恢复行为节点
    behavior_server_node = Node(
        package='nav2_behaviors',
        executable='behavior_server',
        name='behavior_server',
        parameters=[
            {'use_sim_time': LaunchConfiguration('use_sim_time')},
             #加载yaml文件
            LaunchConfiguration('behavior_yaml')
        ]
    )
    ld.add_action(behavior_server_node)
    
    # 路点跟踪节点
    waypoint_node = Node(
        package='nav2_waypoint_follower',
        executable='waypoint_follower',
        name='waypoint_follower',
        parameters=[
            {'use_sim_time': LaunchConfiguration('use_sim_time')},
            #加载yaml文件
            LaunchConfiguration('waypoint_yaml')
        ]
    )
    ld.add_action(waypoint_node)

    # 路径平滑节点
    smoother_node = Node(
        package='nav2_smoother',
        executable='smoother_server',
        name='smoother_server',
        parameters=[
            {'use_sim_time': LaunchConfiguration('use_sim_time')},
            #加载yaml文件
            LaunchConfiguration('smoother_yaml')
        ]
    )
    ld.add_action(smoother_node)

    # 速度平滑
    velocity_smoother_node = Node(
        package='nav2_velocity_smoother',
        executable='velocity_smoother',
        name='velocity_smoother',
        parameters=[
            {"use_sim_time": LaunchConfiguration('use_sim_time')},
            # 加载yaml文件
            LaunchConfiguration('velocity_smoother_yaml')
        ],
        # 订阅cmd_vel话题
        # 发布smooth_cmd_vel话题
        remappings=[
            ('cmd_vel', 'raw_cmd_vel'),
            ('cmd_vel_smoothed', 'cmd_vel')
        ]
    )
    ld.add_action(velocity_smoother_node)




    #############################################

    # nav2_bt_navigator bt_navigator 节点 行为树服务器
    bt_navigator_node = Node(
        package='nav2_bt_navigator',
        executable='bt_navigator',
        name='bt_navigator',
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

    # 运动控制节点 controller_server
    controller_server_node = Node(
        package='nav2_controller',
        executable='controller_server',
        name='controller_server',
        parameters=[
            {'use_sim_time': LaunchConfiguration('use_sim_time')},
            #加载yaml文件
            LaunchConfiguration('controller_yaml')
        ],
        #controller发布的速度话题要被速度平滑器处理,需要将controller_server节点的输出话题
        # 重定向到velocity_smoother节点的输入,可以通过remappings参数实现
        remappings=[
            ('cmd_vel', 'raw_cmd_vel')
        ]
    )
    ld.add_action(controller_server_node)
    

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
                'controller_server', 
                # 'recoveries_server', 
                'bt_navigator',
                'behavior_server',
                'waypoint_follower',
                'smoother_server',
                'velocity_smoother'
            ]
        }]
    )
    ld.add_action(lifecycle_manager_node)

    # 设置分组
    group = GroupAction(
        actions=[
            PushRosNamespace(LaunchConfiguration('car_num')),
            ld
        ]
    )
    group_ld.add_action(group)

    return group_ld
