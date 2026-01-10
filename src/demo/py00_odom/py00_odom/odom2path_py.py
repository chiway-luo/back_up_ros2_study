import rclpy
from rclpy.node import Node
from nav_msgs.msg import Path
from nav_msgs.msg import Odometry
from geometry_msgs.msg import Pose, PoseStamped
from builtin_interfaces.msg import Time
"""
    需求:相比于cpp版本,py中只实现无限制发布路径实现
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建里程计订阅节点(绑定回调函数)
            3-2创建路径数据发布节点
            3-3在回调中转换数据并发布路径数据
        4.调用spin函数,并传入节点对象指针
        5.释放资源
"""

class Odom2Path(Node):
    def __init__(self,str1):
        super().__init__(str1)
        self.get_logger().info(f"成功创建节点{str1}")
        #创建订阅节点
        self.sub_ = self.create_subscription(
            msg_type=Odometry,
            topic="odom",
            callback=self.call_back,
            qos_profile=10
        )
        #创建发布对象
        self.pub_ = self.create_publisher(
            msg_type=Path,
            topic="path",
            qos_profile=10
        )
        #创建路径对象
        self.path_msg_ = Path()
        #创建最后位姿信息
        self.last_pose_ = PoseStamped()

    def call_back(self,odom_msg: Odometry):#回调函数
        #给最后位姿信息赋值
        self.last_pose_.header.frame_id = odom_msg.header.frame_id
        #获取时间对象
        time = odom_msg.header.stamp
        #计算距离 阈值0.5m
        if self.current_distance(odom_msg.pose.pose,time) < 0.5:
            return
        #转换数据
        self.path_msg_.header.frame_id = odom_msg.header.frame_id
        self.path_msg_.header.stamp = time
        self._append_pose(odom_msg.pose.pose,time)#添加位姿信息,给last_pose_赋值
        #发布路径数据
        self.pub_.publish(self.path_msg_)


    def current_distance(self,pose: Pose,time: Time):
        #判断路径数据是否为空
        if len(self.path_msg_.poses) == 0:
            self.last_pose_.header.stamp = time
            self.last_pose_.pose = pose
            return 10e9
        #计算距离
        dx = pose.position.x - self.last_pose_.pose.position.x
        dy = pose.position.y - self.last_pose_.pose.position.y
        distance = (dx**2 + dy**2)**0.5
        #给last_pose_赋值
        if distance < 0.5:
            return 0.0
        self.last_pose_.header.stamp = time
        self.last_pose_.pose = pose
        return distance
    
    def _append_pose(self,pose: Pose,time: Time):
        new_pose = PoseStamped()
        new_pose.header.frame_id = self.last_pose_.header.frame_id
        new_pose.header.stamp = time
        new_pose.pose = pose
        self.path_msg_.poses.append(new_pose)
        #给last_pose_赋值
        self.last_pose_.pose = pose
        self.last_pose_.header.stamp = time
        

def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    rclpy.spin(Odom2Path("odom_2_path_py"))

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()