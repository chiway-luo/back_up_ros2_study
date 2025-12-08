import rclpy
from rclpy.node import Node
import time
import threading
from rclpy.time import Time
from rclpy.duration import Duration

def change_ns2s(ns):
    return ns/1e9

class MyNode(Node):
    def __init__(self,str1):
        super().__init__(str1)
        self.get_logger().info(f"成功创建节点{str1}")
        # self.demo_rate()
        # self.demo_time()
        # self.demo_duration()
        self.demo_time_duration()


    def demo_rate(self):
        #创建Rate对象
        self.rate = self.create_rate(1.0) #1Hz
        #ros2 python主线程中调用sleep函数会导致程序阻塞
        #解决方法一 直接使用time.sleep()
        self.count = 0
        # while rclpy.ok():
        #     time.sleep(1)
        #     # self.rate.sleep()
        #     self.count += 1
        #     self.get_logger().info(f"当前休眠时间: {self.count}s")
        #解决方法二 创建一个子线程
        thread = threading.Thread(target=self.do_someth)
        thread.start()

    def do_someth(self):
        while rclpy.ok():
            time.sleep(1)
            self.rate.sleep()
            self.count += 1
            self.get_logger().info(f"当前休眠时间: {self.count}s")

    def demo_time(self):
        #创建Time对象
        t1 = Time(seconds=1,nanoseconds=500000000)
        right_now = self.get_clock().now()

        #调用time函数
        self.get_logger().info(f"s = {t1.seconds_nanoseconds()[0]} nanoseconds = {t1.seconds_nanoseconds()[1]}")
        self.get_logger().info(f"s = {right_now.seconds_nanoseconds()[0]} nanoseconds = {right_now.seconds_nanoseconds()[1]}")
        self.get_logger().info(f"nanoseconds = {right_now.nanoseconds}")

    def demo_duration(self):
        #创建duration对象
        d1 = Duration(seconds=1,nanoseconds=500000000) #1.5s
        d2 = Duration(seconds=2,nanoseconds=200000000) #2.2s
        #调用duration函数
        self.get_logger().info(f"ns = {d1.nanoseconds}")
        self.get_logger().info(f"ns = {d2.nanoseconds}")
    
    #time与duration计算
    def demo_time_duration(self):
        t1 = Time(seconds=3)
        t2 = Time(seconds=5)
        d1 = Duration(seconds=2)
        t3 = t1 + d1
        d2 = t1 - t2
        d3 = t1 - d1
        self.get_logger().info(f"t1 + t2 = {t3.seconds_nanoseconds()[0]}s")
        self.get_logger().info(f"t1 - t2 = {change_ns2s(d2.nanoseconds)}s")
        self.get_logger().info(f"t1 - d1 = {change_ns2s(d3.nanoseconds)}s")

def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    rclpy.spin(MyNode("time_node"))

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()