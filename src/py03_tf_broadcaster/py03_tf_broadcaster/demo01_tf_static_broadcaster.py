import rclpy
from rclpy.node import Node
from tf2_ros.static_transform_broadcaster import StaticTransformBroadcaster
from geometry_msgs.msg import TransformStamped
import sys
from rclpy.logging import get_logger
import tf_transformations

"""
    需求:创建一个静态坐标变换广播节点
        ros2 run 包 可执行程序 x y z roll pitch yaw frame_id child_frame_id
    流程:
        1.包含头文件
        2.初始化ros2客户端 判断传入的参数是否合法
        3.自定义节点类
            3-1创建广播对象
            3-2组织并发布数据
        4.调用spin函数,并传入节点对象指针
        5.释放资源
"""

class TfBroadcaster(Node):
    def __init__(self,str1,transform):
        super().__init__(str1)
        self.get_logger().info(f"成功创建节点{str1}")
        #创建静态坐标变换广播对象
        self.broadcaster = StaticTransformBroadcaster(self)
        #组织并发布数据
        self.transform_pub(transform)
        
    def transform_pub(self,transform):
        tfs = TransformStamped()
        #设置参数
        tfs.header.stamp = self.get_clock().now().to_msg()
        tfs.header.frame_id = transform[7]
        tfs.child_frame_id = transform[8]

        tfs.transform.translation.x = float(transform[1])
        tfs.transform.translation.y = float(transform[2])
        tfs.transform.translation.z = float(transform[3])

        #转换欧拉角为四元数
        o2q = tf_transformations.quaternion_from_euler(float(transform[4]),float(transform[5]),float(transform[6]))
        
        tfs.transform.rotation.x = o2q[0]
        tfs.transform.rotation.y = o2q[1]
        tfs.transform.rotation.z = o2q[2]
        tfs.transform.rotation.w = o2q[3]

        self.broadcaster.sendTransform(tfs)

    def call_back(self,msgs):#回调函数
        self.get_logger().info("数据是: %s"%msgs.data)

def main():
    #判断传入的参数是否合法
    if len(sys.argv) != 9:
        get_logger("rclpy").error(f"传入的参数不合法")
        return 1
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    rclpy.spin(TfBroadcaster("tf_static_broadcaster_node_py",sys.argv))

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()