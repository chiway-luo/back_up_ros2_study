#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "base_interfaces_demo/action/nav.hpp"
#include "turtlesim/msg/pose.hpp"
#include "geometry_msgs/msg/twist.hpp"

/*
    需求:处理请求发送的目标点.控制乌龟向该目标点运动,并连续反馈乌龟与目标点之间的剩余距离
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类 
            创建原生乌龟位姿订阅方,获取当前乌龟坐标
            创建速度指令发布方,控制乌龟运动
            创建动作服务端,处理客户端发送的请求
            3-1目标处理回调函数
            3-2取消处理回调函数
            3-3连续反馈处理回调函数
            在子线程中产生连续反馈并响应连续反馈,发布速度指令,并响应最终结果
        4.调用spin函数,并传入节点对象指针
        5.释放资源

        ros2 action send_goal /action_turtle base_interfaces_demo/action/Nav "{'goal_x': 10,'goal_y': 9,'goal_theta': 3.14}" --feedback 
*/
using base_interfaces_demo::action::Nav;
using namespace std::chrono_literals;
double normalize_angle(double a);


class ActionNode :public rclcpp::Node{
public:
    ActionNode(std::string str1):Node(str1){
        RCLCPP_INFO(this->get_logger(),(str1+"节点创建成功").c_str());
        // 创建原生乌龟位姿订阅方,获取当前乌龟坐标
        // 3-1.创建订阅方(原生乌龟的位姿\turtle1\pose )
        pose_sub_ = this->create_subscription<turtlesim::msg::Pose>(
            "/turtle1/pose",
            10,
            std::bind(&ActionNode::callback_sub,this,std::placeholders::_1)
        );

        // 创建速度指令发布方,控制乌龟运动
        v_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel",10);
        // 创建动作服务端,处理客户端发送的请求
        action_server_ = rclcpp_action::create_server<Nav>(
            this,
            "action_turtle",
            /* 
                rclcpp_action::Server<ActionT>::GoalCallback handle_goal, 
                rclcpp_action::Server<ActionT>::CancelCallback handle_cancel, 
                rclcpp_action::Server<ActionT>::AcceptedCallback handle_accepted,
            */
           std::bind(&ActionNode::goal_handle_callback,this,std::placeholders::_1,std::placeholders::_2),
           std::bind(&ActionNode::cancel_handle_callback,this,std::placeholders::_1),
           std::bind(&ActionNode::feedback_handle_callback,this,std::placeholders::_1)
        );
    }

private:
    rclcpp_action::Server<Nav>::SharedPtr action_server_;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr pose_sub_;//订阅的原生乌龟位姿
    base_interfaces_demo::action::Nav::Goal::SharedPtr temp_pose_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr v_pub_;//发布速度消息的发布方
    turtlesim::msg::Pose::SharedPtr now_turtle_pose_ = nullptr;

    void callback_sub(const turtlesim::msg::Pose::SharedPtr msg){
        //订阅原乌龟节点的坐标信息
        now_turtle_pose_ = msg;
    }


    //回调函数
    /*  请求目标处理回调函数
        using GoalCallback = std::function<GoalResponse(
        const GoalUUID &, std::shared_ptr<const typename ActionT::Goal>)>;
    */
    rclcpp_action::GoalResponse goal_handle_callback(const rclcpp_action::GoalUUID & uuid,
        std::shared_ptr<const Nav::Goal> goal){
        (void)uuid;
        //取出目标中的xy坐标,分别判断是否超出窗体范围 0-11.08 超出就视为非法 theta范围-3.14~3.14
        // if(goal->goal_x < 0.0 || goal->goal_x > 11.08 || goal->goal_y < 0.0 || goal->goal_y > 11.08){
        //     RCLCPP_ERROR(this->get_logger(),"请求的目标点超出窗体范围,拒绝该目标请求");
        //     return rclcpp_action::GoalResponse::REJECT;
        // }
        RCLCPP_ERROR(this->get_logger(),"目标点合法,接受请求");
        return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
    }

