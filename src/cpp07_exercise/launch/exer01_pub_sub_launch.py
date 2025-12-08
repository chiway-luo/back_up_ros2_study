from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import ExecuteProcess,RegisterEventHandler
from launch.event_handlers import OnProcessExit 



def generate_launch_description():
    #启动两个turtlesim节点
    t1 = Node(package='turtlesim', executable='turtlesim_node')
    t2 = Node(package='turtlesim', executable='turtlesim_node',namespace='sdy_namespace')
    #控制第二个乌龟掉头

    # rotato = ExecuteProcess(
    #     cmd=[f"ros2 action send_goal /sdy_namespace/turtle1/rotate_absolute turtlesim/action/RotateAbsolute theta:\ 3.1415926\ "],
    #     output="both",#设置日志输出位置,既输出到文件也输出到终端
    #     shell=True #代表cmd中的命令是通过shell运行的
    # )

    # rotato = ExecuteProcess(
    #     cmd=["ros2 service call sdy_namespace/turtle1/teleport_absolute\turtlesim/srv/TeleportAbsolute \"{x: 5.4, y: 5.4, theta: 3.14}\" "],
    #     output="both",#设置日志输出位置,既输出到文件也输出到终端
    #     shell=True #代表cmd中的命令是通过shell运行的
    # )


    #调用自定义节点,并且该节点调用顺序有要求(要在掉头完毕才能执行)
    #需要通过注册事件完成
    #创建注册事件对象,在对象中声明针对哪个目标节点,在哪个事件触发时执行哪个操作
    exer01 = Node(package='cpp07_exercise', executable='exer01_pub_sub')
    # when_rotato_done = RegisterEventHandler(
    #     #创建一个新对象
    #     event_handler=OnProcessExit( #触发动作
    #         target_action=rotato, #目标节点
    #         on_exit=exer01 #触发执行的事件
    #     ),
    # )
    return LaunchDescription(
        [
            t1,
            t2,
            # rotato,
            # when_rotato_done
            exer01
        ]
    )