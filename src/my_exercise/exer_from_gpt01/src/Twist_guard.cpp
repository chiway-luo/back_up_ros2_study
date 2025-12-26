#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"


/*
    需求:机器人上游会发布/cmd_vel_in 话题 但是可能会突然给很大的速度指令,抖动或者停止不及时.需要把速度变得安全平滑,输出到/cmd_vel 话题
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1
            3-2
            3-3
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/

using namespace std::chrono_literals; //使用时间命名空间
using namespace std::placeholders; //占位符命名空间
std::mutex mutex_; //互斥锁

class TwistGuard :public rclcpp::Node{
public:
    TwistGuard(std::string str1,std::string str2):Node(str1,str2){
        //动态参数
        this->declare_parameter<double>("max_linear",0.5);
        this->declare_parameter<double>("max_angular",1.0);
        this->declare_parameter<double>("acc_linear",0.02);//线速度加速度限制(用于平滑)



        RCLCPP_INFO(this->get_logger(),"namesapce: %s node: %s 节点创建成功",str2.c_str(),str1.c_str());
        //创建速度订阅方
        sub_twist_ = this->create_subscription<geometry_msgs::msg::Twist>(
            "/cmd_vel_in",
            10,
            std::bind(&TwistGuard::sub_vel,this,_1)
        );
        //创建速度发布方
        pub_twist_ = this->create_publisher<geometry_msgs::msg::Twist>(
            "/cmd_vel",
            10
        );
    }
    

private:
    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr sub_twist_;
    geometry_msgs::msg::Twist::SharedPtr twist_msg;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_twist_;

    //订阅速度指令
    void sub_vel(const geometry_msgs::msg::Twist::SharedPtr msg){
        {
            //上锁
            std::lock_guard<std::mutex> lock(mutex_);
            twist_msg = msg;
        }
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<TwistGuard>("twistguard_node_cpp","my_car"));//node_name,namespace

    //释放资源
    rclcpp::shutdown();
    return 0;
}