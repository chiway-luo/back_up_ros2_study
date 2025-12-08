import rclpy
from rclpy.node import Node

"""
    需求:创建参数服务端节点，一个普通节点就是参数服务端节点  
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1增加参数
            3-2获取参数
            3-3修改参数
            3-4删除参数 必须设置 allow_undeclared_parameters=True
        4.调用spin函数,并传入节点对象指针
        5.释放资源
"""

class ParamServer(Node):
    def __init__(self,str1):
        super().__init__(str1,allow_undeclared_parameters=True)
        self.get_logger().info(f"成功创建节点{str1}")

    #增加参数
    def add_param(self):
        self.get_logger().info(f"新增参数")
        self.declare_parameter("sdy_name","水滴鱼")
        self.declare_parameter("sdy_age",18)
        self.declare_parameter("sdy_height",1.59)
        #未声明参数
        self.set_parameters([rclpy.Parameter("sdy_weight",value=50.5)])
    
    #获取参数
    def get_param(self):
        self.get_logger().info(f"获取参数")
        #获取指定参数
        param1 = self.get_parameter("sdy_name")
        self.get_logger().info(f"name: {param1.name} value: {param1.value} type: {param1.Type.from_parameter_value(param1.value)}")
        #获取多个参数
        params = self.get_parameters(["sdy_name","sdy_age","sdy_height"])
        for i in params:
            self.get_logger().info(f"name: {i.name} value: {i.value} type: {i.Type.from_parameter_value(i.value)}")
        #判断是否存在某个参数
        self.get_logger().info(f"参数sdy_name是否存在: %s"%("是" if self.has_parameter('sdy_name')  else "否"))

    #修改参数
    def update_param(self):
        self.get_logger().info(f"修改参数")
        self.set_parameters([rclpy.Parameter("sdy_name",value="水滴鱼修改版本"),rclpy.Parameter("sdy_age",value=90)])



    #删除参数
    def delete_param(self):
        self.get_logger().info(f"删除参数")
        self.undeclare_parameter("sdy_height")
        self.get_logger().info(f"参数sdy_height是否存在: %s"%("是" if self.has_parameter('sdy_height')  else "否"))

def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    node = ParamServer("Param_server_py")

    node.add_param()
    node.get_param()
    node.update_param()
    node.delete_param()

    rclpy.spin(node)

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()