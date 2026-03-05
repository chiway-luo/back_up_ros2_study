import rclpy
from rclpy.node import Node
from std_msgs.msg import String

from rclpy.executors import SingleThreadedExecutor#单线程执行器,默认就是单线程执行器
from rclpy.executors import MultiThreadedExecutor#多线程执行器,可以同时处理多个节点的回调函数,适用于多个节点需要同时处理回调函数的情况

import threading
"""
    需求:
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1
            3-2
            3-3
        4.调用spin函数,并传入节点对象指针
        5.释放资源
"""

class Pub(Node):
    def __init__(self,str1):
        super().__init__(str1)
        self.get_logger().info(f"成功创建节点{str1}")
        #创建发布方对象
        self.pub_ = self.create_publisher(String,"topic",10)
        #创建定时器对象,并绑定回调函数
        self.timer_ = self.create_timer(0.5,self._timer_callback)
    
    def _timer_callback(self):#回调函数
        msg = String()
        msg.data = "测试消息@"
        msg.data += str(self.get_clock().now().to_msg().nanosec)
        self.get_logger().info(f"发布方线程号: {threading.get_ident()}")
        self.pub_.publish(msg)

class Sub(Node):
    def __init__(self,str1):
        super().__init__(str1)
        self.get_logger().info(f"成功创建节点{str1}")
        self.sub_ = self.create_subscription(String,"topic",self._sub_callback,10)
    
    def _sub_callback(self,msg:String):#回调函数
        self.get_logger().info(f"接收到消息:{msg.data},时间间隔: {self.get_clock().now().to_msg().nanosec - int(msg.data.split('@')[1])}纳秒,当前线程号{threading.get_ident()}")

        pass


def main():
    #初始化ros2客户端
    rclpy.init()

    #将发布对象和订阅对象集成到同一进程内
    #创建进程执行器
    # executor = SingleThreadedExecutor()
    executor = MultiThreadedExecutor()

    #将节点对象添加进节点执行器
    executor.add_node(Pub("pub"))
    executor.add_node(Sub("sub"))
    #调用spin函数,并传入节点对象指针
    executor.spin()

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()