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
# from launch.actions import DeclareLaunchArgument
# from launch.substitutions import LaunchConfiguration
# from launch.conditions import IfCondition #判断是否执行
# from launch.conditions import UnlessCondition #取反
# from launch.substitutions import PythonExpression #运行时计算表达式
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
from ament_index_python.packages import get_package_share_directory
# urdf文件处理相关--------------
# from launch_ros.parameter_descriptions import ParameterValue
# from launch.substitutions import Command



def generate_launch_description():
    ld = LaunchDescription()

    #启动stage_ros2仿真环境节点,并加载自定义的仿真环境
    stage_ros2_node = Node(
        package='stage_ros2',
        executable='stage_ros2',
        name='stage_ros2',
        parameters=[{
            'world_file': os.path.join(
                get_package_share_directory('demo_stage_sim'),
                'world',
                'sim.world'
            )
        }]
    )
    ld.add_action(stage_ros2_node)

    #启动雷达融合节点(双雷达模型需要开启)
    """ 
        <param name="destination_frame" value="base_link"/>
		<param name="cloud_destination_topic" value="/merged_cloud"/>
		<param name="scan_destination_topic" value="/scan_multi"/>
		<param name="laserscan_topics" value ="/scansx /scandx" />
		<!-- LIST OF THE LASER SCAN TOPICS TO SUBSCRIBE -->
		<param name="angle_min" value="-3.14"/>
		<param name="angle_max" value="3.14"/>
		<param name="angle_increment" value="0.00437"/>
		<param name="scan_time" value="0.0"/>
		<param name="range_min" value="0.1"/>
		<param name="range_max" value="2.0"/>
    """
    ira_laser_node = Node(
        package='ira_laser_tools',
        executable='laserscan_multi_merger',
        name='ira_laser_merger_node',
        parameters=[{
            'destination_frame': 'base_link',
            'cloud_destination_topic': '/merged_cloud',
            'scan_destination_topic': '/scan_multi',
            'laserscan_topics': "/base_scan1 /base_scan2",
            'angle_min': -3.14,
            'angle_max': 3.14,
            'angle_increment': 0.00437,
            'scan_time': 0.0,
            'range_min': 0.1,
            'range_max': 20.0  #雷达最大测距20米
        }],
        output='screen'
    )
    ld.add_action(ira_laser_node)

    #启动rviz2节点,并加载自定义的rviz配置文件
    rviz2_node = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        arguments=['-d', os.path.join(
            get_package_share_directory('demo_stage_sim'),
            'config',
            'sim_multi.rviz' #双雷达 sim_multi.rviz  单雷达是 sim.rviz
        )],
        output='screen'
    )
    ld.add_action(rviz2_node)

    return ld
