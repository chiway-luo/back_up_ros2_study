# import rclpy 方式1

# def main():
#     # 初始化rclpy 初始化ros2客户端库
#     rclpy.init()
#     # 创建节点对象
#     node = rclpy.create_node('helloworld_sdy')
#     # 打印日志信息
#     node.get_logger().info('Hello World Python ROS2')
#     # 释放资源
#     rclpy.shutdown()


# if __name__ == '__main__':
#     main()


import rclpy
# 继承的写法
from rclpy.node import Node

#自定义类
class MyNode(Node):
    def __init__ (self,str1):
        super().__init__(str1)
        self.get_logger().info(str1 + " 节点创建成功")

def main():
    #初始化ros节点
    rclpy.init()
    #创建对象
    node1 = MyNode("sdy_node1")
    #资源释放
    rclpy.shutdown()




