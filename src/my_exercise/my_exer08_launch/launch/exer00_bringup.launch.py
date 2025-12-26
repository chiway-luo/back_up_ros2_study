#install(DIRECTORY config params launch DESTINATION share/${PROJECT_NAME}) #cmake配置
#<exec_depend>ros2launch</exec_depend> #package.xml配置
#from glob import glob #用于setup.py配置多个launch文件
#('share/' + package_name, glob('launch/*_launch.py')),
#('share/' + package_name, glob('launch/*_launch.xml')),
#('share/' + package_name, glob('launch/*_launch.yaml')),

from launch import LaunchDescription
from launch_ros.actions import Node
# 封装终端指令相关类--------------
from launch.actions import ExecuteProcess
# from launch.substitutions import FindExecutable   #FindExecutable(name="ros2")
# 参数声明与获取-----------------
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch.conditions import IfCondition
from launch.conditions import UnlessCondition #取反判断
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
    需求:
        启动仿真环境
"""
import os
def generate_launch_description():
#定义参数
    use_sim = DeclareLaunchArgument('use_sim',default_value='true',description='是否使用仿真环境')
    local = True  #本地运行还是远程运行

    ld = LaunchDescription()
    
    if not local:
        #########################################################
        ###############远程实体机器人启动##########################
        #########################################################
        #实际测试后,虽然可以远程启动但是无法终止程序
        # remote_cmd = (
        #     "source /opt/ros/humble/setup.bash && "
        #     "source /$HOME/ros2_exercise/install/setup.bash && "
        #     "export MYCAR_MODEL=stm32_4w && "
        #     "export USE_CAM=1 && "
        #     "export LIDAR=sl_A1 && "
        #     "ros2 launch mycar_bringup mycar_bringup.launch.py"
        # )
        """
            #配置底盘类型
            #export MYCAR_MODEL=stm32_2w
            export MYCAR_MODEL=stm32_4w
            #export MYCAR_MODEL=arduino

            #配置雷达类型
            export LIDAR=sl_A1
            # export LIDAR=ls_N10

            #配置摄像头是否使用1/0
            export USE_CAM=1
        """
    
        # ssh_node = ExecuteProcess(
        #     cmd=[
        #         "ssh", "-Y", "chiway@192.168.0.108",
        #         f"bash -lc '{remote_cmd}'"
        #     ],
        #     output="screen",
        #     emulate_tty=True
        # )
        # ld.add_action(ssh_node)
    else:
        #########################################################
        ################本地仿真环境启动##########################
        #########################################################
        #包含仿真环境launch文件
        stage_launch = IncludeLaunchDescription(
            launch_description_source=PythonLaunchDescriptionSource(
                os.path.join(
                    get_package_share_directory('stage_ros2'),'launch','my_house.launch.py'
                )
            ),
            condition=IfCondition(LaunchConfiguration('use_sim'))#添加后可以不使用if判断
        )
        

        # ld.add_action(stage_launch)

        # ld.add_action(use_sim)

        # ld.add_action(pub_vel_node)

    # return ld
    return LaunchDescription([
        use_sim,
        stage_launch,
    ])