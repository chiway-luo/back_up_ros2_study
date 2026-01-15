import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image

from array import array
import numpy as np

"""
    需求:发布动态图片消息
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

class SimCam(Node):
    def __init__(self,str1):
        super().__init__(str1)
        self.get_logger().info(f"成功创建节点{str1}")
        #创建发布方对象
        self.pub_ = self.create_publisher(Image,"image",10)
        #创建定时器对象
        self.timer_ = self.create_timer(0.05,self.timer_callback)
        #创建图片消息对象
        self.image_msg_ = Image()
        #定义动态变换的图片的相关变量
        self.arr = [0,0] #x y坐标
        self.init_flag_ = False
        self.move_step_ = True #True 向右移动 False 向左移动
        #初始化图片消息对象
        self.init_image_msg()


    def timer_callback(self):
        if self.init_flag_ == False:
            return
        #初始化data数据
        # self.image_msg_.data = [0] * (self.image_msg_.height * self.image_msg_.step)
        # self.image_msg_.data = np.zeros(self.image_msg_.height * self.image_msg_.step,dtype=np.uint8).tolist()
        self._img_view.fill(0)
        #判断边界
        if (self.move_step_ == True and self.arr[0] >= (self.image_msg_.width - 25)) or (self.move_step_ == False and self.arr[0] <= 25):
            self.move_step_ = not self.move_step_
        #填充数据
        for i in range(self.arr[0] - 25,self.arr[0] + 25):#x坐标
            for j in range(self.arr[1] - 25,self.arr[1] + 25):#y坐标
                index = j * self.image_msg_.step + i * 3
                self.image_msg_.data[index] = 255     #R
                self.image_msg_.data[index + 1] = 255   #G
                self.image_msg_.data[index + 2] = 255   #B
        #更新坐标
        self.arr[0] = self.arr[0] + (5 if self.move_step_ == True else -5)
        self.image_msg_.header.stamp = self.get_clock().now().to_msg()
        #发布图片消息
        self.pub_.publish(self.image_msg_)
                
        
        

    def init_image_msg(self):
        self.image_msg_.header.frame_id = "camera_frame"
        self.image_msg_.height = 480
        self.image_msg_.width = 640
        self.image_msg_.encoding = "rgb8"
        self.image_msg_.is_bigendian = 0
        self.image_msg_.step = self.image_msg_.width * 3
        #初始化data数据
        self.arr[1] = self.image_msg_.height // 2
        self.init_flag_ = True
        #构建全为0的data数据
        self.image_msg_.data = array('B', [0]) * (self.image_msg_.height * self.image_msg_.step)
        self._img_view = np.frombuffer(self.image_msg_.data, dtype=np.uint8)  # 0 拷贝视图
        


def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    rclpy.spin(SimCam("simcam_node_py"))

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()