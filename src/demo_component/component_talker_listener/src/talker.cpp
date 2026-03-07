#include "rclcpp/rclcpp.hpp"
#include "rclcpp_components/register_node_macro.hpp"//注册组建的头文件
#include "std_msgs/msg/string.hpp"//消息类型的头文件

/*
    需求:编写一个发布方组件,周期性的发布消息
    流程:
        1.包含头文件(rclcpp_components/register_node_macro.hpp 调用注册宏)
        2.自定义组件类
            2-1设置namespace
            2-2构造函数设置rclcpp::NodeOptions参数
        3.注册组件
    
        ros2 component load /ComponentManager component_talker_listener my_componet::TalkerComponent 
*/
namespace my_componet{
using namespace std::chrono_literals; //使用时间命名空间
using namespace std::placeholders; //占位符命名空间

class TalkerComponent :public rclcpp::Node{
public:
    TalkerComponent(const rclcpp::NodeOptions & options):Node("TalkerComponent", options){
        RCLCPP_INFO(this->get_logger(),"talker组件创建成功,node: %s",this->get_name());
        //创建发布方对象
        pub_ = this->create_publisher<std_msgs::msg::String>("wtf",10);
        //创建定时器
        timer_ = this->create_wall_timer(500ms,std::bind(&TalkerComponent::callback_this,this));
        //创建消息对象
        msg_ = std::make_shared<std_msgs::msg::String>();
        msg_->data = "我cnm,发布次数: ";
    }

private:
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_;
    rclcpp::TimerBase::SharedPtr timer_;
    std::shared_ptr<std_msgs::msg::String> msg_;//消息对象
    int count_ = 0; //发布次数

    //回调函数
    void callback_this(){
        std_msgs::msg::String temp;
        temp.data = msg_->data + std::to_string(++count_);
        pub_->publish(temp);
        RCLCPP_INFO(this->get_logger(),"talker: %s", temp.data.c_str());
    }
};
}

//注册组件
RCLCPP_COMPONENTS_REGISTER_NODE(my_componet::TalkerComponent)