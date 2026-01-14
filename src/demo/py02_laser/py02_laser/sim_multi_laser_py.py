import rclpy
from rclpy.node import Node
from sensor_msgs.msg import PointCloud2
from sensor_msgs.msg import PointField

import numpy as np

import struct
from array import array

"""
    需求:生成并发布多线激光雷达消息
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建发布方
            3-2创建定时器
            3-3在回调中解析并输出数据
        4.调用spin函数,并传入节点对象指针
        5.释放资源
"""

class MultiLaer(Node):
    def __init__(self,str1):
        super().__init__(str1)
        self.get_logger().info(f"成功创建节点{str1}")
        #创建发布方
        self.pub_ = self.create_publisher(PointCloud2,"point_cloud",10)
        #创建定时器
        self.timer_ = self.create_timer(0.05,self.timer_call_back)
        #创建多线激光雷达消息
        self.laser_msg_ = PointCloud2()
        self.msg_flag_ = False
        self.init_multi_laser()
        self.distance_ = 10.0 #模拟障碍物的距离


    def timer_call_back(self):#回调函数
        if self.msg_flag_ == False:
            return
        #创建np数组
        # point_data = np.zeros(self.laser_msg_.height * self.laser_msg_.width * 4, dtype=np.float32)
        #清空数据
        
        #生成点云数据
        for i in range(self.laser_msg_.height): #32线
            for j in range(self.laser_msg_.width): #每线1800个点
                #计算点在数据中的起始位置
                # index = (i * self.laser_msg_.width + j) * int(self.laser_msg_.point_step / 4)
                # int(index)
                index = (i * self.laser_msg_.width + j) * self.laser_msg_.point_step
                #计算点坐标
                #实现水平墙壁推进 x轴负方向移动 y轴-5 ~ 5 z轴每行间距0.1
                x = self.distance_
                # point_data[index] = self.distance_
                y = -5.0 + 10.0 * j / self.laser_msg_.width
                # point_data[index + 1] = -5.0 + 10.0 * j / self.laser_msg_.width
                z = 0.1 * i
                # point_data[index + 2] = 0.1 * i
                intensity = 100.0
                # point_data[index + 3] = 100.0
                #将坐标转换为字节存入数据中
                
                # self.laser_msg_.data[index:index + 16] = struct.pack("<ffff", x, y, z, intensity)
                self.laser_msg_.data[index:index + 16] = array('B',struct.pack("<ffff", x, y, z, intensity))
                
        # self.laser_msg_.data = point_data.tobytes()
        #模拟墙壁推进
        self.distance_ -= 0.1
        if self.distance_ < -10.0:
            self.distance_ = 10.0
        #更新消息头时间戳
        self.laser_msg_.header.stamp = self.get_clock().now().to_msg()

        #发布数据
        self.pub_.publish(self.laser_msg_)

    def init_multi_laser(self):
        #初始化对线激光雷达消息
        self.laser_msg_.header.frame_id = "laser"
        self.laser_msg_.height = 32 #32线
        self.laser_msg_.width = 1800 #每线1800个点
        self.laser_msg_.is_bigendian = False #小端字节序
        self.laser_msg_.is_dense = True #没有无效点
        self.laser_msg_.point_step = 16 #每个点16字节 4 * float32
        self.laser_msg_.row_step = self.laser_msg_.point_step * self.laser_msg_.width
        #定义点云数据字段
        field_x = PointField()
        field_x.name = "x"
        field_x.offset = 0
        field_x.datatype = PointField.FLOAT32
        field_x.count = 1

        field_y = PointField()
        field_y.name = "y"
        field_y.offset = 4
        field_y.datatype = PointField.FLOAT32
        field_y.count = 1

        field_z = PointField()
        field_z.name = "z"
        field_z.offset = 8
        field_z.datatype = PointField.FLOAT32
        field_z.count = 1

        field_intensity = PointField()
        field_intensity.name = "intensity"
        field_intensity.offset = 12
        field_intensity.datatype = PointField.FLOAT32
        field_intensity.count = 1

        self.laser_msg_.fields = [field_x,field_y,field_z,field_intensity]

        #预分配点云数据内存
        # self.laser_msg_.data = bytearray(self.laser_msg_.row_step * self.laser_msg_.height)
        # self.laser_msg_.data = np.array(self.laser_msg_.row_step * self.laser_msg_.height, dtype=np.float32).tobytes()
        self.laser_msg_.data = bytearray(self.laser_msg_.row_step * self.laser_msg_.height)


        self.msg_flag_ = True
        return


def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    rclpy.spin(MultiLaer("multilaser_node_py"))

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()