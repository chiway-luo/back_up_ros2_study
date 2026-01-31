import rclpy
# from rclpy.lifecycle import LifecycleNode
from rclpy.lifecycle import TransitionCallbackReturn,Node,State


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
# def on_configure(self, state) -> TransitionCallbackReturn:
#         """Handle configure transition request."""
#         return TransitionCallbackReturn.SUCCESS

#     def on_cleanup(self, state) -> TransitionCallbackReturn:
#         """Handle cleanup transition request."""
#         return TransitionCallbackReturn.SUCCESS

#     def on_shutdown(self, state) -> TransitionCallbackReturn:
#         """Handle shutdown transition request."""
#         return TransitionCallbackReturn.SUCCESS

#     def on_activate(self, state) -> TransitionCallbackReturn:
#         """Handle activate transition request."""
#         return TransitionCallbackReturn.SUCCESS

#     def on_deactivate(self, state) -> TransitionCallbackReturn:
#         """Handle deactivate transition request."""
#         return TransitionCallbackReturn.SUCCESS

#     def on_error(self, state) -> TransitionCallbackReturn:
#         """Handle error transition request."""
#         return TransitionCallbackReturn.SUCCESS

class CycleNode(Node):
    def __init__(self,str1):
        super().__init__(str1)
        self.get_logger().info(f"成功创建生命周期节点{str1}")
    
    def on_configure(self, state: State) -> TransitionCallbackReturn:
        """Handle configure transition request."""
        self.get_logger().info(f"配置生命周期节点")
        return TransitionCallbackReturn.SUCCESS

    def on_cleanup(self, state: State) -> TransitionCallbackReturn:
        """Handle cleanup transition request."""
        self.get_logger().info(f"清理生命周期节点")
        return TransitionCallbackReturn.SUCCESS

    def on_shutdown(self, state: State) -> TransitionCallbackReturn:
        """Handle shutdown transition request."""
        self.get_logger().info(f"生命周期节点被销毁---------------")
        return TransitionCallbackReturn.SUCCESS

    def on_activate(self, state: State) -> TransitionCallbackReturn:
        """Handle activate transition request."""
        self.get_logger().info(f"激活生命周期节点")
        return TransitionCallbackReturn.SUCCESS

    def on_deactivate(self, state: State) -> TransitionCallbackReturn:
        """Handle deactivate transition request."""
        self.get_logger().info(f"停用生命周期节点")
        return TransitionCallbackReturn.SUCCESS

    # def on_error(self, state: State) -> TransitionCallbackReturn:
    #     """Handle error transition request."""
    #     return TransitionCallbackReturn.SUCCESS

def main():
    #初始化ros2客户端
    rclpy.init()

    #在Python中不需要创建节点指针,并传入node->get_node_base_interface()
    #直接传入节点对象即可
    life_node = CycleNode("lifecycle_node")
    rclpy.spin(life_node)

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()