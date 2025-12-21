#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"


/*
    需求: 编写发布方实现,发布速度指令控制机器人做圆周运动;
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建消息发布方,发布速度指令
            3-2创建定时器,周期性的发布速度指令
            3-3在定时器的回调函数中,组织消息数据并发布
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/
using namespace std::chrono_literals; //使用时间命名空间
class PubVel :public rclcpp::Node{
public:
    PubVel(std::string str1,std::string str2):Node(str1,str2){
        //动态参数
        this->declare_parameter<double>("linear_x", 0.0);
        this->declare_parameter<double>("angular_z", 0.0);

        RCLCPP_INFO(this->get_logger(),(str1+"节点创建成功").c_str());
        //创建消息发布方
        pub_vel_ = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel",10);
        //创建定时器,周期性的发布速度指令
        timer_ = this->create_wall_timer(
            500ms,
            std::bind(&PubVel::timer_callback,this)
        );
    }

private:
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_vel_;
    rclcpp::TimerBase::SharedPtr timer_;

    
    void timer_callback(){
        //组织消息数据并发布
        geometry_msgs::msg::Twist vel_msg;
        vel_msg.linear.x = this->get_parameter("linear_x").as_double();
        vel_msg.angular.z = this->get_parameter("angular_z").as_double();
        pub_vel_->publish(vel_msg);
        RCLCPP_INFO(this->get_logger(),"发布速度指令:线速度:%.2f,角速度:%.2f",vel_msg.linear.x,vel_msg.angular.z);
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<PubVel>("pub_vel","my_car"));

    //释放资源
    rclcpp::shutdown();
    return 0;
}