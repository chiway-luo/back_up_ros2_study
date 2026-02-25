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
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
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
# from ament_index_python.packages import get_package_share_directory
# urdf文件处理相关--------------
# from launch_ros.parameter_descriptions import ParameterValue
# from launch.substitutions import Command

"""
    为单独的一辆车发送导航请求的坐标
"""
def generate_launch_description():
    ld = LaunchDescription()

    # ld.add_action(DeclareLaunchArgument('car_num',default_value='robot_1'))
    # car_num = LaunchConfiguration('car_num').perform()


    # this->declare_parameter("car_num","robot_1");
    #     this->declare_parameter("main_car_frame","robot_0/base_link");
    #     this->declare_parameter("goal_frame","robot_1/goal");
    #     this->declare_parameter("follow_car_frame","robot_1/base_link");
    #     this->declare_parameter("x_offset",1.0);
    #     this->declare_parameter("y_offset",0.0);
    #     this->declare_parameter("yaw_offset",0.0);//单位为弧度
    
    static_tf_node_1 = Node(
        package='mycar_multi',
        executable='convoy',
        name='robot_1_convoy',
        output='screen',
        parameters=[{
            'car_num': 'robot_1',
            'main_car_frame': 'robot_0/base_link',
            'goal_frame': 'robot_1/goal',
            # 'follow_car_frame': 'robot_1/base_link',
            'x_offset': -0.5,
            'y_offset': -0.5,
            'yaw_offset': 0.0
        }]
    )

    static_tf_node_2 = Node(
        package='mycar_multi',
        executable='convoy',
        name='robot_2_convoy',
        output='screen',
        parameters=[{
            'car_num': 'robot_2',
            'main_car_frame': 'robot_0/base_link',
            'goal_frame': 'robot_2/goal',
            # 'follow_car_frame': 'robot_1/base_link',
            'x_offset': -0.5,
            'y_offset': 0.5,
            'yaw_offset': 0.0
        }]
    )

    ld.add_action(static_tf_node_1)
    ld.add_action(static_tf_node_2)

    return ld

