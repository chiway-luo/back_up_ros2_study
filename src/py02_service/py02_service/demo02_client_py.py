import rclpy
from rclpy.node import Node
from base_interfaces_demo.srv import AddInts
import sys
from rclpy.logging import get_logger

"""
    需求:编写客户端实现，提交两个整型数据，并处理响应结果
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建客户端
            3-2连接服务端 如果没有连接则等待
            3-3组织并发送请求数据
        4.等待服务端响应
        5.接收服务端返回值
        6.释放资源
"""

class Client_Node(Node):
    def __init__(self,str1):
        super().__init__(str1)
        self.get_logger().info(f"成功创建客户端{str1}")
        #创建客户端对象
        self.client_ = self.create_client(AddInts,"sdy_add_ints")
        self.request = AddInts.Request()

    #连接客户端
    def conect_server(self):
        while not self.client_.wait_for_service(timeout_sec=1.0):
            if not rclpy.ok():
                self.get_logger().error("客户端中断")
                return False
            self.get_logger().warn("等待服务端连接...")
        return True
    """
        while rclpy.ok():
            if self.client_.wait_for_service(timeout_sec=1.0):
                self.get_logger().info("服务端已连接")
                return True
            self.get_logger().warn("等待服务端连接...")
        self.get_logger().error("客户端中断")
        return False
    """

    #组织数据
    def send_msgs(self,msgs):#回调函数
        # self.get_logger().info("数据是: num1: %d num2: %d"%(msgs[0],msgs[1]))
        self.request.num1 = msgs[0]
        self.request.num2 = msgs[1]
        self.get_logger().info("数据是: num1: %d num2: %d"%(msgs[0],msgs[1]))
        # self.future = self.client_.call_async(AddInts.Request(num1=msgs[0],num2=msgs[1]))
        self.future = self.client_.call_async(self.request)

def main():
    #判断传入参数数量
    if len(sys.argv) !=3:
        get_logger("rclpy").error("参数错误!请传入两个整数")
        return
    #初始化ros2客户端
    rclpy.init()
    #创建客户端对象
    client = Client_Node("client_py")
    #连接服务端 
    flag = client.conect_server()
    if not flag:
        return 
    
    
    #组织请求数据 获取服务端返回值
    client.send_msgs((int(sys.argv[1]),int(sys.argv[2])))
    #处理响应结果
    rclpy.spin_until_future_complete(client,client.future)
    try:
        response = client.future.result()
    except Exception as e:
        client.get_logger().error("服务调用失败 %r"% (e,))
    else:
        client.get_logger().info("响应的数据是: sum: %d"%response.sum)

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()