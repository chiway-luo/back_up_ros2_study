from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import ExecuteProcess
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

    #调用客户端发送目标 6 9 0.0 --ros-args
    n1 = Node(package='cpp07_exercise',executable='exer04_spawn_client',arguments=[str(x),str(y),str(theta)]) 

    return LaunchDescription(
        [
            spawn,
            n1
        ]
    )