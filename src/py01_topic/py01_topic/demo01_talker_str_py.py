import rclpy
from rclpy.node import Node
from std_msgs.msg import String

"""
    1.包含头文件
    2.初始化ros2客户端
    3.定义节点类
        3-1.创建发布方
        3-2创建定时器
        3-3组织消息并发送
    4.调用spin函数，并传入节点指针
    5.释放资源
"""
class TalkerNode(Node):
    def __init__(self,str1):
        super().__init__(str1)
        self.get_logger().info(f"成功创建发布方{str1}")
        #创建消息发布方 消息类型 话题名称 QOS,队列长度 返回值:发布对象
        self.publisher = self.create_publisher(String,"chatter",10)
        #创建定时器
        self.timer = self.create_timer(1.0,self.on_timer)#时间 回调函数 返回值:定时器对象

        self.count = 0


    def on_timer(self):
        self.count+=1
        # str2 = f"水滴鱼发送的数据: {self.count}"
        msgs = String()
        msgs.data = f"水滴鱼发送的数据: {self.count}"
        self.publisher.publish(msgs)
        self.get_logger().info("发布的数据 %s"% msgs.data)

def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    node = TalkerNode("talk_sdy")
    rclpy.spin(node)

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()



