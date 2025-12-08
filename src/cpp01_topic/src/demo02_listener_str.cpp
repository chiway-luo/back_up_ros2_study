#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
/*
    需求:订阅发布方发布的消息,并在终端输出
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
class Listener_sdy :public rclcpp::Node{
    public:
        Listener_sdy(std::string str1):Node(str1){
            RCLCPP_INFO(this->get_logger(),(str1+"节点创建成功").c_str());
        }
        void my_listener(){
            //创建订阅方
            sub_ = this->create_subscription<std_msgs::msg::String>(
                "chatter",10,
                std::bind(&Listener_sdy::callback_this,this,std::placeholders::_1)
            );
            
        }
    private:
        rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_;
        //回调函数
        void callback_this(const std_msgs::msg::String &msg){
            RCLCPP_INFO(this->get_logger(),"订阅到的数据:%s",msg.data.c_str());
        }
};

int main(int argc, char const *argv[])
{
    //初始化ros客户端
    rclcpp::init(argc,argv);

    //调用spin函数,并传入节点对象指针
    // rclcpp::spin(std::make_shared<Listener_sdy>("sdy_listen1"));
    auto node = std::make_shared<Listener_sdy>("sdy_listen1");
    node->my_listener();
    rclcpp::spin(node);

    //资源释放
    rclcpp::shutdown();
    return 0;

}

