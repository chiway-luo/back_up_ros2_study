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
    在gazebo中加载自定义的仿真环境
    并生成小车模型
"""
def generate_launch_description():
    ld = LaunchDescription()
    #获取ros_gz_sim功能包路径
    ros_gz_sim_path = get_package_share_directory('ros_gz_sim')
    #获取demo_gazebo_sim功能包路径 
    demo_gazebo_sim_path = get_package_share_directory('demo_gazebo_sim')

    # 启动仿真环境   ros2 launch ros_gz_sim gz_sim.launch.py gz_args:="-v 4 -r visualize_lidar.sdf"
    """
    编辑.bashrc文件,添加环境变量
        #ign模型路径
        export IGN_GAZEBO_RESOURCE_PATH=ign_models  #相对路径
        #export IGN_GAZEBO_RESOURCE_PATH=~/ign_models #绝对路径
    """
    gazebo_visualize_node = IncludeLaunchDescription(
        launch_description_source=PythonLaunchDescriptionSource(
            os.path.join(
                ros_gz_sim_path,
                'launch',
                'gz_sim.launch.py'
            )
        ),
        launch_arguments={# -v 是指日志等级 4 是最高等级的日志 -r 是指加载的sdf模型文件路径 
            # 'gz_args': f"-v 4 -r {os.path.join(demo_gazebo_sim_path,'world','house.sdf')}" #原始墙壁模型
            'gz_args': f"-r {os.path.join(demo_gazebo_sim_path,'world','house_add.sdf')}" #添加家具的房子模型
            # 'gz_args': f"-v 4 -r {os.path.join(get_package_share_directory('demo_gazebo_sim'),'world','visualize_lidar.sdf')}"
        }.items()
    )
    ld.add_action(gazebo_visualize_node)

    #加载小车模型的launch文件
    mycar_desc_sim_launch = IncludeLaunchDescription(
        launch_description_source=PythonLaunchDescriptionSource(
            os.path.join(
                get_package_share_directory('mycar_description_detailed'),
                'launch',
                'mycar_desc_sim.launch.py'
            )
        )
    )
    ld.add_action(mycar_desc_sim_launch)

    #调用ros_gz_sim
    ros_gz_sim_node = Node(
        package='ros_gz_sim',
        executable='create',
        arguments=[
            '-name', 'mycar_4w',
            '-topic', '/robot_description',
            '-x', '-4',
            # '-y', '0.0',
            '-z', '0.1', #防止生成模型时与地面嵌合
        ],
        output='screen'
    )
    ld.add_action(ros_gz_sim_node)

    #建立仿真环境与ros2的桥接 
    #转换: 速度 里程计 tf 关节状态 时钟
    # ign_gazebo 使用的话题
    ros_bridge_node = Node(
        package='ros_gz_bridge',
        executable='parameter_bridge',
        arguments=[
            '/cmd_vel@geometry_msgs/msg/Twist]gz.msgs.Twist',#速度
            '/model/mycar_4w/odometry@nav_msgs/msg/Odometry[gz.msgs.Odometry',#odom 里程计消息
            '/model/mycar_4w/tf@tf2_msgs/msg/TFMessage[gz.msgs.Pose_V',#tf消息
            '/world/empty/model/mycar_4w/joint_state@sensor_msgs/msg/JointState[gz.msgs.Model',#关节状态
            '/clock@rosgraph_msgs/msg/Clock[gz.msgs.Clock', #时钟

            '/scan@sensor_msgs/msg/LaserScan[gz.msgs.LaserScan', #单线激光雷达 
            '/scan/points@sensor_msgs/msg/PointCloud2[gz.msgs.PointCloudPacked', #多线激光雷达 
            '/depth_camera@sensor_msgs/msg/Image[gz.msgs.Image', #深度相机图像
            '/depth_camera/points@sensor_msgs/msg/PointCloud2[ignition.msgs.PointCloudPacked', #深度相机点云数据
            '/image_raw@sensor_msgs/msg/Image[gz.msgs.Image', #图像参数
            "/camera_info@sensor_msgs/msg/CameraInfo[gz.msgs.CameraInfo",#相机参数
        ],
        parameters=[{"qos_overrides./model/mycar.subscriber.reliability": "reliable"}],
        remappings=[
            ('/model/mycar_4w/odometry', '/odom'),
            ('/model/mycar_4w/tf', '/tf'),
            ('/world/empty/model/mycar_4w/joint_state', '/joint_states'),
        ]
    )
    ld.add_action(ros_bridge_node)

    #启动rviz2
    rviz2_node = Node(
        package='rviz2',
        executable='rviz2',
        arguments=['-d', os.path.join(demo_gazebo_sim_path,'rviz','ign_sim_car.rviz')],
        output='screen'
    )
    ld.add_action(rviz2_node)

    #因为 depth_camera/points 坐标系没发生改变 mycar_4w/base_footprint/depth_camera 发布static 坐标系变换与 camera
    static_laser_tf = Node(
        package='tf2_ros',
        executable='static_transform_publisher',
        name='static_laser_tf',
        arguments=[
            '--frame-id', 'camera',
            '--child-frame-id', 'mycar_4w/base_footprint/depth_camera',
            '--x', '0.0',
            '--y', '0.0',
            '--z', '0.0',
            '--roll', '0.0',
            '--pitch', '0.0',
            '--yaw', '0.0'
        ]
    )
    ld.add_action(static_laser_tf)

    
    return ld

