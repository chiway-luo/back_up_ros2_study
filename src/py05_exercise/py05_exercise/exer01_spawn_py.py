import rclpy
from rclpy.node import Node
from turtlesim.srv import Spawn

"""
    需求:编写客户端实现,发送请求生成一只新的乌龟
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1使用参数服务声明新的乌龟信息
            3-2创建服务客户端对象,发送请求
            3-3连接服务器
            3-4组织并发布数据
        4.创建自定义节点类对象,调用函数,处理相相应结果
        5.释放资源
"""

class SpawnNode(Node):
    def __init__(self,str1):
        super().__init__(str1)
        self.get_logger().info(f"成功创建节点{str1}")
        #使用参数服务声明新的乌龟信息
        self.declare_parameter("turtle_name","turtle2")#声明参数
        self.declare_parameter("x",8.0)
        self.declare_parameter("y",9.0)
        self.declare_parameter("theta",3.14)
        self.x = self.get_parameter("x").get_parameter_value().double_value
        self.y = self.get_parameter("y").get_parameter_value().double_value
        self.theta = self.get_parameter("theta").get_parameter_value().double_value
        self.turtle_name = self.get_parameter("turtle_name").get_parameter_value().string_value

        #创建服务客户端对象
        self.client_ = self.create_client(
            srv_type=Spawn,
            srv_name="/spawn"
        )
        #在cpp中等待服务端连接的时候如果终端程序会重复打印日志,但是py中中断程序后不会打印日志,这是由于py是逐行解析并执行的
        while not self.client_.wait_for_service(timeout_sec=1.0):
            self.get_logger().warn("服务端未连接,等待中...")

    def send_request(self):
        #组织数据
        request = Spawn.Request()
        request.name = self.turtle_name
        request.x = self.x
        request.y = self.y
        request.theta = self.theta

        #发送数据 (request: SrvTypeRequest@call_async) -> Future
        self.future = self.client_.call_async(request)



    def call_back(self,msgs):#回调函数
        self.get_logger().info("数据是: %s"%msgs.data)

def main():
    #初始化ros2客户端
    rclpy.init()

    #创建自定义节点类对象,调用函数,处理响应结果
    spawn_node = SpawnNode("spawn_client")
    spawn_node.send_request()
    while rclpy.ok():
        rclpy.spin_until_future_complete(spawn_node,spawn_node.future)#节点 和 Future对象
        if spawn_node.future.done():
            if spawn_node.future.result().name != "":
                spawn_node.get_logger().info("乌龟生成成功,名称:%s"%spawn_node.future.result().name)
            else:
                spawn_node.get_logger().error("服务调用失败")
            break
        

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()