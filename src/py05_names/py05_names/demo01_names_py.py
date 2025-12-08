import rclpy
from rclpy.node import Node
from std_msgs.msg import String
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

class MyNode(Node):
    def __init__(self,str1):
        super().__init__(str1,namespace="sdy_namespace_py")
        self.get_logger().info(f"成功创建节点{str1}")
        #创建发布者对象
        #相对话题 /sdy_namespace_py/sdy_topic_py
        self.pub_ = self.create_publisher(String,"sdy_topic_py",10)
        #全局话题 /sdy_topic_py
        self.pub2 = self.create_publisher(String,"/sdy_topic_py",10)
        #私有话题 /sdy_namespace_py/sdy_name_py/sdy_topic_py
        self.pub3 = self.create_publisher(String,"~/sdy_topic_py",10)


    def call_back(self,msgs):#回调函数
        self.get_logger().info("数据是: %s"%msgs.data)

def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    rclpy.spin(MyNode("sdy_node_py"))

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()