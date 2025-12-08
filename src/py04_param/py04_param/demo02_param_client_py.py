import rclpy
from rclpy.node import Node
from rcl_interfaces.srv import GetParameters

"""
    当前ros2并没有设置参数客户端的api,所以只能创建节点,然后通过节点对象来获取参数
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

class ParamClient(Node):
    def __init__(self,str1):
        super().__init__(str1)
        self.get_logger().info(f"成功创建节点{str1}")
        #创建参数客户端
        self.param_client_ = self.create_client(GetParameters,"/Param_server_py/get_parameters")
        #获取参数
        request = GetParameters.Request()
        request.names = ["sdy_name"]
        #等待相应
        while self.param_client_.wait_for_service(timeout_sec=1.0) == False:
            self.get_logger().info("服务未启动,等待中...")
        #发送请求
        self.futrue = self.param_client_.call_async(request)

def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    node = ParamClient("Param_client_py")
    rclpy.spin_until_future_complete(node, node.futrue)
    node.get_logger().info(f"sdy_name = {node.futrue.result().values[0].string_value}")

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()