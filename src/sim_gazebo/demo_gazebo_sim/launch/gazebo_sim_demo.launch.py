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
    控制节点:
    # ros2 run teleop_twist_keyboard teleop_twist_keyboard --ros-args --remap /cmd_vel:=/model/vehicle_blue/cmd_vel

    ros2 run teleop_twist_keyboard teleop_twist_keyboard 

    ps aux | grep ign 查看运行中的gazebo服务
    chiway    122486 58.4  1.2 5022264 413284 pts/6  Rl   21:58   0:02 ign gazebo server
    kill -9 122486     强制关闭gazebo服务  

"""
def generate_launch_description():
    ld = LaunchDescription()
    #获取ros_gz_sim功能包路径
    ros_gz_sim_path = get_package_share_directory('ros_gz_sim')
    #获取demo_gazebo_sim功能包路径 
    demo_gazebo_sim_path = get_package_share_directory('demo_gazebo_sim')

    # 启动仿真环境   ros2 launch ros_gz_sim gz_sim.launch.py gz_args:="-v 4 -r visualize_lidar.sdf"
    gazebo_visualize_node = IncludeLaunchDescription(
        launch_description_source=PythonLaunchDescriptionSource(
            os.path.join(
                ros_gz_sim_path,
                'launch',
                'gz_sim.launch.py'
            )
        ),
        launch_arguments={# -v 是指日志等级 4 是最高等级的日志 -r 是指加载的sdf模型文件路径 
            'gz_args': f"-v 4 -r {os.path.join(demo_gazebo_sim_path,'world','visualize_lidar.sdf')}"
            # 'gz_args': f"-v 4 -r {os.path.join(get_package_share_directory('demo_gazebo_sim'),'world','visualize_lidar.sdf')}"
        }.items()
    )
    ld.add_action(gazebo_visualize_node)

    #桥接节点(all)
    bridge = Node(
        package='ros_gz_bridge',
        executable='parameter_bridge',
        arguments=[
            '/model/vehicle_blue/tf@tf2_msgs/msg/TFMessage[gz.msgs.Pose_V',#启动tf桥接节点 /model/vehicle_blue/tf
            '/model/vehicle_blue/odometry@nav_msgs/msg/Odometry[gz.msgs.Odometry',#启动odometry桥接节点
            '/model/vehicle_blue/cmd_vel@geometry_msgs/msg/Twist]gz.msgs.Twist',#启动cmd_vel控制桥接节点 ros2 run ros_gz_bridge parameter_bridge /model/vehicle_blue/cmd_vel@geometry_msgs/msg/Twist]gz.msgs.Twist
            '/lidar2@sensor_msgs/msg/LaserScan[gz.msgs.LaserScan' #启动雷达桥接节点 /lidar /lidar2 雷达2才是车上的雷达
        ],
        remappings=[
            ('/model/vehicle_blue/tf', '/tf'),
            ('/model/vehicle_blue/cmd_vel', '/cmd_vel'),
        ]
    )
    ld.add_action(bridge)


    #启动tf桥接节点 /model/vehicle_blue/tf
    bridge_tf_world_node = Node(
        package='ros_gz_bridge',
        executable='parameter_bridge',
        arguments=[
            '/world/visualize_lidar_world/pose/info@tf2_msgs/msg/TFMessage[gz.msgs.Pose_V'
        ],
        remappings=[('/world/visualize_lidar_world/pose/info', '/tf_static')]
    )
    # ld.add_action(bridge_tf_world_node)

    

    #发布静态坐标系变换
    static_laser_tf = Node(
        package='tf2_ros',
        executable='static_transform_publisher',
        name='static_laser_tf',
        arguments=[
            # '--frame-id', 'vehicle_blue/odom',vehicle_blue/chassis
            '--frame-id', 'vehicle_blue/chassis',
            # '--child-frame-id', 'model_with_lidar/link/gpu_lidar',
            '--child-frame-id', 'vehicle_blue/lidar_link/gpu_lidar',
            '--x', '0.0',
            '--y', '0.0',
            '--z', '0.5',
            '--roll', '0.0',
            '--pitch', '0.0',
            '--yaw', '0.0'
        ]
    )
    ld.add_action(static_laser_tf)

    #启动rviz2节点
    rviz2_node = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        arguments=['-d', os.path.join(
                demo_gazebo_sim_path,
                'rviz',
                'visualize.rviz'
            )],
        output='screen'
    )
    ld.add_action(rviz2_node)

    return ld








#启动odometry桥接节点
# bridge_odom_node = Node(
#     package='ros_gz_bridge',
#     executable='parameter_bridge',
#     arguments=[
#         '/model/vehicle_blue/odometry@nav_msgs/msg/Odometry[gz.msgs.Odometry'
#     ]
# )
# ld.add_action(bridge_odom_node)

#启动控制桥接节点 ros2 run ros_gz_bridge parameter_bridge /model/vehicle_blue/cmd_vel@geometry_msgs/msg/Twist]gz.msgs.Twist
# bridge_cmd = Node(
#     package='ros_gz_bridge',
#     executable='parameter_bridge',
#     arguments=[
#         '/model/vehicle_blue/cmd_vel@geometry_msgs/msg/Twist]gz.msgs.Twist'
#     ],
#     output='screen'
# )
# ld.add_action(bridge_cmd)

#启动雷达桥接节点
# bridge_laser = Node(
#     package='ros_gz_bridge',
#     executable='parameter_bridge',
#     arguments=[ # 雷达 是对称分布的 雷达2才是车上的雷达
#         '/lidar2@sensor_msgs/msg/LaserScan[gz.msgs.LaserScan'
#     ],
#     output='screen'
# )
# ld.add_action(bridge_laser)

#启动tf桥接节点 /model/vehicle_blue/tf
# bridge_tf_model_node = Node(
#     package='ros_gz_bridge',
#     executable='parameter_bridge',
#     arguments=[
#         '/model/vehicle_blue/tf@tf2_msgs/msg/TFMessage[gz.msgs.Pose_V'
#     ],
#     remappings=[('/model/vehicle_blue/tf', '/tf')]
# )
# ld.add_action(bridge_tf_model_node)
