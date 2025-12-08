import rclpy
from rclpy.node import Node
from geometry_msgs.msg import PointStamped
import math

"""
    需求:创建消息发布节点,发布pointstamped类型消息
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建消息发布方对象
            3-2创建定时器对象并绑定回调函数
            3-3在回调中组织并发布数据
        4.调用spin函数,并传入节点对象指针
        5.释放资源
"""

class TfPoint(Node):
    def __init__(self,str1):
        super().__init__(str1)
        self.get_logger().info(f"成功创建节点{str1}")
        self.t = 0  #时间变量
        #创建消息发布对象
        self.pub_point_ = self.create_publisher(
            PointStamped,
            "point",
            10
        )
        #创建定时器对象
        self.timers_ = self.create_timer(
            0.5,
            self.call_back
        )

    def call_back(self):#回调函数
        #创建消息对象
        self.point_stamp = PointStamped()
        self.point_stamp.header.stamp = self.get_clock().now().to_msg()
        self.point_stamp.header.frame_id = "laser"

        R = 1 #半径
        #圆心偏移量
        x_c = 2
        y_c = 0
        #初始相位
        phi_0 = 3.14
        #角速度
        omega = 0.5
        #时间变量
        self.t += 1

        self.point_stamp.point.x = x_c + R *  math.cos((omega * self.t) + phi_0)
        self.point_stamp.point.y = y_c + R *  math.sin((omega * self.t) + phi_0)
        self.point_stamp.point.z = -0.1

        #发布消息
        self.pub_point_.publish(self.point_stamp)

def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    rclpy.spin(TfPoint("tf_point_broadcaster_node_py"))

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()