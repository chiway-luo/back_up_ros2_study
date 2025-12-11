#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/pose.hpp"
#include "turtlesim/msg/pose.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "turtlesim/srv/spawn.hpp" //可选

/*
    需求:编写程序实现,程序运行后会启动turtlesim_node节点,再调用spawn服务创建第二只乌龟,编写一个节点,订阅
        第一只乌龟的位姿,让第二只乌龟跟随第一只乌龟运动
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建订阅者对象,订阅pose信息
            3-2创建订阅者对象.订阅第二只乌龟的pose信息
            3-3创建发布者对象和定时器对象,发布速度信息Twist
        4.调用spin函数,并传入节点对象指针
        5.释放资源

    ros2 run teleop_twist_keyboard teleop_twist_keyboard --ros-args --remap /cmd_vel:=/turtle1/cmd_vel

    可选 launch中生成小乌龟 或者创建服务客户端创建小乌龟
*/
void normalize_angle(double &angle);
using namespace std::chrono_literals;
class FollowTurtle :public rclcpp::Node{
public:
    FollowTurtle(std::string str1,std::string str2):Node(str1,str2){
        RCLCPP_INFO(this->get_logger(),(str1+"节点创建成功").c_str());
        //创建服务客户端对象并生成小乌龟 可选
        client_spawn_ = this->create_client<turtlesim::srv::Spawn>("/spawn");
        while(!client_spawn_->wait_for_service(1s));
        auto request = std::make_shared<turtlesim::srv::Spawn::Request>();
        request->x = 8.0;
        request->y = 9.0;
        request->theta = 3.14;
        request->name = "turtle2";
        auto result = client_spawn_->async_send_request(request);
        //创建发布者对象
        pub_twist_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle2/cmd_vel",10);
        //创建订阅者对象 订阅原乌龟的位姿
        sub_pose_ = this->create_subscription<turtlesim::msg::Pose>(
            "/turtle1/pose",10,
            std::bind(&FollowTurtle::sub_cb_1,this,std::placeholders::_1)
        );
        //创建订阅者对象 订阅第二只乌龟的位姿
        sub_pose_2 = this->create_subscription<turtlesim::msg::Pose>(
            "/turtle2/pose",10,
            std::bind(&FollowTurtle::sub_cb_2,this,std::placeholders::_1)
        );
        //创建定时器对象
        pub_timer_ = this->create_wall_timer(
            0.2s,
            std::bind(&FollowTurtle::pub_twist_timer,this)
        );

    }

private:
    rclcpp::Client<turtlesim::srv::Spawn>::SharedPtr client_spawn_;

    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr sub_pose_;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr sub_pose_2;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_twist_;
    rclcpp::TimerBase::SharedPtr pub_timer_;
    turtlesim::msg::Pose::SharedPtr msg_turtle1;
    turtlesim::msg::Pose::SharedPtr msg_turtle2;
    bool sub_flag1 = false;
    bool sub_flag2 = false;

    //回调函数
    void sub_cb_1(const turtlesim::msg::Pose::SharedPtr msg){        
        msg_turtle1 = msg;
        sub_flag1 = true;
    }
    void sub_cb_2(const turtlesim::msg::Pose::SharedPtr msg){
        msg_turtle2 = msg;
        sub_flag2 = true;
    }

    void pub_twist_timer(){
        if (!sub_flag1 || !sub_flag2)
        {
            return;
        }
        
        //创建速度消息
        geometry_msgs::msg::Twist twist_msg;
        //误差
        double d_x = msg_turtle1->x - msg_turtle2->x;
        double d_y = msg_turtle1->y - msg_turtle2->y;
        double distance = std::fabs(std::sqrt(d_x*d_x + d_y*d_y));

        double target_theta = std::atan2(d_y,d_x);
        double error_theta = target_theta - msg_turtle2->theta;

        // 归一化到 [-pi, pi]
        normalize_angle(error_theta);

        

        //设置线速度和角速度
        double v_x = 1.0 * distance;
        double w_z = 2.0 * error_theta;

        if (distance < 0.1) {
            v_x = 0.0;
            w_z = 0.0;
        }
        


        twist_msg.linear.x = v_x;
        twist_msg.linear.y = 0.0;
        twist_msg.linear.z = 0.0;

        twist_msg.angular.x = 0.0;
        twist_msg.angular.y = 0.0;
        twist_msg.angular.z = w_z;

        pub_twist_->publish(twist_msg);
        
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<FollowTurtle>("pub_follow","namespace_follow"));

    //释放资源
    rclcpp::shutdown();
    return 0;
}

void normalize_angle(double &angle)
{
    while (angle > M_PI)  angle -= 2 * M_PI;
    while (angle < -M_PI) angle += 2 * M_PI;
}
