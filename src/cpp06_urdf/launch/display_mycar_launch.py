from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
import os

"""
    当前为sw导出urdf文件测试launch文件  测试依赖于 test_car功能包 
"""

def generate_launch_description():
    # 找到 test_car 这个资源包的 share 目录
    test_car_share = get_package_share_directory('test_car')

    # URDF 文件路径（按你实际的文件名来）
    urdf_file = os.path.join(test_car_share, 'urdf', 'test_car.urdf')

    # 读取 URDF 内容，填到 robot_description 参数里
    with open(urdf_file, 'r') as f:
        robot_description = f.read()

    # 机器人状态发布节点
    rsp_node = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='test_car_state_publisher',
        output='screen',
        parameters=[{'robot_description': robot_description}]
    )
    joint_state_pub = Node(
        package="joint_state_publisher",
        executable="joint_state_publisher"
    )

    # 启动 RViz2，先用默认配置（想高级一点可以再加 -d 配置文件）
    rviz_node = Node(
        package='rviz2',
        executable='rviz2',
        arguments=["-d", os.path.join(test_car_share, 'config', 'Nav_test.rviz')],
        # name='test_car_rviz',
        output='screen'
    )

    return LaunchDescription([
        rsp_node,
        rviz_node,
        joint_state_pub

    ])
