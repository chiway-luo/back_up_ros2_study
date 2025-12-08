#include "rclcpp/rclcpp.hpp"
#include "base_interfaces_demo/action/progress.hpp"
#include "rclcpp_action/rclcpp_action.hpp"

/*
    需求:编写动作通信客户端,发送整型数据到客户端,并处理客户端的联系反馈和最终相应结果
    流程:
        前提:可以解析终端下动态传入的消息
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建动作客户端
            3-2发送请求
            3-3处理关于目标值的服务端相应
            3-4处理连续反馈
            3-5处理最终响应
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/
using base_interfaces_demo::action::Progress;
using namespace std::chrono_literals;
using std::placeholders::_1;
using std::placeholders::_2;


class Action_client :public rclcpp::Node{
public:
    Action_client(std::string str1):Node(str1){
        RCLCPP_INFO(this->get_logger(),(str1+"节点创建成功").c_str());
        // 3-1创建动作客户端
        action_client_ = rclcpp_action::create_client<Progress>(this,"get_sum");
        
    }

    bool send_goal(int num){
        // (void)num;
        //确保连接到服务端
        int count = 0;
        while (!action_client_->wait_for_action_server(1s))
        {
            count++;
            RCLCPP_WARN(rclcpp::get_logger("rclcpp"),"等待服务端连接...%ds",count);
            if (!rclcpp::ok())
            {
                RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"客户端被终止");
                return false;
            }
            
        }

        //防止连续反馈的过程数据丢失需要暂停0.5s
        rclcpp::sleep_for(500ms);

        // 3-2发送请求
        /*
            inline std::shared_future<...> rclcpp_action::Client<...>::async_send_goal
            (const base_interfaces_demo::action::Progress::Goal &goal, 
            const rclcpp_action::Client<...>::SendGoalOptions &options)
        */
        auto goal = Progress::Goal(); 
        goal.num = num;
        rclcpp_action::Client<Progress>::SendGoalOptions options;

        options.goal_response_callback = std::bind(&Action_client::goal_response_callback, this,_1);
        options.feedback_callback = std::bind(&Action_client::feedback_callback, this,_1,_2);
        options.result_callback = std::bind(&Action_client::result_callback, this,_1);

        auto future = action_client_->async_send_goal(goal,options);
        // 3-3处理关于目标值的服务端相应
        // 3-4处理连续反馈
        // 3-5处理最终响应
        return true;
    }
    //处理关于目标值的回调函数
    /* 
    using GoalHandle = ClientGoalHandle<ActionT>;
  using GoalResponseCallback = std::function<void (typename GoalHandle::SharedPtr)>;
    */
    void goal_response_callback(rclcpp_action::ClientGoalHandle<Progress>::SharedPtr goal_handle){
        //是否可以被接收
        if(!goal_handle){//如果指针为空
            RCLCPP_ERROR(this->get_logger(),"目标请求被服务端拒绝");
            return;
        }
        else{
            RCLCPP_INFO(this->get_logger(),"目标处理中");
        }

    }
    //处理关于连续反馈的回调函数
    /* 
        using GoalHandle = ClientGoalHandle<ActionT>;
        using FeedbackCallback = typename GoalHandle::FeedbackCallback;

        using FeedbackCallback =
    std::function<void (
        typename ClientGoalHandle<ActionT>::SharedPtr,
        const std::shared_ptr<const Feedback>)>;

    */
    void feedback_callback(rclcpp_action::ClientGoalHandle<Progress>::SharedPtr goal_handle,
        const std::shared_ptr<const Progress::Feedback> feedback){
        // (void)goal_handle;
        if (!rclcpp::ok())
        {
            RCLCPP_ERROR(this->get_logger(),"客户端被终止");
            action_client_->async_cancel_goal(goal_handle);
            return;
        }
        
        double progress = feedback->progress;
        RCLCPP_INFO(this->get_logger(),"当前进度%d%%",int(progress*100));
    }
    //处理最终响应的回调函数
    /* 
        using GoalHandle = ClientGoalHandle<ActionT>;
        using ResultCallback = typename GoalHandle::ResultCallback;

  using ResultCallback = std::function<void (const WrappedResult & result)>;
  using WrappedResult = typename GoalHandle::WrappedResult;


    */
    void result_callback(const rclcpp_action::ClientGoalHandle<Progress>::WrappedResult &result){
        //通过result的封装的状态码判断状态
        // rclcpp_action::ResultCode::
        /* 
            UNKNOWN
            SUCCEEDED
            CANCELED
            ABORTED 
        */
        if (result.code == rclcpp_action::ResultCode::SUCCEEDED)
        {
            RCLCPP_INFO(this->get_logger(),"最终结果: %ld",result.result->sum);
        }
        else if (result.code == rclcpp_action::ResultCode::ABORTED)
        {
            RCLCPP_ERROR(this->get_logger(),"任务中断！！！");
        }
        else if (result.code == rclcpp_action::ResultCode::CANCELED)
        {
            RCLCPP_WARN(this->get_logger(),"任务被取消");
        }
        else
        {
            RCLCPP_ERROR(this->get_logger(),"未知异常");
        }
        rclcpp::shutdown();//不加上,程序完成后主程序不会终止
        return;        
    }

private:
    std::shared_ptr<rclcpp_action::Client<base_interfaces_demo::action::Progress>> action_client_;
};

int main(int argc, char * argv[])
{
    if (argc != 2){
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"传入参数错误!请传入一个整数");
        return 1;
    }
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    auto node = std::make_shared<Action_client>("sdy_action_client_cpp");
    bool flag = node->send_goal(atoi(argv[1]));
    if (!flag)
    {
        return 1;
    }
    
    rclcpp::spin(node);

    //释放资源
    rclcpp::shutdown();
    return 0;
}