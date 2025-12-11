import rclpy
from rclpy.node import Node
from tf2_ros import TransformBroadcaster
from turtlesim.msg import Pose
from geometry_msgs.msg import TransformStamped
import tf_transformations
"""
    需求:广播两只乌龟相对于base_link坐标系的关系
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建动态坐标变换广播器
            3-2创建订阅方,订阅乌龟的位姿
            3-3在回调中将位姿数据转换为坐标系相对关系然后广播
        4.调用spin函数,并传入节点对象指针
        5.释放资源
"""

class TFBroadcaster(Node):
    def __init__(self,str1):
        super().__init__(str1)
        self.get_logger().info(f"成功创建节点{str1}")
        #创建参数对象
        self.declare_parameter("child_frame_id","turtle1")
        self.declare_parameter("frame_id","base_link")
        #创建动态坐标系变换广播器
        self.tf_broadcaster = TransformBroadcaster(self)
        #创建订阅方,订阅乌龟的位姿
        self.sub_pose = self.create_subscription(
            Pose,
            f"/{self.get_parameter('child_frame_id').get_parameter_value().string_value}/pose",
            self.pose_cb,10
        )
        

    def pose_cb(self,msg: Pose):
        #创建坐标系变换消息
        self.tfs = TransformStamped()

        self.tfs.header.stamp = self.get_clock().now().to_msg()
        self.tfs.header.frame_id = self.get_parameter("frame_id").get_parameter_value().string_value
        self.tfs.child_frame_id = self.get_parameter("child_frame_id").get_parameter_value().string_value

        self.tfs.transform.translation.x = msg.x
        self.tfs.transform.translation.y = msg.y
        self.tfs.transform.translation.z = 0.0
        #欧拉角转换为四元数
        qtn = tf_transformations.quaternion_from_euler(0,0,msg.theta)
        self.tfs.transform.rotation.x = qtn[0]
        self.tfs.transform.rotation.y = qtn[1]
        self.tfs.transform.rotation.z = qtn[2]
        self.tfs.transform.rotation.w = qtn[3]

        #发布坐标系变换消息
        self.tf_broadcaster.sendTransform(self.tfs)

def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    rclpy.spin(TFBroadcaster("name"))

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()