#install(DIRECTORY launch DESTINATION share/PROJECT_NAME)
#<exec_depend>ros2launch</exec_depend>
#from glob import glob 用于setup.py配置多个launch文件
#('share/' + package_name, glob('launch/py/*_launch.py')),

from launch import LaunchDescription
from launch_ros.actions import Node
# 封装终端指令相关类--------------
from launch.actions import ExecuteProcess
# from launch.substitutions import FindExecutable   #FindExecutable(name="ros2")
# 参数声明与获取-----------------
# from launch.actions import DeclareLaunchArgument
# from launch.substitutions import LaunchConfiguration
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

def generate_launch_description():
    point_pub_active = Node(
        package='cpp03_tf_broadcaster',
        executable='demo03_point_tf_broadcaster'
    )
    tf_pub1 = Node(
        package='tf2_ros',
        executable='static_transform_publisher',
        arguments=['0.4','0.0','0.2','0.0','0.0','0.0','base_link','laser']
    )
    tf_pub2 = Node(
        package='tf2_ros',
        executable='static_transform_publisher',
        arguments=['-0.5','0.0','0.4','0.0','0.0','0.0','base_link','camera']
    )
    #以上为发布坐标系变换节点和运动点坐标
    msg_filter_listener_pub = Node(
        package='cpp04_tf_listener',
        executable='demo02_msg_filter'
    )

    rviz_node = ExecuteProcess(
        cmd=['rviz2','-d','/home/chiway/study_code/stu_ros2/ws02_tools/my_set.rviz'],
        output='screen'
    )

    return LaunchDescription(
        [
            point_pub_active,
            tf_pub1,
            tf_pub2,
            msg_filter_listener_pub,
            rviz_node
        ]
    )