    /* 
        取消处理回调函数
        using CancelCallback = std::function<CancelResponse(std::shared_ptr<ServerGoalHandle<ActionT>>)>;
    */
    rclcpp_action::CancelResponse cancel_handle_callback(std::shared_ptr<rclcpp_action::ServerGoalHandle<Nav>> goal_handle){
        (void)goal_handle;
        RCLCPP_ERROR(this->get_logger(),"同意取消请求");
        return rclcpp_action::CancelResponse::ACCEPT;
    }

    /* 
        连续反馈处理回调函数         主逻辑函数
        using AcceptedCallback = std::function<void (std::shared_ptr<ServerGoalHandle<ActionT>>)>;
    */
    void feedback_handle_callback(std::shared_ptr<rclcpp_action::ServerGoalHandle<Nav>> goal_handle){
        //在子线程中产生连续反馈,发布速度指令,并响应最终结果
        std::thread(std::bind(&ActionNode::handle_excute,this,goal_handle)).detach();

    }

    //子线程处理函数 方法一 xy方向都有速度,朝着目标点运行同时转变朝向,朝着目标点的朝向转动
    void handle_excute(std::shared_ptr<rclcpp_action::ServerGoalHandle<Nav>> goal_handle){
        auto result = std::make_shared<Nav::Result>();//最终响应结果指针
        //子线程处理主要逻辑
        RCLCPP_INFO(this->get_logger(),"进入子线程处理函数");
        //1.获取目标点坐标
        auto goal = goal_handle->get_goal();
        double goal_x = goal->goal_x;
        double goal_y = goal->goal_y;
        double goal_theta = goal->goal_theta;
        //2.生成连续反馈(发布乌龟运动指令,并反馈乌龟与目标点之间的剩余距离)
        /* 
            方程组 vx vy  vx0 vy0
            vx0 = vx cos(sita`) + vy sin(sita`)
            vy0 = vx sin(sita`) + vy cos(sita`)
            vy0/vx0 = tan(theta)
            v_t = sqrt(vx0^2 + vy0^2) 
            w_z = goal_theta - now_theta
        */
        rclcpp::Rate rate(10);
        //计算一次初始总距离
        double start_distance = std::sqrt(std::pow(goal_x - now_turtle_pose_->x,2) + std::pow(goal_y - now_turtle_pose_->y,2));
        double start_theta = now_turtle_pose_->theta;

        //开始发布时间
        rclcpp::Time start_time = this->now();
        while (true)
        {
            geometry_msgs::msg::Twist v_msg;
            //如果客户端发送了取消请求,则停止运动,退出循环
            if(goal_handle->is_canceling()){
                v_msg.linear.x = 0.0;
                v_msg.linear.y = 0.0;
                v_msg.angular.z = 0.0;
                v_pub_->publish(v_msg);
                goal_handle->canceled(result);
                RCLCPP_INFO(this->get_logger(),"目标取消,停止运动");
                return;
            }
            //计算乌龟与目标点之间的距离
            double dx = goal_x - now_turtle_pose_->x;
            double dy = goal_y - now_turtle_pose_->y;
            double d  = std::sqrt(dx*dx + dy*dy);//当前距离
            //计算世界系速度
            double vt = 1.0; //总线速度
            double vx = vt * (dx / d);
            double vy = vt * (dy / d);
            //世界系 → 小乌龟坐标系：
            double theta = now_turtle_pose_->theta;//当前朝向
            double vxb =  vx * std::cos(theta) + vy * std::sin(theta);
            double vyb = -vx * std::sin(theta) + vy * std::cos(theta);
            //姿态插值（注意 L 是初始距离、theta0_ 是初始角度，在收到目标那一刻存的）
            // double d_theta = goal_theta - now_turtle_pose_->theta;
            double lambda = (start_distance - d) / start_distance;// 当前走过的路程/总路程 0-1
            if (lambda < 0.0) lambda = 0.0;
            if (lambda > 1.0) lambda = 1.0;

            double theta_d = start_theta + lambda * (normalize_angle(goal_theta - start_theta));// 期望角度
            double e_theta = normalize_angle(theta_d - theta); // wrap 到 (-pi,pi]
            

            double kw = 2.0; // 自己调
            double w = kw * e_theta;
            
            //判断是否到达目标点
            if(d <= 0.05 && std::fabs(e_theta) <= 0.05){
                //停止运动
                v_msg.linear.x = 0.0;
                v_msg.linear.y = 0.0;
                v_msg.angular.z = 0.0;
                v_pub_->publish(v_msg);
                break;
            }
            //发布速度指令
            v_msg.linear.x = vxb;
            v_msg.linear.y = vyb;
            v_msg.angular.z = w;
            v_pub_->publish(v_msg);

            //生成连续反馈
            auto feedback = std::make_shared<Nav::Feedback>();
            feedback->distance = d;
            // //响应连续反馈
            goal_handle->publish_feedback(feedback);
            if( (this->now() - start_time).seconds() >= 2.0 ){
                RCLCPP_INFO(this->get_logger(),"距离目标点:%.2f,角度偏差:%.2f",feedback->distance,goal_theta - now_turtle_pose_->theta);
                start_time = this->now();
            }
            // rate.sleep();
        }
        
        //3.生成最终响应结果
        result->turtle_x = now_turtle_pose_->x;
        result->turtle_y = now_turtle_pose_->y;
        result->turtle_theta = now_turtle_pose_->theta;

        goal_handle->succeed(result);
        RCLCPP_INFO(this->get_logger(),"到达目标点,运动结束");
    }

