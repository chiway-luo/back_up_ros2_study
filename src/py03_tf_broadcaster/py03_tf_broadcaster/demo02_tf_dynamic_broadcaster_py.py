import rclpy
from rclpy.node import Node
from tf2_ros import TransformBroadcaster
from turtlesim.msg import Pose
from geometry_msgs.msg import TransformStamped
import tf_transformations
"""
    需求:启动turtlesim_node 节点 编写程序,发布乌龟相对于窗体的位姿信息
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建动态坐标变换广播器
            3-2订阅乌龟位姿信息
            3-3回调函数中获取乌龟位姿并生成相对关系然后发布
        4.调用spin函数,并传入节点对象指针
        5.释放资源
"""

class TfDynamic(Node):
    def __init__(self,str1):
        super().__init__(str1)
        self.get_logger().info(f"成功创建节点{str1}")
        #创建动态广播器
        self.broadcaster_ = TransformBroadcaster(self)
        #创建乌龟位姿订阅方
        self.sub_turtle_pose_ = self.create_subscription(
            Pose,
            "turtle1/pose",
            self.call_back,
            10
        )


    def call_back(self,msgs: Pose):#回调函数
        #创建坐标变换消息
        self.tfs_ = TransformStamped()
        #填充消息
        self.tfs_.header.stamp = self.get_clock().now().to_msg()
        self.tfs_.header.frame_id = "world"
        self.tfs_.child_frame_id = "turtle1"

        self.tfs_.transform.translation.x = msgs.x
        self.tfs_.transform.translation.y = msgs.y
        self.tfs_.transform.translation.z = 0.0

        #将欧拉角转换为四元数
        qtn = tf_transformations.quaternion_from_euler(0,0,msgs.theta)

        self.tfs_.transform.rotation.x = qtn[0]
        self.tfs_.transform.rotation.y = qtn[1]
        self.tfs_.transform.rotation.z = qtn[2]
        self.tfs_.transform.rotation.w = qtn[3]

        #发布坐标变换
        self.broadcaster_.sendTransform(self.tfs_)

def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    rclpy.spin(TfDynamic("tf_dynamic_broadcaster_node_py"))

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()