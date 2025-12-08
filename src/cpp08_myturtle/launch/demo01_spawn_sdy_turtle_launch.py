from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import ExecuteProcess

def generate_launch_description():
    n1 = Node(package='cpp08_myturtle',executable='demo01_spawn_sub_pub')
    n2 = Node(package='turtlesim',executable='turtlesim_node')

    x = 6
    y = 9
    theta = 1.57
    name = 'sdy_turtle'
    #启动客户端对象 生成小乌龟
    # ros2 service call /spawn turtlesim/srv/Spawn "{'x': 3,'y': 4,'theta': 1.57,'name': 'sdy_turtle'}"
    spawn = ExecuteProcess(
        cmd=[f"ros2 service call /spawn turtlesim/srv/Spawn \"{{'x': {x},'y': {y},'theta': {theta},'name': '{name}'}}\""],
        output="both",
        shell=True
    )
    return LaunchDescription(
        [
            n2,
            spawn,
            n1
        ]
    )