import rclpy
from rclpy.node import Node
from sensor_msgs.msg import LaserScan
from math import pi as M_PI
"""
    需求:
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建
            3-2实现回调函数
            3-3在回调中解析并输出数据
        4.调用spin函数,并传入节点对象指针
        5.释放资源
"""

class SimLaser(Node):
    def __init__(self,str1):
        super().__init__(str1)
        self.get_logger().info(f"成功创建节点{str1}")
        #创建雷达消息发布方
        self.sub_ = self.create_publisher(LaserScan,"scan",10)
        #创建定时器
        self.timer_ = self.create_timer(0.05,self.time_callback)
        #创建雷达消息对象
        self.sensor_msg_ = LaserScan()
        #初始化雷达消息对象
        self.init_laser_msg(frame_id="laser",pov=360.0,num_points=720, range_min=0.12, range_max=3.5, rate=20.0)
        #消息初始化标志位
        self.msg_init_flag_ = False
        self.distance = 9.0#模拟距离值


    def time_callback(self,msgs):#回调函数
        #判断雷达消息是否初始化完成
        if self.msg_init_flag_ == False:
            return
        #组织雷达消息
        self.sensor_msg_.header.stamp = self.get_clock().now().to_msg()#时间戳
        if self.distance <= 0.12:
            self.distance = 9.0
        #填充距离数据
        self.sensor_msg_.ranges = [self.distance for i in range(0,720)]
        self.distance -= 0.05
        #发布雷达消息
        self.sub_.publish(self.sensor_msg_)

    def init_laser_msg(self,frame_id="laser",pov=360.0,num_points=720, range_min=0.12, range_max=3.5, rate=20.0):
        self.sensor_msg_.header.frame_id = frame_id#坐标系id
        #计算起始角度
        half_pov = pov / 2.0
        self.sensor_msg_.angle_min = half_pov * M_PI / 180.0 * -1#起始角度
        self.sensor_msg_.angle_max = half_pov * M_PI / 180.0#结束角度
        self.sensor_msg_.angle_increment = self.sensor_msg_.angle_max / (num_points / 2)#角度增量
        self.sensor_msg_.time_increment = 1/rate/num_points#时间增量 同一圈扫描里相邻两束之间的时间间隔
        self.sensor_msg_.scan_time = 1/rate#扫描时间 一圈扫描所需的时间
        self.sensor_msg_.range_min = range_min#最小范围
        self.sensor_msg_.range_max = range_max#最大范围
        # int num_points = static_cast<int>((sensor_msg_.angle_max - sensor_msg_.angle_min) / sensor_msg_.angle_increment)#扫描点数
        # self.sensor_msg_.ranges.resize(num_points,0)#调整范围数组大小
        self.msg_init_flag_ = True#雷达消息初始化完成
        return

def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    rclpy.spin(SimLaser("simlaser_node_py"))

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()