import rclpy
from rclpy.node import Node
from std_msgs.msg import String

"""
    需求:订阅发布方发布的消息,并在终端输出
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建订阅方
            3-2实现回调函数
            3-3在回调中解析并输出数据
        4.调用spin函数,并传入节点对象指针
        5.释放资源
"""

class Listener_sdy(Node):
    def __init__(self,str1):
        super().__init__(str1)
        self.get_logger().info(f"成功创建订阅方节点{str1}")
        #创建订阅方
        self.create_subscription(String,"chatter",self.on_time,10)
        
    def on_time(self,msgs):#回调函数
        self.get_logger().info("订阅到的数据是: %s"%msgs.data)

def main():
    #初始化ros客户端
    rclpy.init()

    #调用spin函数并传入类对象
    rclpy.spin(Listener_sdy("listener_sdy_py"))
    
    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()