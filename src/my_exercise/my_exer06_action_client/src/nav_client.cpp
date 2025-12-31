#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "my_exer_interfaces/action/nav.hpp" //包含自定义的动作头文件

/*
    取消请求使用自定义终止变量实现
    需求:向服务端发送数据(前进距离),并处理服务端发送的连续反馈和最终响应
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建运动客户端对象
            3-2编写发布请求响应
            3-3处理服务端返回的请求数据是否合法
            3-4接收服务端发送的连续反馈数据和最终响应
            3-5特定条件下发送取消请求响应
                a.客户端按下ctrl+c
                b.发送取消任务请求
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/

using namespace std::chrono_literals; //使用时间命名空间
using namespace std::placeholders; //占位符命名空间
using my_exer_interfaces::action::Nav; //使用自定义动作接口

static std::atomic_bool g_stop{false};//全局变量,用于接管Ctrl+C信号

class NavClient :public rclcpp::Node{
public:
    NavClient(std::string str1):Node(str1){
        RCLCPP_INFO(this->get_logger(),"namesapce: node: %s 节点创建成功",str1.c_str());
        //创建运动客户端对象
        nav_client_ = rclcpp_action::create_client<Nav>(this,"nav_action");
    }

    //编写发布请求响应
    void send_goal(double distance){
        while(!nav_client_->wait_for_action_server(1s)){
            if (g_stop.load())
            {
                RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"程序被中断");
                return;
            }
            RCLCPP_WARN(rclcpp::get_logger("rclcpp"),"等待动作服务端连接...");
        }
        connected_server = true;
        //写入全局对象
        target_distance = distance;
        //创建请求对象
        Nav::Goal goal_msg;
        goal_msg.distance = distance;
        //封装回调函数
        rclcpp_action::Client<Nav>::SendGoalOptions options;
        /*
            using GoalHandle = ClientGoalHandle<ActionT>;

            using GoalResponseCallback = std::function<void (typename GoalHandle::SharedPtr)>;
            using FeedbackCallback = typename GoalHandle::FeedbackCallback;
            using ResultCallback = typename GoalHandle::ResultCallback;

        */
        options.goal_response_callback = std::bind(&NavClient::goal_response_callback,this,_1);
        options.feedback_callback = std::bind(&NavClient::feedback_callback,this,_1,_2);
        options.result_callback = std::bind(&NavClient::result_callback,this,_1);
        //发送目标请求
        nav_client_->async_send_goal(
            goal_msg,
            options
        );
    }

    void cancel_goal(){
        if (!goal_handle_) {
            RCLCPP_WARN(this->get_logger(), "还没有 goal_handle,无法取消");
            return;
        }
        RCLCPP_WARN(this->get_logger(), "发送取消请求...");
        cancel_future_ = nav_client_->async_cancel_goal(goal_handle_);
    }
    // 新增：main 里用来等待取消完成
    bool has_cancel_future() const { return cancel_future_.valid(); }
    auto get_cancel_future() { return cancel_future_; }
    bool connected_server = false;//是否连接到服务端
    bool goal_canceled = false;//是否被拒绝或者取消
    bool goal_finished = false;//目标是否完成

private:
    rclcpp_action::Client<Nav>::SharedPtr nav_client_;
    double target_distance;
    rclcpp_action::ClientGoalHandle<Nav>::SharedPtr goal_handle_;//储存目标句柄指针
    // 新增：保存 cancel future（为了等待 cancel 发完/回包）
    std::shared_future<rclcpp_action::Client<Nav>::CancelResponse::SharedPtr> cancel_future_;

    //回调函数
    //处理服务端请求响应是否合法
    void goal_response_callback(rclcpp_action::ClientGoalHandle<Nav>::SharedPtr goal_handle){
        if (goal_handle == nullptr)
        {
            RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"目标请求被拒绝!!!");
            goal_canceled = true;

        }else{
            RCLCPP_INFO(rclcpp::get_logger("rclcpp"),"目标请求被接受!!!");
            goal_handle_ = goal_handle;
        }

    }
    //处理服务端发送的连续反馈数据
    void feedback_callback(rclcpp_action::ClientGoalHandle<Nav>::SharedPtr goal_handle,
                            const std::shared_ptr<const Nav::Feedback> feedback){
        (void)goal_handle;
        RCLCPP_INFO(this->get_logger(),"当前移动距离: %.2f / %.2f , 剩余距离 %.2f ,(%.2f, %.2f)",
                                        this->target_distance - feedback->diff_distance,
                                        this->target_distance,
                                        feedback->diff_distance,
                                        feedback->x,feedback->y
                                    );
    }
    //处理服务端发送的最终响应
    void result_callback(const rclcpp_action::ClientGoalHandle<Nav>::WrappedResult & result){
        switch (result.code)
        {
        case rclcpp_action::ResultCode::SUCCEEDED:
            RCLCPP_INFO(this->get_logger(),"导航成功到达目标位置");
            break;
        case rclcpp_action::ResultCode::ABORTED:
            RCLCPP_ERROR(this->get_logger(),"导航被中止!!!");
            break;
        case rclcpp_action::ResultCode::CANCELED:
            RCLCPP_ERROR(this->get_logger(),"导航被取消!!!");
            break;
        default:
            RCLCPP_ERROR(this->get_logger(),"未知结果!!!");
            break;
        }
        RCLCPP_INFO(this->get_logger(),"响应位置: (%.2f, %.2f),实际距离 %.2f / 发送距离 %.2f ",
                                result.result->x,result.result->y,
                                result.result->actual_distance,
                                result.result->target_distance);
        goal_finished = true;
        return;
    }
};


int main(int argc, char * argv[])
{
    //判断提交的参数是否合法
    if (argc != 2)
    {
        RCLCPP_INFO(rclcpp::get_logger("rclcpp"),"请提交一个距离数据!!!");
        return 1;
    }
    //初始化ros2客户端
    // ✅关键：不要让 rclcpp 收到 Ctrl+C 直接 shutdown
    auto opt = std::make_shared<rclcpp::InitOptions>();
    opt->shutdown_on_signal = false;
    rclcpp::init(argc, argv, *opt);

    // ✅自己接管 Ctrl+C
    std::signal(SIGINT, [](int){ g_stop = true; });

    //创建节点类对象
    auto node = std::make_shared<NavClient>("navclient_node_cpp");

    //发送请求
    node->send_goal(std::atof(argv[1]));
    
    //调用spin函数,并传入节点对象指针
    rclcpp::executors::SingleThreadedExecutor exec;
    exec.add_node(node);

    // ✅用循环 spin_some，便于在 stop 时插入 cancel
    while (rclcpp::ok() && !g_stop.load() && node->goal_canceled == false && node->goal_finished == false) {
        exec.spin_some();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // ✅收到 Ctrl+C：先 cancel
    if (node->connected_server && node->goal_canceled == false && g_stop.load())// 连接到服务端 没有被拒绝 没有主动终止程序
    {
        node->cancel_goal();  //主动取消请求
        // ✅再给一点时间把取消请求发出去并等回包（可选但强烈建议）
        // if (node->has_cancel_future()) {
        //     exec.spin_until_future_complete(node->get_cancel_future(), std::chrono::seconds(1));
        // }
    }
    
    //释放资源
    rclcpp::shutdown();
    return 0;
}