import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Imu 
from geometry_msgs.msg import TransformStamped
from tf2_ros import TransformBroadcaster

"""
    需求:订阅imu消息,提取旋转角度数据,生成坐标变换(机器人mycar相对于地面world的坐标变换)消息发布
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建订阅方对象
            3-2创建坐标变换广播器
            3-3
        4.调用spin函数,并传入节点对象指针
        5.释放资源
"""

class Imu2Tf(Node):
    def __init__(self,str1):
        super().__init__(str1)
        self.get_logger().info(f"成功创建节点{str1}")
        #3-1创建订阅方对象
        self.sub_ = self.create_subscription(Imu,"/imu",self.call_back,10)
        #3-2创建坐标变换广播器
        self.tf_bro_ = TransformBroadcaster(self)
        #3-3创建坐标变换消息对象
        self.tf_msg_ = TransformStamped()
        #填充不变的部分
        self.tf_msg_.header.frame_id = "world"
        self.tf_msg_.child_frame_id = "mycar"
        self.tf_msg_.transform.translation.x = 0.0
        self.tf_msg_.transform.translation.y = 0.0
        self.tf_msg_.transform.translation.z = 1.0


    def call_back(self,msg: Imu):#
        #提取四元数数据
        self.tf_msg_.transform.rotation = msg.orientation
        self.tf_msg_.header.stamp = self.get_clock().now().to_msg()
        #发布坐标变换
        self.tf_bro_.sendTransform(self.tf_msg_)

def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    rclpy.spin(Imu2Tf("imu2tf_node_py"))

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()