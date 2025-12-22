#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "my_exer_interfaces/action/nav.hpp"
#include "nav_msgs/msg/odometry.hpp"

/*
    动作通信服务端(最大请求数据不能超过3米,只能前进)
    需求:处理服务端提交的数据,控制机器人运动指定的距离,连续反馈剩余距离和当前坐标,
        最终返回机器人运动的距离和提交的距离和当前坐标
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建动作通信服务端
            3-2处理请求数据(判断是否合法)
            3-3向机器人控制节点发布速度消息
            3-4订阅里程计消息,计算当前运动距离
            3-5处理客户端取消请求
            3-6生成最终响应+连续反馈
        4.调用spin函数,并传入节点对象指针
        5.释放资源

    模拟客户端
    ros2 action send_goal /my_car/nav_action my_exer_interfaces/action/Nav "{'distance': 1}" -f
*/

using namespace std::chrono_literals; //使用时间命名空间
using namespace std::placeholders; //占位符命名空间
using my_exer_interfaces::action::Nav;
std::mutex odom_mutex_; //互斥锁

class NavServer :public rclcpp::Node{
public:
    NavServer(std::string str1,std::string str2):Node(str1,str2){
        //创建参数客户端对象
        param_client_ = std::make_shared<rclcpp::AsyncParametersClient>(this,"/my_car/pub_vel_node_cpp");
        //连接参数服务端
        while (!param_client_->wait_for_service(1s))
        {
            if (!rclcpp::ok())
            {
                RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"客户端连接被中断");
                return;
            }
            RCLCPP_WARN(rclcpp::get_logger("rclcpp"),"连接参数服务器中...");
        }

        RCLCPP_INFO(this->get_logger(),"namesapce: %s node: %s 节点创建成功",str2.c_str(),str1.c_str());
        // 3-1创建动作通信服务端
        action_server_ = rclcpp_action::create_server<Nav>(
            this,
            "/my_car/nav_action",
            /*
                rclcpp_action::Server<ActionT>::GoalCallback handle_goal, 
                rclcpp_action::Server<ActionT>::CancelCallback handle_cancel, 
                rclcpp_action::Server<ActionT>::AcceptedCallback handle_accepted

                using GoalCallback = std::function<GoalResponse(
                        const GoalUUID &, std::shared_ptr<const typename ActionT::Goal>)>;
                using CancelCallback = std::function<CancelResponse(std::shared_ptr<ServerGoalHandle<ActionT>>)>;
                using AcceptedCallback = std::function<void (std::shared_ptr<ServerGoalHandle<ActionT>>)>;
            */
            std::bind(&NavServer::handle_goal,this,_1,_2),//处理请求数据
            std::bind(&NavServer::handle_cancel,this,_1),//处理客户端取消请求
            std::bind(&NavServer::handle_accepted,this,_1)//处理最终响应和连续反馈

        );
        // 3-2处理请求数据(判断是否合法)
        // 3-3向机器人控制节点发布速度消息
        // 3-4订阅里程计消息,计算当前运动距离
        odom_sub_ = this->create_subscription<nav_msgs::msg::Odometry>(
            "/odom",
            10,
            std::bind(&NavServer::odom_callback,this,_1)
        );
        
        // 3-5处理客户端取消请求
        // 3-6生成最终响应+连续反馈
    }

