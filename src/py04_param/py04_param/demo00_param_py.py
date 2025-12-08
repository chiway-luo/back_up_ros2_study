import rclpy
from rclpy.node import Node

"""
    需求:演示参数api使用
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建参数对象
            3-2解析参数的值/键
        4.调用spin函数,并传入节点对象指针
        5.释放资源
"""

class ParameterNode(Node):
    def __init__(self,str1):
        super().__init__(str1)
        self.get_logger().info(f"成功创建param节点{str1}")
        #创建参数对象
        param1 = rclpy.Parameter("sdy_name",rclpy.Parameter.Type.STRING,"水滴鱼")
        param2 = rclpy.Parameter("sdy_age",rclpy.Parameter.Type.INTEGER,40)
        param3 = rclpy.Parameter("sdy_height",rclpy.Parameter.Type.DOUBLE,175.5)
        #解析参数
        self.get_logger().info(f"参数名称:{param1.name},参数值:{param1.value},参数类型:{param1.type_}")
        self.get_logger().info(f"参数名称:{param2.name},参数值:{param2.value},参数类型{param2.type_}")
        self.get_logger().info(f"参数名称:{param3.name},参数值:{param3.value},参数类型{param3.type_}")

def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    rclpy.spin(ParameterNode("Parameter_sdy"))

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()