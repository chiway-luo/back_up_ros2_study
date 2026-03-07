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
# 组件相关-------------
from launch_ros.actions import ComposableNodeContainer
from launch_ros.descriptions import ComposableNode

def generate_launch_description():
    ld = LaunchDescription()


    # 创建组件对象
    #组件对象
    # *, 
    # package: SomeSubstitutionsType, 
    # plugin: SomeSubstitutionsType, 
    # name: SomeSubstitutionsType | None = None, 
    # namespace: SomeSubstitutionsType | None = None, 
    # parameters: SomeParameters | None = None, 
    # remappings: SomeRemapRules | None = None, 
    # extra_arguments: SomeParameters | None = None, 
    # condition: Condition | None = None) -> ComposableNode
    listener_component = ComposableNode(
        package="component_talker_listener",
        plugin="my_componet::ListenerComponent",
        name="listener_component"
    )
    talker_component = ComposableNode(
        package="component_talker_listener",
        plugin="my_componet::TalkerComponent",
        name="talker_component"
    )

    # 创建组件容器,并加载组件
    container_node = ComposableNodeContainer(
        package= "rclcpp_components",
        executable= "component_container",
        name = "container_talker_listener",
        namespace="sdy",
        # composable_node_descriptions: Optional[List[ComposableNode]] = None,
        composable_node_descriptions=[
            listener_component,
            talker_component
        ]
    )
    ld.add_action(container_node)


    return ld
