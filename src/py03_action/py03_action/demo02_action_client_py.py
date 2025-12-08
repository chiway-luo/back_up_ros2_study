import rclpy
import sys
from rclpy.action import ActionClient
from rclpy.action.client import ClientGoalHandle
from rclpy.node import Node
from base_interfaces_demo.action import Progress
from rclpy.task import Future


"""
    需求:编写动作通信客户端，发送整型数据到客户端,并处理客户端的连续反馈
    和最终响应结果
    前提:可以解析终端下动态传入的消息
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建动作客户端
            3-2发送请求
            3-3处理关于目标值的服务端响应
            3-4处理连续反馈
            3-5处理最终响应
        4.调用spin函数,并传入节点对象指针
        5.释放资源
"""

class Action_client(Node):
    def __init__(self,str1):
        super().__init__(str1)
        self.get_logger().info(f"成功创建节点{str1}")
        #创建动作客户端
        self.action_client_ = ActionClient(
            self,
            Progress,
            "get_sum",
        )
    #发送请求
    def send_goal(self,num):
        #确保连接到服务端
        count = 0
        while self.action_client_.wait_for_server(timeout_sec=1.0) == False:
            count += 1
            self.get_logger().warn(f"等待服务端连接...{count}s")
            if not rclpy.ok():
                self.get_logger().error("客户端被终止")
                return False
        
        #发送请求
        goal = Progress.Goal()
        goal.num = num
        self.futrue = self.action_client_.send_goal_async(goal,self.fd_callback)
        self.futrue.add_done_callback(self.goal_response_callback)
        self.futrue.result

    #回调函数
    def fd_callback(self,feedback_msg):           
        #处理连续反馈
        progress = feedback_msg.feedback.progress
        self.get_logger().info(f"当前进度...{int(progress*100)}")
        pass

    def goal_response_callback(self,futrue:Future):
        #获取目标句柄
        goal_handle: ClientGoalHandle = futrue.result()
        #判断目标是否被正常接收
        if not goal_handle.accepted:
            self.get_logger().error("目标值被拒绝")
            return
        self.get_logger().info("目标值被接受")
        #处理最终响应
        self.result_futrue:Future = goal_handle.get_result_async()
        self.result_futrue.add_done_callback(self.get_result_callback)

    def get_result_callback(self,futrue:Future):
        result = futrue.result().result
        self.get_logger().info(f"最终结果为:{result.sum}")

def main():
    if len(sys.argv) != 2:
        print("请在终端下传入整数数据")
        return
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    main_client = Action_client("sdy_action_client_py")
    main_client.send_goal(int(sys.argv[1]))
    rclpy.spin(main_client)

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()