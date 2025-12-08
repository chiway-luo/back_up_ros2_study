import rclpy
from rclpy.action.server import ActionServer, ServerGoalHandle,GoalResponse,CancelResponse
from rclpy.node import Node
from base_interfaces_demo.action import Progress
import time
from rclpy.logging import get_logger


"""
    动作通信:由目标反馈和结果三部分组成
        目标发送实现是服务通信的封装,结果获取也是对五福通信的封装,而连续反馈则是对话题通信的封装
    通信过程:
        客户端发送请求给服务端,服务端返回决定是否处理,客户端再次发送请求,
        服务端开始处理后会有连续反馈(由服务端单向传递给客户端)
        处理完成后服务端发送数据给客户端
    需求:编写动作服务器,需要解析客户端提交的数
    字,遍历该数字累加求和,最终结果响应回客户端,且请求响应过程中需要生成连续反馈
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建动作服务器
            3-2处理提交的返回值(回调函数)--默认实现
            3-3处理取消请求(回调函数)--默认实现,可以重写回调函数
            3-4生成连续反馈与最终响应(回调函数)
        4.调用spin函数,并传入节点对象指针
        5.释放资源
"""

class Action_Node(Node):
    def __init__(self,str1):
        super().__init__(str1)
        self.get_logger().info(f"成功创建服务端节点{str1}")
        #创建服务端对象
        """ 
            (node: Any,
            action_type: Any,
            action_name: Any, 
        execute_callback: Any, *, 
        callback_group: Any | None = None, 
        goal_callback: Any = default_goal_callback, 
        handle_accepted_callback: Any = default_handle_accepted_callback, 
        cancel_callback: Any = default_cancel_callback, 
            goal_service_qos_profile: QoSProfile = qos_profile_services_default, 
            result_service_qos_profile: QoSProfile = qos_profile_services_default, 
            cancel_service_qos_profile: QoSProfile = qos_profile_services_default, 
            feedback_pub_qos_profile: QoSProfile = QoSProfile(depth=10), 
            status_pub_qos_profile: QoSProfile = qos_profile_action_status_default, 
            result_timeout: int = 900) -> ActionServer
        """
        
        self.action_server = ActionServer(
            self,
            Progress,
            "get_sum",
            self.execute_callback,                      # 必填：执行主逻辑
            goal_callback=self.goal_callback,          # 重写：处理收到目标请求
            # handle_accepted_callback=self.handle_accepted_callback,  # 重写：接受后怎么处理
            cancel_callback=self.cancel_callback       # 重写：取消请求如何响应
        )

    #执行总逻辑
    def execute_callback(self,goal_handle: ServerGoalHandle):
        #1.生成连续反馈
        num = goal_handle.request.num
        sum = 0
        self.get_logger().info(f"收到客户端请求,数据为{num}")

        for i in range(1,num+1):
            #取消请求处理
            if not rclpy.ok():
                goal_handle.canceled()
                get_logger("rclpy").error("目标请求被取消")
                return result

            sum += i
            feedback = Progress.Feedback()
            feedback.progress = (float(i)/float(num))
            goal_handle.publish_feedback(feedback)
            self.get_logger().info(f"连续反馈进度: %d%%"%(feedback.progress*100))
            time.sleep(1)
            
        #相应最终结果
        goal_handle.succeed()
        #生成最终结果
        result = Progress.Result()
        result.sum = sum
        self.get_logger().info(f"最终结果: {result.sum}")
        #2.响应最终结果
        return result

    #处理收到的目标请求
    def goal_callback(self,goal):
        num = goal.num
        if num <= 0:
            self.get_logger().warn(f"拒绝处理目标请求,目标值为{num}")
            return GoalResponse.REJECT
        self.get_logger().info(f"接受处理目标请求,目标值为{num}")
        return GoalResponse.ACCEPT
        
    #处理取消请求
    # def handle_accepted_callback(self):
    #     return self.cancel_callback()

    #取消请求如何相应
    def cancel_callback(self,goal_handle:ServerGoalHandle):
        goal_handle.canceled()
        self.get_logger().info("取消请求被接受")
        return CancelResponse.ACCEPT

def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    rclpy.spin(Action_Node("sdy_action_server_py"))

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()