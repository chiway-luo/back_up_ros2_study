#install(DIRECTORY launch DESTINATION share/${PROJECT_NAME}) cmake配置
#<exec_depend>ros2launch</exec_depend> package.xml配置
#from glob import glob 用于setup.py配置多个launch文件
#('share/' + package_name, glob('launch/*_launch.py')),
#('share/' + package_name, glob('launch/*_launch.xml')),
#('share/' + package_name, glob('launch/*_launch.yaml')),

from launch import LaunchDescription
from launch_ros.actions import Node
# 封装终端指令相关类--------------
# from launch.actions import ExecuteProcess
# from launch.substitutions import FindExecutable   #FindExecutable(name="ros2")
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
from launch.event_handlers import OnProcessStart, OnProcessExit
from launch.actions import ExecuteProcess, RegisterEventHandler,LogInfo
# 获取功能包下share目录路径-------
# from ament_index_python.packages import get_package_share_directory
"""
    需求:实现乌龟护航案例 创建三只乌龟 ,要求乌龟最终位置在乌龟1的,偏移量(0,2)(0,-2)(-2,0)
          2  1  3
             4
    步骤:
        1.启动乌龟1
        2.生成乌龟2,3,4
        3.启动坐标变换节点 (turtle1->base_link,turtle2->base_link,turtle3->base_link,turtle4->base_link)

        ros2 run teleop_twist_keyboard teleop_twist_keyboard --ros-args --remap /cmd_vel:=/turtle1/cmd_vel

"""
def generate_launch_description():
    #########################################
    #参数声明
    turtle1_name = DeclareLaunchArgument(
        "turtle1_name",
        default_value="turtle1",
    )
    turtle2_name = DeclareLaunchArgument(
        "turtle2_name",
        default_value="turtle2",
    )
    turtle3_name = DeclareLaunchArgument(
        "turtle3_name",
        default_value="turtle3",
    )
    turtle4_name = DeclareLaunchArgument(
        "turtle4_name",
        default_value="turtle4",
    )
    #########################################
    #启动乌龟节点
    turtle_node = Node(
        package="turtlesim",
        executable="turtlesim_node",
        # name=LaunchConfiguration("turtle1_name"),
    )
    #########################################
    #生成乌龟
    spawn_turtle2 = Node(
        package="cpp05_exercise",
        executable="exer01_spawn",
        name="spawn_turtle2",
        parameters=[
            {'x': 1.0},
            {'y': 1.0},
            {'turtle_name': LaunchConfiguration("turtle2_name")}
        ]
    )
    spawn_turtle3 = Node(
        package="cpp05_exercise",
        executable="exer01_spawn",
        name="spawn_turtle3",
        parameters=[
            {'x': 9.0},
            {'y': 1.0},
            {'turtle_name': LaunchConfiguration("turtle3_name")}
        ]
    )
    spawn_turtle4 = Node(
        package="cpp05_exercise",
        executable="exer01_spawn",
        name="spawn_turtle4",
        parameters=[
            {'x': 4.0},
            {'y': 9.0},
            {'turtle_name': LaunchConfiguration("turtle4_name")}
        ]
    )
    #########################################
    #启动坐标变换节点 全部相对于base_link
    tf_broadcaster_1 = Node(
        package="cpp05_exercise",
        executable="exer02_tf_broadcaster",
        name="tf_broadcaster_turtle1",
        parameters=[
            {'turtle': LaunchConfiguration("turtle1_name")}
        ]
    )
    tf_broadcaster_2 = Node(
        package="cpp05_exercise",
        executable="exer02_tf_broadcaster",
        name="tf_broadcaster_turtle2",
        parameters=[
            {'turtle': LaunchConfiguration("turtle2_name")}
        ]
    )
    tf_broadcaster_3 = Node(
        package="cpp05_exercise",
        executable="exer02_tf_broadcaster",
        name="tf_broadcaster_turtle3",
        parameters=[
            {'turtle': LaunchConfiguration("turtle3_name")}
        ]
    )
    tf_broadcaster_4 = Node(
        package="cpp05_exercise",
        executable="exer02_tf_broadcaster",
        name="tf_broadcaster_turtle4",
        parameters=[
            {'turtle': LaunchConfiguration("turtle4_name")}
        ]
    )
    #########################################
    #监听坐标系变换并发布速度指令
    tf_listener_t2 = Node(
        package="cpp05_exercise",
        executable="exer03_tf_listener_test",
        name="tf_listener_turtle2",
        parameters=[
            {"frame_id": LaunchConfiguration("turtle2_name")},
            {"child_frame_id": LaunchConfiguration("turtle1_name")},
            {'goal_x_offset': 0.0},
            {'goal_y_offset': 1.0},
        ]
    )
    tf_listener_t3 = Node(
        package="cpp05_exercise",
        executable="exer03_tf_listener_test",
        name="tf_listener_turtle3",
        parameters=[
            {"frame_id": LaunchConfiguration("turtle3_name")},
            {"child_frame_id": LaunchConfiguration("turtle1_name")},
            {'goal_x_offset': 0.0},
            {'goal_y_offset': -1.0},
        ]
    )
    tf_listener_t4 = Node(
        package="cpp05_exercise",
        executable="exer03_tf_listener_test",
        name="tf_listener_turtle4",
        parameters=[
            {"frame_id": LaunchConfiguration("turtle4_name")},
            {"child_frame_id": LaunchConfiguration("turtle1_name")},
            {'goal_x_offset': -1.0},
            {'goal_y_offset': 0.0},
        ]
    )
    #########################################
    #########################################
    #生成乌龟逻辑
    step_spawn = RegisterEventHandler(
        event_handler=OnProcessStart(
            target_action=turtle_node,
            on_start=[
                spawn_turtle2,
                spawn_turtle3,
                spawn_turtle4,
                tf_broadcaster_1
            ],
        )
    )
    #启动监听节点逻辑
    """
        当spawn_turtle2节点启动后,启动tf_broadcaster_t2节点和tf_listener_t2节点
        当spawn_turtle3节点启动后,启动tf_broadcaster_t3节点和tf_listener_t3节点
        当spawn_turtle4节点启动后,启动tf_broadcaster_t4节点和tf_listener_t4节点
    """
    t2_start = RegisterEventHandler(
        event_handler=OnProcessExit(
            target_action=spawn_turtle2,
            on_exit=[tf_broadcaster_2, tf_listener_t2],
        )
    )

    t3_start = RegisterEventHandler(
        event_handler=OnProcessExit(
            target_action=spawn_turtle3,
            on_exit=[tf_broadcaster_3, tf_listener_t3],
        )
    )

    t4_start = RegisterEventHandler(
        event_handler=OnProcessExit(
            target_action=spawn_turtle4,
            on_exit=[tf_broadcaster_4, tf_listener_t4],
        )
    )

    return LaunchDescription(
        [
            #参数声明
            turtle1_name,
            turtle2_name,
            turtle3_name,
            turtle4_name,
            #节点启动
            turtle_node,
            step_spawn,
            # step_listener,
            t2_start,
            t3_start,
            t4_start,
            
        ]
    )