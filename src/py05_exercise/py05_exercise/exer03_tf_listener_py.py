import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist #乌龟速度格式文件
from geometry_msgs.msg import TransformStamped
from tf2_ros import TransformListener,Buffer
import math

"""
    需求:监听坐标系变化广播数据,并生成turtle1相对于turtle的坐标系变换
        进而生成turtle2的运动指令
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1声明参数服务
            3-2创建缓冲区对象
            3-3创建坐标系变换监听器与缓冲区对象关联
            3-4创建速度发布方
            3-5创建定时器,回调函数中实现坐标变换,生成速度指令并发布
        4.调用spin函数,并传入节点对象指针
        5.释放资源

        ros2 run teleop_twist_keyboard teleop_twist_keyboard --ros-args --remap /cmd_vel:=/turtle1/cmd_vel
"""

class TFListener(Node):
    def __init__(self,str1):
        super().__init__(str1)
        self.get_logger().info(f"成功创建节点{str1}")
        self.declare_parameter("turtle1_name","turtle1")
        self.declare_parameter("turtle2_name","turtle2")
        #创建缓冲区对象
        self.buffer_ = Buffer()
        #创建坐标系变换监听器,与缓冲区对象关联
        self.tf_listener_ = TransformListener(self.buffer_,self)
        #创建速度发布方
        self.vel_pub_ = self.create_publisher(
            Twist,
            f"/{self.get_parameter('turtle2_name').get_parameter_value().string_value}/cmd_vel",10
        )
        #创建定时器
        self.timer_ = self.create_timer(0.5,self.timer_cb)


    def timer_cb(self):
        #进行坐标系转换
        if self.buffer_.can_transform(
            f"{self.get_parameter('turtle2_name').get_parameter_value().string_value}",
            f"{self.get_parameter('turtle1_name').get_parameter_value().string_value}",
            rclpy.time.Time() 
        ):
            transform_stamped: TransformStamped
            transform_stamped = self.buffer_.lookup_transform(
                f"{self.get_parameter('turtle2_name').get_parameter_value().string_value}",
                f"{self.get_parameter('turtle1_name').get_parameter_value().string_value}",
                rclpy.time.Time() 
            )#target_frame: str, source_frame: str,
            #创建速度消息
            vel_msg = Twist()
            #根据坐标系变换数据计算速度
            d_x = transform_stamped.transform.translation.x
            d_y = transform_stamped.transform.translation.y
            distance = (d_x**2 + d_y**2)**0.5
            angle = math.atan2(d_y,d_x)

            vel_msg.linear.x = 1 * distance
            vel_msg.angular.z = 2 * angle

            if distance < 0.1:
                vel_msg.linear.x = 0.0
                vel_msg.angular.z = 0.0

            #发布速度消息
            self.vel_pub_.publish(vel_msg)
        else:
            self.get_logger().warn("等待坐标系转换数据...")

def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    rclpy.spin(TFListener("name"))

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()