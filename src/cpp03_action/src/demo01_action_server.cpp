#include "rclcpp/rclcpp.hpp"
#include "base_interfaces_demo/action/progress.hpp"
#include "rclcpp_action/rclcpp_action.hpp"

/*
    ros2 action send_goal /server_get_sum base_interfaces_demo/action/Progress "{'num': 10}"

    动作通信：由目标反馈和结果三部分组成
        目标发送实现是服务通信的封装,结果的获取也是对服务通信的封装，而连续反馈则是对话题通信的封装
    通信过程：
        客户端发送请求给服务端，服务端返回决定是否处理，客户端再次发送请求，
        服务端开始处理后会有连续反馈（由服务端单向传递给客户端）
        处理完成后服务端发送数据给客户端
        
    需求:编写动作服务端，需要解析客户端提交的数字，遍历该数字累加求和，最终结果响应回
    客户端，且请求响应过程中需要生成连续反馈
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建动作服务端
            3-2处理提交的返回值(回调函数)
            3-3处理取消请求(回调函数)
            3-4生成连续的返馈与最终响应(回调函数)
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/
using base_interfaces_demo::action::Progress;
using namespace std::chrono_literals;
using std::placeholders::_1;
using std::placeholders::_2;

class Action_server :public rclcpp::Node{
public:
    Action_server(std::string str1):Node(str1){
        RCLCPP_INFO(this->get_logger(),(str1+"节点创建成功").c_str());
        //创建动作服务端
        /*
            rclcpp_action::Server<ActionT>::SharedPtr create_server<ActionT, NodeT>
            (NodeT node, const std::string &name, 
            rclcpp_action::Server<ActionT>::GoalCallback handle_goal, 
            rclcpp_action::Server<ActionT>::CancelCallback handle_cancel,
            rclcpp_action::Server<ActionT>::AcceptedCallback handle_accepted, 
            const rcl_action_server_options_t &options = rcl_action_server_get_default_options(), 
            rclcpp::CallbackGroup::SharedPtr group = nullptr)
        */
        action_server_ = rclcpp_action::create_server<Progress>(
            this,//节点
            "get_sum",//话题名称
            /*
            /// Signature of a callback that accepts or rejects goal requests.

            using GoalCallback = std::function<GoalResponse(
                    const GoalUUID &, 
                    std::shared_ptr<const typename ActionT::Goal>)>;

            /// Signature of a callback that accepts or rejects requests to cancel a goal.

            using CancelCallback = std::function<CancelResponse
            (std::shared_ptr<ServerGoalHandle<ActionT>>)>;
            
            /// Signature of a callback that is used to notify when the goal has been accepted.

            using AcceptedCallback = std::function<void (std::shared_ptr<ServerGoalHandle<ActionT>>)>;
            */
            std::bind(&Action_server::handle_goal,this,_1,_2),//目标处理回调函数,
            std::bind(&Action_server::handle_cancel,this,_1),//取消处理回调函数,
            std::bind(&Action_server::handle_accepted,this,_1)//连续反馈处理回调函数,


        );
    }
    //析构函数
    ~Action_server()
    {
        // 析构时把线程收回来，避免进程退出时线程还在跑
        if (worker_thread_.joinable()) {
            worker_thread_.join();
        }
    }

private:
    rclcpp_action::Server<Progress>::SharedPtr action_server_;
    std::thread worker_thread_;

    /*
        /// Signature of a callback that accepts or rejects goal requests.

        using GoalCallback = std::function<GoalResponse(
                const GoalUUID &, 
                std::shared_ptr<const typename ActionT::Goal>)>;

        /// Signature of a callback that accepts or rejects requests to cancel a goal.

        using CancelCallback = std::function<CancelResponse
        (std::shared_ptr<ServerGoalHandle<ActionT>>)>;
        
        /// Signature of a callback that is used to notify when the goal has been accepted.
        
        using AcceptedCallback = std::function<void (std::shared_ptr<ServerGoalHandle<ActionT>>)>;
    */
    //目标处理回调函数
    rclcpp_action::GoalResponse handle_goal(
        const rclcpp_action::GoalUUID &uuid, std::shared_ptr<const typename Progress::Goal> goal){
        (void)uuid;//编码期间如果没有使用参数则需要void声明
        //业务逻辑: 判断提交的数字是否大于1.是就继续,否则退出
        if(goal->num <= 1){
            RCLCPP_ERROR(this->get_logger(),"提交的数据必须大于1");
            return rclcpp_action::GoalResponse::REJECT;
        }
        RCLCPP_INFO(this->get_logger(),"提交的数据合法");
        return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
    }

    //取消处理请求回调函数
    rclcpp_action::CancelResponse handle_cancel(
        std::shared_ptr<rclcpp_action::ServerGoalHandle<Progress>> goal_handle){
        (void)goal_handle;
        RCLCPP_INFO(this->get_logger(),"接收到任务取消请求");
        return rclcpp_action::CancelResponse::ACCEPT;
        
    }

    //连续反馈处理回调函数
    void handle_accepted(std::shared_ptr<rclcpp_action::ServerGoalHandle<Progress>> goal_handle){
        // std::thread(std::bind(&Action_server::excute,this,goal_handle)).detach();
        // 如果上一个目标的线程还在跑，简单起见：先等它结束
        if (worker_thread_.joinable()) {
            worker_thread_.join();
        }

        // 开一个新线程跑 execute
        worker_thread_ = std::thread(&Action_server::excute, this, goal_handle);

    }
    void excute(std::shared_ptr<rclcpp_action::ServerGoalHandle<Progress>> goal_handle){
        //1.生成连续反馈 返回给客户端
        /*
            void publish_feedback
            (std::shared_ptr<base_interfaces_demo::action::Progress_Feedback> feedback_msg)
        */
        // goal_handle->publish_feedback()
        //获取目标值 遍历累加 每循环一次计算进度 并作为连续反馈发布
        int num = goal_handle->get_goal()->num;
        int sum = 0;
        auto feedback = std::make_shared<Progress::Feedback>();
        //设置休眠
        rclcpp::Rate rate(1.0);
        auto result = std::make_shared<Progress::Result>();
        RCLCPP_INFO(this->get_logger(),"任务开始处理,目标值: %d",num);
        for (int i = 1; i <= num; i++)
        {
            if(!rclcpp::ok()){
                // result->sum = sum;
                // goal_handle->canceled(result);
                RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"任务被取消");
                return;
            }
            sum += i;
            double progress = i/(double)num;

            feedback->progress=progress;
            goal_handle->publish_feedback(feedback);
            RCLCPP_INFO(this->get_logger(),"连续反馈中... 进度: %d%%",int(progress*100));

            //判断是否接受到了取消请求
            //如果接收到了,终止程序
            // goal_handle->canceled()
            /*
                void canceled(
                std::shared_ptr<base_interfaces_demo::action::Progress_Result> result_msg)
            */
            //goal_handle->is_canceling()
            
            
            rate.sleep();
        }
        //2.生成最终响应结果
        /*
            void succeed
            (std::shared_ptr<base_interfaces_demo::action::Progress_Result> result_msg)
        */
        // goal_handle->succeed()
        if (rclcpp::ok())
        {
            auto result = std::make_shared<Progress::Result>();
            result->sum = sum;
            goal_handle->succeed(result);

        }
        RCLCPP_INFO(this->get_logger(),"任务处理完成,最终结果: %d",sum);
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<Action_server>("sdy_action_server_cpp"));

    //释放资源
    rclcpp::shutdown();
    return 0;
}