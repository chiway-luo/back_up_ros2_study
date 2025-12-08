#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"


/*
    需求:
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
using namespace std::chrono_literals;
class MyNode :public rclcpp::Node{
public:
    MyNode(std::string str1):Node(str1){
        RCLCPP_INFO(this->get_logger(),(str1+"节点创建成功").c_str());
        rclcpp::Rate rate(1s);
        //创建一个发布方对象
        pub_ = this->create_publisher<std_msgs::msg::String>("topic_sdy",10);
        rclcpp::sleep_for(3s);
        int count = 0;
        while(rclcpp::ok()){
            //创建消息对象
            auto msg = std_msgs::msg::String();
            msg.data = "hello_sdy " + std::to_string(count);
            RCLCPP_INFO(this->get_logger(),"发布的消息: %s",msg.data.c_str());
            //发布消息
            pub_->publish(msg);
            ++count;
            //设置频率
            rate.sleep();
        }
    }

private:
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_;
    //回调函数
    void callback_this(){
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<MyNode>("sdy"));

    //释放资源
    rclcpp::shutdown();
    return 0;
}