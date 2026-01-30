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
            'gz_args': f"-v 4 -r {os.path.join(demo_gazebo_sim_path,'world','house_add.sdf')}" #添加家具的房子模型
            # 'gz_args': f"-v 4 -r {os.path.join(get_package_share_directory('demo_gazebo_sim'),'world','visualize_lidar.sdf')}"
        }.items()
    )
    ld.add_action(gazebo_visualize_node)

    
    return ld

