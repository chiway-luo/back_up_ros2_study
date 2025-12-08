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

class TalkerNode(Node):
    def __init__(self,str1):
        super().__init__(str1)
        self.get_logger().info(f"成功创建节点{str1}")
        #创建发布者对象
        self.sub = self.create_publisher(Student,"stu_msgs",10)  
        #创建计时器
        self.timer = self.create_timer(0.5,self.call_back)
    
    def call_back(self):#回调函数
        #组织学生信息并发布数据
        msgs = Student()
        msgs.name = "chiway"
        msgs.age = 18
        msgs.height = 1.72
        self.sub.publish(msgs)
        self.get_logger().info("发布的的数据是: name: %s age: %d height: %f"%(msgs.name,msgs.age,msgs.height))
        


def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    rclpy.spin(TalkerNode("stu_tal_py"))

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()