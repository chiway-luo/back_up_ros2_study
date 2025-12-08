import rclpy
from rclpy.node import Node
from base_interfaces_demo.msg import Student

"""
    需求:
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建
            3-2实现回调函数
            3-3在回调中解析并输出数据
        4.调用spin函数,并传入节点对象指针
        5.释放资源
"""

class ListenerNode(Node):
    def __init__(self,str1):
        super().__init__(str1)
        #创建订阅者对象
        self.create_subscription(Student,"stu_msgs",self.call_back,10)

    def call_back(self,msgs):#回调函数
        self.get_logger().info("接收到的消息: name: %s age: %d height: %f"%(msgs.name,msgs.age,msgs.height))

def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    node = ListenerNode("stu_lis_py")
    rclpy.spin(node)

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()