private:
    rclcpp_action::Server<my_exer_interfaces::action::Nav>::SharedPtr action_server_;
    rclcpp::AsyncParametersClient::SharedPtr param_client_;
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_sub_;//订阅里程计消息对象指针
    nav_msgs::msg::Odometry::SharedPtr last_odom_;//上一次里程计消息
    bool first_odom_flag_ = false; //标志位,是否是第一次接收里程计消息

    //处理目标请求
    rclcpp_action::GoalResponse handle_goal(
        const rclcpp_action::GoalUUID & uuid, 
        std::shared_ptr<const Nav::Goal> goal){
        (void)uuid;
        //获取数据
        double distance = goal->distance;
        RCLCPP_INFO(this->get_logger(),"收到客户端请求,请求距离: %.2f 米",distance);
        if (!first_odom_flag_){//还未接收到里程计消息
            RCLCPP_WARN(this->get_logger(),"还未接收到里程计消息,拒绝请求");
            return rclcpp_action::GoalResponse::REJECT;
        }
        //处理请求数据
        if (distance <= 0)//请求距离不能小于等于0
        {
            RCLCPP_WARN(this->get_logger(),"请求距离小于等于0,拒绝请求");
            return rclcpp_action::GoalResponse::REJECT;
        }
        else if (distance > 3)//最大请求距离不能大于3米
        {
            RCLCPP_WARN(this->get_logger(),"距离超过3米,拒绝请求");
            return rclcpp_action::GoalResponse::REJECT;
        }
        RCLCPP_INFO(this->get_logger(),"请求距离合法,接收请求");
        return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
    }
    
    //处理客户端取消请求
    rclcpp_action::CancelResponse handle_cancel(std::shared_ptr<rclcpp_action::ServerGoalHandle<Nav>> goal_handle){
        (void)goal_handle;
        RCLCPP_WARN(this->get_logger(),"同意 客户端发送取消请求");
        param_client_->set_parameters({
                rclcpp::Parameter("linear_x",0.0),
                rclcpp::Parameter("angular_z",0.0)
        });
        return rclcpp_action::CancelResponse::ACCEPT;
    }
    //处理接收请求和连续反馈
    void handle_accepted(std::shared_ptr<rclcpp_action::ServerGoalHandle<Nav>> goal_handle){
        (void)goal_handle;
        //使用子线程处理请求
        std::thread{std::bind(&NavServer::execute,this,_1),goal_handle}.detach();
    }

    void execute(std::shared_ptr<rclcpp_action::ServerGoalHandle<Nav>> goal_handle){
        //处理请求数据,连续反馈,最终响应
        nav_msgs::msg::Odometry::SharedPtr start_odom;
        {
            std::lock_guard<std::mutex> lk(odom_mutex_);
            start_odom = last_odom_;//起始里程计数据
        }
        double goal_distance = goal_handle->get_goal()->distance;//获取请求距离
        double current_distance = 0.0;//当前运动距离
        param_client_->set_parameters({//发送移动数据
                rclcpp::Parameter("linear_x",0.1),
                rclcpp::Parameter("angular_z",0.0)
        });
        rclcpp::Rate rate(20); //频率20Hz
        rclcpp::Time start_time = this->now();//记录开始时间
        auto result_msg = std::make_shared<my_exer_interfaces::action::Nav_Result>();//最终响应结果指针
        auto feed_msg = std::make_shared<my_exer_interfaces::action::Nav_Feedback>();//连续反馈指针
        while (rclcpp::ok()){
            //记录当前坐标
            {
                std::lock_guard<std::mutex> lk(odom_mutex_);   
                feed_msg->x = last_odom_->pose.pose.position.x;
                feed_msg->y = last_odom_->pose.pose.position.y;
            }
            //计算当前移动距离
            current_distance = sqrt(
                pow((feed_msg->x - start_odom->pose.pose.position.x),2) +
                pow((feed_msg->y - start_odom->pose.pose.position.y),2)
            );
            //设置反馈数据
            feed_msg->diff_distance = goal_distance - current_distance;
            //判断是否取消
            if (goal_handle->is_canceling()){
                RCLCPP_ERROR(this->get_logger(),"目标请求被取消,停止运动");
                result_msg->x = feed_msg->x;
                result_msg->y = feed_msg->y;
                result_msg->actual_distance = current_distance;
                result_msg->target_distance = goal_distance;
                goal_handle->canceled(result_msg);
                break;
            }
            //判断是否完成
            if(current_distance >= goal_distance){
                RCLCPP_INFO(this->get_logger(),"任务完成 请求距离: %.2f 米,实际运动距离: %.2f 米",
                    goal_distance,current_distance);
                result_msg->x = feed_msg->x;
                result_msg->y = feed_msg->y;
                result_msg->actual_distance = current_distance;
                result_msg->target_distance = goal_distance;
                goal_handle->succeed(result_msg);
                break;
            }
            //发送反馈数据
            if (this->now()-start_time >= rclcpp::Duration(1s))
            {
                start_time = this->now();
                goal_handle->publish_feedback(feed_msg); 
            }
            rate.sleep();
        }
        // 退出时确保停一下
        param_client_->set_parameters({
            rclcpp::Parameter("linear_x", 0.0),
            rclcpp::Parameter("angular_z", 0.0)
        });
    }

    void odom_callback(const nav_msgs::msg::Odometry::SharedPtr msg){
        first_odom_flag_ = true;
        //计算机器人运动的距离
        {
            std::lock_guard<std::mutex> lk(odom_mutex_);
            last_odom_ = msg;
        }
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<NavServer>("navserver_node_cpp","my_car"));//node_name,namespace

    //释放资源
    rclcpp::shutdown();
    return 0;
}