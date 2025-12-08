#install(DIRECTORY launch DESTINATION share/PROJECT_NAME)
#<exec_depend>ros2launch</exec_depend>
#from glob import glob 用于setup.py配置多个launch文件
#('share/' + package_name, glob('launch/py/*_launch.py')),

from launch import LaunchDescription
from launch_ros.actions import Node
# 封装终端指令相关类--------------
# from launch.actions import ExecuteProcess
# from launch.substitutions import FindExecutable
# 参数声明与获取-----------------
# from launch.actions import DeclareLaunchArgument
# from launch.substitutions import LaunchConfiguration
# 文件包含相关-------------------
# from launch.actions import IncludeLaunchDescription
# from launch.launch_description_sources import PythonLaunchDescriptionSource
# 分组相关----------------------
from launch_ros.actions import PushRosNamespace
from launch.actions import GroupAction
# 事件相关----------------------
# from launch.event_handlers import OnProcessStart, OnProcessExit
# from launch.actions import ExecuteProcess, RegisterEventHandler,LogInfo
# 获取功能包下share目录路径-------
# from ament_index_python.packages import get_package_share_directory
"""
    需求:创建三个turtlesim_node节点,前两个划分为一组,第三个单独一组

    chiway@chiway-ros2-humble:~/study_code/stu_ros2/ws02_tools$ ros2 node list 
    /sdy_group1/turtlesim_node1
    /sdy_group1/turtlesim_node2
    /sdy_group2/turtlesim_node1
"""
def generate_launch_description():
    #创建三个节点
    n1 = Node(package='turtlesim',executable='turtlesim_node',name='turtlesim_node1')
    n2 = Node(package='turtlesim',executable='turtlesim_node',name='turtlesim_node2')
    n3 = Node(package='turtlesim',executable='turtlesim_node',namespace='sdy_group2',name='turtlesim_node1')
    #分组
    #设置当前组命名空间和包含的节点
    g1 = GroupAction(actions=[PushRosNamespace('sdy_group1'),n1,n2])
    g2 = GroupAction(actions=[n3])

    return LaunchDescription(
        [
            g1,
            g2
        ]
    )
