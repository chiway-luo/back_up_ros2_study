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
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
# from launch.conditions import IfCondition #判断是否执行
# from launch.conditions import UnlessCondition #取反
from launch.substitutions import PythonExpression
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
    多车编队(护航实现)  1为主车 0 2 为护航车辆
    需求: 
        0.前提:准备一个多车环境(仿真或者实体机器人)
        1.在车辆1的正后方制定一个固定坐标系goal_1 和 goal_2
        2.订阅坐标变换解析出机器人x和goal_x的位姿关系,并生成控制机器人x运动的速度指令  

    ros2 launch my_exer10_tf_sub car_follow.launch.py   
"""
def generate_launch_description():
    ld = LaunchDescription()
    #0仿真环境(可注释) 启动stage_ros2 并发布robot_0/odom 与 robot_1/odom 到map的静态坐标系变换
    stage_ros2 = IncludeLaunchDescription(
        launch_description_source=PythonLaunchDescriptionSource(
            os.path.join(
                get_package_share_directory("my_exer09_tf_pub"),
                "launch",
                "stage_ros2_multi_escort.launch.py",
            )
        )
    )
    ld.add_action(stage_ros2)

    # 1.在车辆1的正后方制定一个固定坐标系goal,改坐标系是车辆2实现跟随参考
    #声明参数
    car_followd_frame = DeclareLaunchArgument(name="car_followd_frame", default_value="robot_1/base_link")#被跟随坐标系

    car_follow_frame_0 = DeclareLaunchArgument(name="car_follow_frame_0", default_value="robot_0/base_link")#跟随坐标系
    car_follow_frame_2 = DeclareLaunchArgument(name="car_follow_frame_2", default_value="robot_2/base_link")#跟随坐标系
    car_goal_frame_0 = DeclareLaunchArgument(name="car_goal_frame_0", default_value="goal_0")
    car_goal_frame_2 = DeclareLaunchArgument(name="car_goal_frame_2", default_value="goal_2")
    goal_x = DeclareLaunchArgument(name="goal_x", default_value="-0.8") #车辆1后方0.8米处
    goal_y = DeclareLaunchArgument(name="goal_y", default_value="0.5")
    ld.add_action(car_followd_frame)
    ld.add_action(car_follow_frame_0)
    ld.add_action(car_follow_frame_2)
    ld.add_action(car_goal_frame_0)
    ld.add_action(car_goal_frame_2)
    ld.add_action(goal_x)
    ld.add_action(goal_y)

    #发布静态坐标变换
    goal0_to_base1 = Node(
        package="tf2_ros",
        executable="static_transform_publisher",
        name="goal_to_odom1",
        arguments=[
            "--frame-id",LaunchConfiguration("car_followd_frame"),
            "--child-frame-id",LaunchConfiguration("car_goal_frame_0"),
            "--x",LaunchConfiguration("goal_x"),
            "--y",PythonExpression(['str(-1 * float(',LaunchConfiguration("middle_distance"),'))'])
        ]
    )    
    ld.add_action(goal0_to_base1)

    goal2_to_base1 = Node(
        package="tf2_ros",
        executable="static_transform_publisher",
        name="goal_to_odom1",
        arguments=[
            "--frame-id",LaunchConfiguration("car_followd_frame"),
            "--child-frame-id",LaunchConfiguration("car_goal_frame_2"),
            "--x",LaunchConfiguration("goal_x"),
            "--y",PythonExpression(['str(1 * float(',LaunchConfiguration("middle_distance"),'))'])
        ]
    )    
    ld.add_action(goal2_to_base1)

    # 2.订阅坐标变换解析出机器人2和goal的位姿关系,并生成控制机器人2运动的速度指令
    # 订阅坐标变换并转换到坐标系base2下,发布速度控制指令  
    tf_listener_0 = Node(#右后方机器人
        package="my_exer10_tf_sub",
        executable="tf_listener",
        name="car_follow_0",
        output="screen",
        # this->declare_parameter<std::string>("goal_frame","goal");//目标坐标系
        # this->declare_parameter<std::string>("follow_frame","robot_0/base_link");//跟随坐标系
        # this->declare_parameter<double>("obstacle_threshold",0.5);//最小避障距离阈值
        parameters=[
            {"goal_frame": LaunchConfiguration("car_goal_frame_0")},
            {"follow_frame": LaunchConfiguration("car_follow_frame_0")},
            {"obstacle_threshold": 0.5},#避障距离阈值
        ],
        # remappings=(("/robot_0/cmd_vel","/car2/cmd_vel"),) #重映射话题到car2 属于实体机器人部分
        namespace="robot_0"
    )
    ld.add_action(tf_listener_0)

    tf_listener_2 = Node(#左后方机器人
        package="my_exer10_tf_sub",
        executable="tf_listener",
        name="car_follow_2",
        output="screen",
        parameters=[
            {"goal_frame": LaunchConfiguration("car_goal_frame_2")},
            {"follow_frame": LaunchConfiguration("car_follow_frame_2")},
            {"obstacle_threshold": 0.5},
        ],
        namespace="robot_2"
    )
    ld.add_action(tf_listener_2)

    return ld

