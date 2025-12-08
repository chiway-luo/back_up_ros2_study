#include "rclcpp/rclcpp.hpp"
#include "turtlesim/msg/pose.hpp"
#include "geometry_msgs/msg/twist.hpp"


/*
    需求:创建发布方发送速度指令给launch文件中创建的乌龟,创建订阅方订阅原生乌龟的节点,并在回调函数中解析数据并发布速度指令
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建
            3-2实现回调函数
            3-3在回调中解析并输出数据
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/
double normalize_angle(double a) {
        while (a > M_PI)  a -= 2.0 * M_PI;
        while (a <= -M_PI) a += 2.0 * M_PI;
        return a;
}

using std::placeholders::_1;
class MyNode :public rclcpp::Node{
public:
    MyNode(std::string str1):Node(str1){
        RCLCPP_INFO(this->get_logger(),(str1+"节点创建成功").c_str());
        //创建订阅者对象
        sub_ = this->create_subscription<turtlesim::msg::Pose>(
            "/turtle1/pose",
            10,
            std::bind(&MyNode::sub_pose,this,_1)
        );
        //创建订阅者对象 订阅新乌龟当前位姿
        sub_now_turtle_ = this->create_subscription<turtlesim::msg::Pose>(
            "/sdy_turtle/pose",
            10,
            std::bind(&MyNode::sub_now_turtle_pose,this,_1)
        );

        //创建发布方对象 给新乌龟发送速度指令
        pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/sdy_turtle/cmd_vel",10);

    }

private:
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr sub_;

    turtlesim::msg::Pose::SharedPtr goal_pose_;//临时变量传递pose信息
    turtlesim::msg::Pose::SharedPtr now_turtle_pose_;//新乌龟当前位姿

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_;

    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr sub_now_turtle_;

    bool has_goal_ = false;
    bool has_now_  = false;


    //回调函数
    void sub_pose(const turtlesim::msg::Pose::SharedPtr msg){
        //订阅数据
        // RCLCPP_INFO(this->get_logger(),"收到乌龟位姿信息");
        goal_pose_ = msg;
        has_goal_ = true;
        send_vel();
    }

    void sub_now_turtle_pose(const turtlesim::msg::Pose::SharedPtr msg){
        //订阅数据
        // RCLCPP_INFO(this->get_logger(),"收到新乌龟位姿信息");
        now_turtle_pose_ = msg;
        has_now_ = true;
        //发送速度指令
        send_vel();
    }

    //发送速度指令函数
    void send_vel(){
        
        // 两边都还没收到就不控制
        if (!has_goal_ || !has_now_) return;

        geometry_msgs::msg::Twist v_msg;

        double goal_x = goal_pose_->x;
        double goal_y = goal_pose_->y;
        

        double now_x = now_turtle_pose_->x;
        double now_y = now_turtle_pose_->y;
        double theta = now_turtle_pose_->theta;
        //计算距离
        double d_x = goal_x - now_x;
        double d_y = goal_y - now_y;
        double d = std::sqrt(d_x * d_x + d_y * d_y);

        //计算角度差
        double goal_theta = std::atan2(d_y, d_x);
        double e_theta = normalize_angle(goal_theta - theta);

        // --- 控制系数，后面可以慢慢调 ---
        double k_d     = 0.5; // 距离 → 线速度
        double k_theta = 2.0; // 角度误差 → 角速度

        //设置速度 只x方向运动
        double v_x = k_d * d;
        double w  = k_theta * e_theta;

        v_msg.linear.x  = v_x;
        v_msg.angular.z = w;
        
        // 接近目标时停下（防止抖动）
        if (d < 0.01 && std::fabs(e_theta) < 0.01) {
            v_msg.linear.x  = 0.0;
            v_msg.angular.z = 0.0;
        }

        pub_->publish(v_msg);
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<MyNode>("sdy_1"));

    //释放资源
    rclcpp::shutdown();
    return 0;
}