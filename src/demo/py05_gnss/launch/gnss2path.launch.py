#install(DIRECTORY config params launch DESTINATION share/${PROJECT_NAME}) #cmake配置
#<exec_depend>ros2launch</exec_depend> <!--package.xml配置-->
#from glob import glob #用于setup.py配置多个launch文件
#('share/' + package_name, glob('launch/*_launch.py')),
#('share/' + package_name, glob('launch/*_launch.xml')),
#('share/' + package_name, glob('launch/*_launch.yaml')),
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

    #启动gnss仿真节点
    sim_gnss = Node(
        package="sim_gnss",
        executable="sim_gnss",
        name="sim_gnss",
        output="screen",
    )
    ld.add_action(sim_gnss)

    #启动gnss2path节点
    gnss2path = Node(
        package="py05_gnss",
        executable="gnss2path_py",
        name="gnss2path_py",
        output="screen",
    )
    ld.add_action(gnss2path)

    #启动rviz2节点
    rviz2 = Node(
        package="rviz2",
        executable="rviz2",
        name="rviz2",
        output="screen",
        arguments=["-d", os.path.join(
            get_package_share_directory("py05_gnss"),
            "config",
            "gnss2path.rviz",
        )],
    )
    ld.add_action(rviz2)

    return ld








# urdf实例代码:
"""
# 1.启动robot_satte_publisher节点,该节点要以参数的方式加载urdf文件内筒;
p_value = ParameterValue(value=Command(["xacro ",get_package_share_directory('cpp06_urdf') + '/urdf/urdf/demo01_helloworld.urdf']),value_type=str)
robot_state_pub = Node(
    package='robot_state_publisher',
    executable='robot_state_publisher',
    parameters=[{'robot_description': p_value}]
)
"""