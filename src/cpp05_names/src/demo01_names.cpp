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

class MyNOde :public rclcpp::Node{
public:
    MyNOde(std::string str1):Node(str1,"sdy_ns"){
        RCLCPP_INFO(this->get_logger(),(str1+"节点创建成功").c_str());
        //创建发布方对象
        //全局话题 和节点名称和命名空间无关系 /sdy_talk
        // pub_ = rclcpp::create_publisher<std_msgs::msg::String>(this,"/sdy_talk",10);
        //相对话题 /sdy_ns/sdy_talk
        // pub_ = rclcpp::create_publisher<std_msgs::msg::String>(this,"sdy_talk",10);
        //私有话题 /sdy_ns/sdy_name_node/sdy_talk
        pub_ = rclcpp::create_publisher<std_msgs::msg::String>(this,"~/sdy_talk",10);

        RCLCPP_INFO(this->get_logger(),"发布方创建成功: %s",pub_->get_topic_name());
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
    rclcpp::spin(std::make_shared<MyNOde>("sdy_name_node"));

    //释放资源
    rclcpp::shutdown();
    return 0;
}