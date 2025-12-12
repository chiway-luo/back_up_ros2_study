#install(DIRECTORY launch DESTINATION share/${PROJECT_NAME}) cmake配置
#<exec_depend>ros2launch</exec_depend> package.xml配置
#from glob import glob 用于setup.py配置多个launch文件
#('share/' + package_name, glob('launch/*_launch.py')),
#('share/' + package_name, glob('launch/*_launch.xml')),
#('share/' + package_name, glob('launch/*_launch.yaml')),

from launch import LaunchDescription
from launch_ros.actions import Node
# 封装终端指令相关类--------------
from launch.actions import ExecuteProcess
from launch.substitutions import FindExecutable   #FindExecutable(name="ros2")
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
# from launch.event_handlers import OnProcessStart, OnProcessExit
# from launch.actions import ExecuteProcess, RegisterEventHandler,LogInfo
# 获取功能包下share目录路径-------
# from ament_index_python.packages import get_package_share_directory
"""
    当前为教师示例代码 使用静态转换实现目标点的追踪
    需求: 实现乌龟护航案例
        先实现一只乌龟护航 ,要求最终位置处理turtle1的正后方2米
    思路:
        1.先发布目标点相对于turtle1的坐标系变换
        2.监听turtle2与目标点的相对位置关系
    流程:
        1.抽取参数
        2.创建turtlesim_node节点,并生成新乌龟
        3.发布坐标变换(turtle1->base_link,turtle2->base_link,目标点->turtle1)
        4.监听目标变换(turtle2->目标点),并发布速度指令

        ros2 run teleop_twist_keyboard teleop_twist_keyboard --ros-args --remap /cmd_vel:=/turtle1/cmd_vel

"""
def generate_launch_description():
    # 1.抽取参数
    escort_back = DeclareLaunchArgument(
        name="turtle_back",
        default_value="turtle_back",
    )
    escort_left = DeclareLaunchArgument(
        name="turtle_left",
        default_value="turtle_left",
    )
    escort_right = DeclareLaunchArgument(
        name="turtle_right",
        default_value="turtle_right",
    )
    turtle1_name = DeclareLaunchArgument(
        "turtle1_name",
        default_value="turtle1",
    )
    point_back = DeclareLaunchArgument(
        "point_back",
        default_value="target_point_back",
    )
    point_left = DeclareLaunchArgument(
        "point_left",
        default_value="target_point_left",
    )
    point_right = DeclareLaunchArgument(
        "point_right",
        default_value="target_point_right",
    )


    # 2.创建turtlesim_node节点,并生成新乌龟
    turlte_node = Node(
        package="turtlesim",
        executable="turtlesim_node",
        name="turtle1",
        output="screen",
    )
    spawn_turtle_back = Node(
        package="cpp05_exercise",
        executable="exer01_spawn",
        name="spawn_back",
        parameters=[
            {'x': 2.0},
            {'y': 2.0},
            {'turtle_name': LaunchConfiguration("turtle_back")}
        ]
    )
    spawn_turtle_left = Node(
        package="cpp05_exercise",
        executable="exer01_spawn",
        name="spawn_left",
        parameters=[
            {'x': 2.0},
            {'y': 2.0},
            {'turtle_name': LaunchConfiguration("turtle_left")}
        ]
    )
    spawn_turtle_right = Node(
        package="cpp05_exercise",
        executable="exer01_spawn",
        name="spawn_right",
        parameters=[
            {'x': 2.0},
            {'y': 2.0},
            {'turtle_name': LaunchConfiguration("turtle_right")}
        ]
    )

    # 3.发布坐标变换(turtle1->base_link,turtle2->base_link,目标点->turtle1(静态坐标变换))
    tf_broadcaster_1 = Node(
        package="cpp05_exercise",
        executable="exer02_tf_broadcaster",
        name="tf_broadcaster_turtle1",
        parameters=[
            {'turtle': LaunchConfiguration("turtle1_name")}
        ]
    )
    tf_broadcaster_back = Node(
        package="cpp05_exercise",
        executable="exer02_tf_broadcaster",
        name="tf_broadcaster_back",
        parameters=[
            {'turtle': LaunchConfiguration("turtle_back")}
        ]
    )
    tf_broadcaster_left = Node(
        package="cpp05_exercise",
        executable="exer02_tf_broadcaster",
        name="tf_broadcaster_left",
        parameters=[
            {'turtle': LaunchConfiguration("turtle_left")}
        ]
    )
    tf_broadcaster_right = Node(
        package="cpp05_exercise",
        executable="exer02_tf_broadcaster",
        name="tf_broadcaster_right",
        parameters=[
            {'turtle': LaunchConfiguration("turtle_right")}
        ]
    )
    #目标点相对于turtle1的静态变换
    tf_point_back_broadcaster = Node(
        package="tf2_ros",
        executable="static_transform_publisher",
        name="tf_point_back_broadcaster",
        arguments=[
            "--frame-id", LaunchConfiguration("turtle1_name"),
            "--child-frame-id", LaunchConfiguration("point_back"),
            "--x","-1",
            # "0","0","0","0","0",
        ]
    )
    tf_point_left_broadcaster = Node(
        package="tf2_ros",
        executable="static_transform_publisher",
        name="tf_point_left_broadcaster",
        arguments=[
            "--frame-id", LaunchConfiguration("turtle1_name"),
            "--child-frame-id", LaunchConfiguration("point_left"),
            "--y","1",
            # "0","0","0","0","0",
        ]
    )
    tf_point_right_broadcaster = Node(
        package="tf2_ros",
        executable="static_transform_publisher",
        name="tf_point_right_broadcaster",
        arguments=[
            "--frame-id", LaunchConfiguration("turtle1_name"),
            "--child-frame-id", LaunchConfiguration("point_right"),
            "--y","-1",
            # "0","0","0","0","0",
        ]
    )
    # 4.监听目标变换(turtle2->目标点),并发布速度指令
    back_escort_goal_back = Node(
        package="cpp05_exercise",
        executable="exer03_tf_listener",
        name="escort_goal_back1",
        parameters=[
            {'frame_id': LaunchConfiguration("turtle_back")},
            {'child_frame_id': LaunchConfiguration("point_back")},
        ],
        #节点重启
        respawn=True
    )
    left_escort_goal_left = Node(
        package="cpp05_exercise",
        executable="exer03_tf_listener",
        name="escort_goal_left1",
        parameters=[
            {'frame_id': LaunchConfiguration("turtle_left")},
            {'child_frame_id': LaunchConfiguration("point_left")},
        ],
        respawn=True

    )
    right_escort_goal_right = Node(
        package="cpp05_exercise",
        executable="exer03_tf_listener",
        name="escort_goal_right1",
        parameters=[
            {'frame_id': LaunchConfiguration("turtle_right")},
            {'child_frame_id': LaunchConfiguration("point_right")},
        ],
        respawn=True
    )


    return LaunchDescription(
        [
            #参数声明
            escort_back,
            escort_left,
            escort_right,
            turtle1_name,
            point_back,
            point_left,
            point_right,
            #启动乌龟节点和生成乌龟
            turlte_node,
            spawn_turtle_back,
            spawn_turtle_left,
            spawn_turtle_right,
            #发布坐标系相对变换 turtlex->base_link
            tf_broadcaster_1,
            tf_broadcaster_back,
            tf_broadcaster_left,
            tf_broadcaster_right,
            #目标点相对于turtle1的静态变换
            tf_point_back_broadcaster,
            tf_point_left_broadcaster,
            tf_point_right_broadcaster,
            #监听坐标系相对变换并发布速度指令
            back_escort_goal_back,
            left_escort_goal_left,
            right_escort_goal_right,
        ]
    )