#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "base_interfaces_demo/action/nav.hpp"
#include "turtlesim/msg/pose.hpp"

/*
    需求:创建action客户端节点,发送新生成的乌龟的实时坐标数据xytheta 给服务端,接收连续反馈,并输出最终结果
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1接收新乌龟位姿数据
            3-2在动作客户端中请求数据
            3-3如果乌龟位姿发生变化,则发送取消之前的目标,并发送新的目标
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/
using base_interfaces_demo::action::Nav;
using std::placeholders::_1;
using std::placeholders::_2;
using namespace std::chrono_literals;

class ActionCleintNode :public rclcpp::Node{
public:
    ActionCleintNode(std::string str1):Node(str1){
        RCLCPP_INFO(this->get_logger(),(str1+"节点创建成功").c_str());
        //创建运动客户端对象指针
        action_client_ = rclcpp_action::create_client<Nav>(this,"/action_turtle");
        //订阅新乌龟位姿
        pose_sub_ = this->create_subscription<turtlesim::msg::Pose>(
            "/sdy_turtle/pose",
            10,
            std::bind(&ActionCleintNode::sub_pose,this,_1)
        );
    }

    void action_send_goal(){
        //连接服务端
        while(!action_client_->wait_for_action_server(1s)){
            RCLCPP_INFO(this->get_logger(),"正在连接服务端...");
            if(!rclcpp::ok()){
                RCLCPP_ERROR(this->get_logger(),"客户端被中断,退出!");
                return;
            }
        }
        RCLCPP_INFO(this->get_logger(),"已连接服务端!");
        //组织并发送数据
        Nav::Goal goal;
        goal.goal_x = pose_msg_->x;
        goal.goal_y = pose_msg_->y;
        goal.goal_theta = pose_msg_->theta;

        rclcpp_action::Client<Nav>::SendGoalOptions goal_options;//创建发送目标选项对象
        
        //发送目标回调函数
        /* 
        std::function<void 
        (std::shared_ptr<rclcpp_action::ClientGoalHandle<base_interfaces_demo::action::Nav>>)> 
        rclcpp_action::Client<base_interfaces_demo::action::Nav>::SendGoalOptions::goal_response_callback
        */
        goal_options.goal_response_callback = std::bind(
           &ActionCleintNode::goal_response_callback,
           this,
           std::placeholders::_1
        );    
        
        //连续目标回调函数
        /*
        using FeedbackCallback = typename GoalHandle::FeedbackCallback;
        using GoalHandle = ClientGoalHandle<ActionT>;
        std::function
        <void 
        (std::shared_ptr<rclcpp_action::ClientGoalHandle<base_interfaces_demo::action::Nav>>, 
        std::shared_ptr<const base_interfaces_demo::action::Nav_Feedback>)> 
        rclcpp_action::Client<base_interfaces_demo::action::Nav>::SendGoalOptions::feedback_callback
        */
        goal_options.feedback_callback = std::bind(
           &ActionCleintNode::feedback_callback,
           this,
           std::placeholders::_1,
           std::placeholders::_2
        );
        //最终结果回调函数
        /* 
        std::function<void 
        (const rclcpp_action::ClientGoalHandle<base_interfaces_demo::action::Nav>::WrappedResult &result)> 
        rclcpp_action::Client<base_interfaces_demo::action::Nav>::SendGoalOptions::result_callback
        */
        goal_options.result_callback = std::bind(
           &ActionCleintNode::result_callback,
           this,
           std::placeholders::_1
        );
        
        action_client_->async_send_goal(goal,goal_options);  
    }

