#include "rclcpp/rclcpp.hpp"
#include "rclcpp_components/register_node_macro.hpp"//注册组建的头文件
#include "std_msgs/msg/string.hpp"//消息类型的头文件

/*
    需求:创建订阅方组件,并订阅发布方组件的消息
    流程:
        1.包含头文件
        3.自定义节点类
            3-1
            3-2

    ros2 component load /ComponentManager component_talker_listener my_componet::ListenerComponent 
*/
namespace my_componet{
using namespace std::chrono_literals; //使用时间命名空间
using namespace std::placeholders; //占位符命名空间

class ListenerComponent :public rclcpp::Node{
public:
    ListenerComponent(const rclcpp::NodeOptions & options):Node("listener_node", options){
        RCLCPP_INFO(this->get_logger(),"listener组件创建成功,node: %s",this->get_name());
        //创建订阅方
        sub_ = this->create_subscription<std_msgs::msg::String>("wtf",10,std::bind(&ListenerComponent::callback_this,this,_1));
    }

private:
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_;

    //回调函数
    void callback_this(const std_msgs::msg::String::SharedPtr msg){
        //打印接收到的数据
        RCLCPP_INFO(this->get_logger(),"listener: %s",msg->data.c_str());
    }
};
}
//注册组件
RCLCPP_COMPONENTS_REGISTER_NODE(my_componet::ListenerComponent)