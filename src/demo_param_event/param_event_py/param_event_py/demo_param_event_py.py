import rclpy
from rclpy.node import Node
from rclpy.parameter import Parameter
from typing import List
from rcl_interfaces.msg import SetParametersResult

"""
    需求:
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

class ParamEvent(Node):
    def __init__(self,str1):
        super().__init__(str1)
        self.get_logger().info(f"成功创建节点{str1}")
        # 声明参数
        self.declare_parameter("sdy_num",10)
        # 创建参数监听器,只能监听该节点
        self.add_on_set_parameters_callback(self.call_back)
    #设置事件处理
    # callback: Callable[[List[Parameter]], SetParametersResult]
    def call_back(self,params : List[Parameter]) -> SetParametersResult:#回调函数
        for param in params:
            self.get_logger().info(f"name: {param.name}, value: {param.value}")
        return SetParametersResult(successful=True)

def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    rclpy.spin(ParamEvent("paramevent_node"))

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()