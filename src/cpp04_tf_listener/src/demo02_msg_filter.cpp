#include "rclcpp/rclcpp.hpp"


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

class TFPOintListener :public rclcpp::Node{
public:
    TFPOintListener(std::string str1,std::string str2):Node(str1,str2){
        RCLCPP_INFO(this->get_logger(),(str1+"节点创建成功").c_str());

    }

private:

    //回调函数
    void callback_this(){
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<TFPOintListener>("node_name","namespace"));

    //释放资源
    rclcpp::shutdown();
    return 0;
}