private:
    rclcpp_action::Client<Nav>::SharedPtr action_client_;//运动客户端对象指针
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr pose_sub_;//订阅新乌龟位姿
    turtlesim::msg::Pose::SharedPtr pose_msg_;//存储新乌龟的位姿数据
    turtlesim::msg::Pose::SharedPtr last_goal_pose_;//上一个目标位姿
    
    rclcpp_action::ClientGoalHandle<Nav>::SharedPtr current_goal_handle_; // 当前目标的句柄（用它来取消）
    rclcpp::Time last_goal_time_;//上次目标发送时间
    bool has_active_goal_ = false;//是否有活动目标
    

    bool has_last_goal_pose_ = false;

    //回调函数
    void sub_pose(const turtlesim::msg::Pose::SharedPtr msg){
        pose_msg_ = msg;
        rclcpp::Rate rate(1s);
        // rate.sleep();

        // 第一次收到位姿，先发一个目标
        if (!has_last_goal_pose_) {
            last_goal_pose_ = msg;
            has_last_goal_pose_ = true;
            // RCLCPP_INFO(this->get_logger(), "第一次收到位姿，发送目标");
            action_send_goal();
            last_goal_time_ = this->now();
            return;
        }
        //判断位姿是否变化
        double dx = pose_msg_->x - last_goal_pose_->x;
        double dy = pose_msg_->y - last_goal_pose_->y;

        double d = std::sqrt(dx * dx + dy * dy);
        // 2. 变化太小 → 忽略
        if (d < 1) {
            return;
        }

        // 3. 冷却时间没到 → 忽略
        if ((this->now() - last_goal_time_).seconds() < 2.0) {
            return;
        }
        /* 
            // constant declarations
        static constexpr int8_t STATUS_UNKNOWN =
            0;
        static constexpr int8_t STATUS_ACCEPTED =
            1;
        static constexpr int8_t STATUS_EXECUTING =
            2;
        static constexpr int8_t STATUS_CANCELING =
            3;
        static constexpr int8_t STATUS_SUCCEEDED =
            4;
        static constexpr int8_t STATUS_CANCELED =
            5;
        static constexpr int8_t STATUS_ABORTED =
            6;
        */
        // 4. 如果当前有 goal 在执行，先取消，但【不要马上发新目标】
        if (current_goal_handle_) {
            if (has_active_goal_)
            {
                RCLCPP_INFO(this->get_logger(),"位姿发生变化,取消之前目标");
                cancel_current_goal();
                // 这里可以选择先不立刻发新目标，等下一次 pose 再发
                return;
            }
        }
        else if(current_goal_handle_ == nullptr){
            RCLCPP_INFO(this->get_logger(),"发送新目标");
            // 2. 更新“上一次目标”的位姿
            last_goal_pose_ = msg;
            //发送新的请求
            action_send_goal();
            last_goal_time_ = this->now();
        }
        
    }
    //处理目标值相关相应结果的回调函数
    void goal_response_callback(std::shared_ptr<rclcpp_action::ClientGoalHandle<Nav>> goal_handle){
        // RCLCPP_INFO(this->get_logger(),"当前为处理目标值函数");
        //服务端接收/拒绝
        if(!goal_handle){
            RCLCPP_ERROR(this->get_logger(),"服务端拒绝请求");
        }
        else{
            RCLCPP_INFO(this->get_logger(),"服务端已接收请求");
            current_goal_handle_ = goal_handle; 
            has_active_goal_ = true;
        }
        
        
    }
    //连续反馈回调函数
    rclcpp::Time last_print_feedback = this->now();
    void feedback_callback(std::shared_ptr<rclcpp_action::ClientGoalHandle<Nav>> goal_handle, 
                            std::shared_ptr<const Nav::Feedback> feedback){
        (void)goal_handle;
        // 2 秒打印一次反馈
        rclcpp::Time now = this->now();
        if((now - last_print_feedback).seconds() >= 2.0){
            RCLCPP_INFO(this->get_logger(),"收到服务端反馈:剩余距离: %f",feedback->distance);
            last_print_feedback = now;
        }
        
    }
    //处理最终响应结果的回调函数
    void result_callback(const rclcpp_action::ClientGoalHandle<Nav>::WrappedResult &result){
        switch (result.code)
        {
        case rclcpp_action::ResultCode::SUCCEEDED:
            RCLCPP_INFO(this->get_logger(),"当前目标响应成功!");
            RCLCPP_INFO(this->get_logger(),"当前追踪乌龟坐标(%f,%f,%f)",result.result->turtle_x,result.result->turtle_y,result.result->turtle_theta);
            break;
        case rclcpp_action::ResultCode::ABORTED:
            RCLCPP_ERROR(this->get_logger(),"当前目标被服务端中止!");
            break;
        case rclcpp_action::ResultCode::CANCELED:
            RCLCPP_WARN(this->get_logger(),"当前目标被取消!");
            // RCLCPP_INFO(this->get_logger(),"位姿变化,请求被更改,重新发送中");
            break;
        default:
            RCLCPP_ERROR(this->get_logger(),"未知结果码!");
            break;
        }
        has_active_goal_ = false;
        current_goal_handle_.reset();  // 本地句柄置空

    }

    //取消当前目标函数
    void cancel_current_goal()
    {
        if (current_goal_handle_) {
            RCLCPP_INFO(this->get_logger(), "取消当前目标...");
            action_client_->async_cancel_goal(current_goal_handle_);
        }
    }
    
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<ActionCleintNode>("action_client_node"));

    //释放资源
    rclcpp::shutdown();
    return 0;
}