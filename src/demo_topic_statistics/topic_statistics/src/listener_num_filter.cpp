/* 
    需求: 实现话题统计,比如消息之间的平均时间间隔,最大时间间隔,最小时间间隔等
    流程:
        1.实现基本的订阅功能
        2.在此基础上实现拓展统计功能
*/
#include <limits>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"
/*
    需求:实现话题过滤,只解析能被7整除的数据,并在终端输出
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建订阅方
            3-2实现回调函数
            3-3在回调中解析并输出数据
        4.调用spin函数,并传入节点对象指针
        5.释放资源


*/
using namespace std::chrono_literals;
class Listener_sdy :public rclcpp::Node{
public:
    Listener_sdy(std::string str1):Node(str1){
        RCLCPP_INFO(this->get_logger(),(str1+"节点创建成功").c_str());
        //创建一个subsriptionoptions对象
        auto option = rclcpp::SubscriptionOptions();
        //设置数据过滤规则
        // option.content_filter_options.filter_expression = "data > 0 AND data < 20";
        option.content_filter_options.filter_expression = "data > %0 AND data < %1";
        option.content_filter_options.expression_parameters = {"0", "20"};//动态设置过滤参数
        //创建订阅方
        sub_ = this->create_subscription<std_msgs::msg::Int32>(
            "chatter",10,
            std::bind(&Listener_sdy::callback_this,this,std::placeholders::_1),
            option
        );
    }

   
        

    
private:
    rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr sub_;

    //回调函数
    void callback_this(const std_msgs::msg::Int32 &msg){
        RCLCPP_INFO(this->get_logger(),"收到的消息: %d",static_cast<int32_t>(msg.data));
    }
};

int main(int argc, char const *argv[])
{
    //初始化ros客户端
    rclcpp::init(argc,argv);

    //调用spin函数,并传入节点对象指针
    // rclcpp::spin(std::make_shared<Listener_sdy>("sdy_listen1"));
    auto node = std::make_shared<Listener_sdy>("sdy_listen1");
    rclcpp::spin(node);

    //资源释放
    rclcpp::shutdown();
    return 0;

}
