import rclpy
from rclpy.node import Node
from base_interfaces_demo.srv import AddInts

"""
    需求:
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建服务端
            3-2编写回调函数处理请求并产生相应
        4.调用spin函数,并传入节点对象指针
        5.释放资源
"""

class Server_Node(Node):
    def __init__(self,str1):
        super().__init__(str1)
        self.get_logger().info(f"成功创建服务端{str1}")
        #创建服务端对象
        self.server_ = self.create_service(AddInts,"sdy_add_ints",self.call_back)

    def call_back(self,request,response):#回调函数
        #打印收到的数据
        self.get_logger().info("数据是: num1: %d num2: %d"%(request.num1,request.num2))
        #处理数据
        response.sum = request.num1 + request.num2
        self.get_logger().info("响应的数据是: sum: %d"%response.sum)
        #返回数据
        return response

def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    rclpy.spin(Server_Node("server_py"))

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()