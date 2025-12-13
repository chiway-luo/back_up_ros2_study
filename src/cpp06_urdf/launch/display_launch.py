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
# from launch.event_handlers import OnProcessStart, OnProcessExit
# from launch.actions import ExecuteProcess, RegisterEventHandler,LogInfo
# 获取功能包下share目录路径-------
from ament_index_python.packages import get_package_share_directory

from launch_ros.parameter_descriptions import ParameterValue
from launch.substitutions import Command
"""
    需求:加载urdf文件并在rviz2中显示机器人模型
    核心步骤:
        1.启动robot_satte_publisher节点,该节点要以参数的方式加载urdf文件内筒;
        2.启动rviz2节点
    优化:
        1.添加joint_state_publisher节点,当机器人有非固定关节时,必须包含该节点,用于发布关节状态信息
        2.设置rviz2的配置文件,
        3.动态传入urdf文件,把urdf文件封装为参数
"""
def generate_launch_description():
    # 1.启动robot_satte_publisher节点,该节点要以参数的方式加载urdf文件内筒;
    # p_value = ParameterValue(value=Command(["xacro ",get_package_share_directory('cpp06_urdf') + '/urdf/urdf/demo01_helloworld.urdf']),value_type=str)
    #调用格式 
    #  ros2 launch cpp06_urdf display_launch.py model_path:=`ros2 pkg prefix --share cpp06_urdf`/urdf/urdf/demo01_helloworld.urdf
    model_path = DeclareLaunchArgument(name='model_path',default_value=get_package_share_directory('cpp06_urdf') + '/urdf/urdf/demo01_helloworld.urdf')
    p_value = ParameterValue(value=Command(["xacro ",LaunchConfiguration('model_path')]),value_type=str)
    robot_state_pub = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        parameters=[{'robot_description': p_value}]
    )
    # joint_state_pub = Node(
    #     package="joint_state_publisher",
    #     executable="joint_state_publisher"
    # )
    joint_state_pub_gui = Node(
        package="joint_state_publisher_gui",
        executable="joint_state_publisher_gui"
    )
    # 2.启动rviz2节点
    rviz2_node = Node(
        package='rviz2',
        executable='rviz2',
        arguments=['-d', get_package_share_directory('cpp06_urdf') + '/config/Nav_test.rviz']
    )
    return LaunchDescription(
        [
            model_path,
            # joint_state_pub,
            joint_state_pub_gui,
            rviz2_node,
            robot_state_pub
        ]
    )

"""
    问题描述: 通过joint_state_publisher_gui 让关节运行到指定位置后,关节会存在抖动
    使用指令查看: ros2 topic info /joint_stateses
    问题原因: 
    1.  joint_state_publisher_gui 会持续发布/joint_states消息
    2.  joint_state_publisher 发布的是默认的空数据
    3.  robot_state_publisher 会订阅关节的运动信息,并生成坐标变换数据广播 
    
    存在两个/joint_states消息源,导致冲突,产生抖动

    都不存在,坐标树报错

"""