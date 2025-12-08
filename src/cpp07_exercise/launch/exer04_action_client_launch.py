from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import ExecuteProcess

"""
    ros2 launch cpp07_exercise exer04_action_client_launch.py 
    ros2 run cpp07_exercise exer05_action_server 
    ros2 run teleop_twist_keyboard teleop_twist_keyboard --ros-args --remap __ns:=/sdy_turtle
"""
def generate_launch_description():

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
    turtle = ExecuteProcess(
        cmd=["ros2 run turtlesim turtlesim_node"],
        output="both",
        shell=True
    )

    #调用客户端订阅并发送目标请求
    n1 = Node(package='cpp07_exercise',executable='exer06_action_client') 

    return LaunchDescription(
        [
            turtle,
            spawn,
            n1
        ]
    )