import rclpy
from rclpy.node import Node
import tf2_ros
from tf2_ros import TransformListener, Buffer

"""
    需求:先发布laser到base_link的坐标变换,再发布camera到base_link的坐标系相对关系
        求解laser在camera坐标系下的位置
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建缓存对象
            3-2创建监听器对象
            3-3编写定时器对象,循环实现转换
        4.调用spin函数,并传入节点对象指针
        5.释放资源
"""

class TfListener(Node):
    def __init__(self,str1):
        super().__init__(str1)
        self.get_logger().info(f"成功创建节点{str1}")
        self.tf_buffer_ = Buffer()#创建缓存对象
        self.tf_listener_ = TransformListener(self.tf_buffer_,self)#创建监听器对象
        self.timer_ = self.create_timer(1.0,self.call_back)#创建定时器对象
    def call_back(self):#回调函数
        future = self.tf_buffer_.can_transform("camera","laser",rclpy.time.Time())
        if future:
            self.transform_callback()
        else:
            self.get_logger().warn(f"等待转换...")

    def transform_callback(self):
        ts = self.tf_buffer_.lookup_transform("camera","laser",self.get_clock().now())
        self.get_logger().info(f"========转换完成的坐标帧信息=======")
        self.get_logger().info(f"父级坐标系: {ts.header.frame_id} 子级坐标系: {ts.child_frame_id} 偏移量: ({ts.transform.translation.x}, {ts.transform.translation.y}, {ts.transform.translation.z})")


def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    rclpy.spin(TfListener("node_name"))

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()