    //方法二 只有x方向有速度,先朝目标点运动,到达目标点后再转动到指定朝向
    void handle_excute2(std::shared_ptr<rclcpp_action::ServerGoalHandle<Nav>> goal_handle){
        auto result = std::make_shared<Nav::Result>();
        auto feedback = std::make_shared<Nav::Feedback>();
        geometry_msgs::msg::Twist v_msg;

        //1.生成连续反馈
        rclcpp::Rate rate(1.0);
        while (true)
        {
            //如果要取消任务,那么需要特殊处理
            if (goal_handle->is_canceling())
            {
                //设置取消后的最终结果
                goal_handle->canceled(result);
                return;
            }
            
            //解析目标点坐标与原生乌龟坐标
            float goal_x = goal_handle->get_goal()->goal_x;
            float goal_y = goal_handle->get_goal()->goal_y;
            //计算剩余 距离并发布
            float distance_x = goal_x - temp_pose_->goal_x;
            float distance_y = goal_y - temp_pose_->goal_y;
            float distance = std::sqrt(std::pow(distance_x,2)+std::pow(distance_y,2));
            feedback->distance = distance;
            goal_handle->publish_feedback(feedback);

            //循环结束条件
            if (distance <= 0.05)
            {
                //停止乌龟运动
                v_msg.linear.x = 0.0;
                v_msg.linear.y = 0.0;
                v_pub_->publish(v_msg);
                RCLCPP_INFO(this->get_logger(),"到达目标点,停止运动");
                break;
            }

            //根据剩余距离计算速度指令并发布
            float scale = 0.5;
            v_msg.linear.x = scale * distance_x;
            v_msg.linear.y = scale * distance_y;
            v_pub_->publish(v_msg);

            
            rate.sleep();
        }
        //生成最终响应
        result->turtle_x = now_turtle_pose_->x;
        result->turtle_y = now_turtle_pose_->y;
        result->turtle_theta = now_turtle_pose_->theta;

        goal_handle->succeed(result);
        
        
        
    }
};


int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<ActionNode>("action_server_node"));

    //释放资源
    rclcpp::shutdown();
    return 0;
}

double normalize_angle(double a) {
        while (a > M_PI)  a -= 2.0 * M_PI;
        while (a <= -M_PI) a += 2.0 * M_PI;
        return a;
}