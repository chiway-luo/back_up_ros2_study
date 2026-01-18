import rclpy
from rclpy.node import Node
from geometry_msgs.msg import PoseStamped
from sensor_msgs.msg import NavSatFix
from nav_msgs.msg import Path

"""
    需求:订阅gnss数据转换为path数据并发布
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

class Gnss2Path(Node):
    def __init__(self,str1):
        super().__init__(str1)
        self.get_logger().info(f"成功创建节点{str1}")
        self.pub_ = self.create_publisher(Path,"/gnss/path",10)#创建发布者对象
        self.sub_ = self.create_subscription(NavSatFix,"/gnss/fix",self.call_back,10)#创建订阅者对象
        self.path_msg_ = Path()#创建成员变量存储发布的数据
        self.first_flage_ = True#创建标志位变量

        self.origin_las_ = 0.0#存储原点经度
        self.origin_lon_ = 0.0#存储原点纬度
        self.origin_alt_ = 0.0#存储原点海拔

        #初始化数据
        self.path_msg_.header.frame_id = "map"
        
        

    def call_back(self,msg: NavSatFix):#回调函数
        if self.first_flage_:#当做原点
            self.origin_lat_ = msg.latitude
            self.origin_lon_ = msg.longitude
            self.origin_alt_ = msg.altitude
            self.first_flage_ = False
        
        #计算相对坐标
        delta_north = (msg.latitude - self.origin_lat_) * 111320.0
        delta_east = (msg.longitude - self.origin_lon_) * 111320.0
        delta_alt = msg.altitude - self.origin_alt_

        #封装posestamp数据
        pose_msg = PoseStamped()
        pose_msg.header.frame_id = "map"
        pose_msg.header.stamp = self.get_clock().now().to_msg()
        pose_msg.pose.position.x = float(delta_east)
        pose_msg.pose.position.y = float(delta_north)
        pose_msg.pose.position.z = float(delta_alt)
        pose_msg.pose.orientation.x = 0.0
        pose_msg.pose.orientation.y = 0.0
        pose_msg.pose.orientation.z = 0.0
        pose_msg.pose.orientation.w = 1.0
        
        #将pose数据添加到path数据中
        self.path_msg_.poses.append(pose_msg)
        self.path_msg_.header.stamp = self.get_clock().now().to_msg()

        #判断path数据长度,超过100个删除最早的数据
        if len(self.path_msg_.poses) > 100:
            self.path_msg_.poses.pop(0)
        #发布path数据
        self.pub_.publish(self.path_msg_)
        


        

def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    rclpy.spin(Gnss2Path("gnss2path_node_py"))

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()