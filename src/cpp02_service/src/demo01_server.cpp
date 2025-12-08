#include "rclcpp/rclcpp.hpp"
#include "base_interfaces_demo/srv/add_ints.hpp"

/*
    ros2 interface show base_interfaces_demo/srv/AddInts 显示接口信息

    ros2 service call /sdy_add_ints base_interfaces_demo/srv/AddInts "{'num1': 10,'num2': 32}"

    需求: 编写服务端实现，解析提交的请求数据，将解析的数据相加并相应到客户端
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建服务端
            3-2实现回调函数
            3-3在回调中解析并输出数据
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/
using base_interfaces_demo::srv::AddInts;
using std::placeholders::_1;
using std::placeholders::_2;

class Server_node :public rclcpp::Node{
public:
    Server_node(std::string str1):Node(str1){
        RCLCPP_INFO(this->get_logger(),(str1+"节点创建成功").c_str());
        //创建服务端对象
        server_ = this->create_service<AddInts>(
            "sdy_add_ints",
            std::bind(&Server_node::callback_this,this,_1,_2)
        );
    }

private:
    rclcpp::Service<AddInts>::SharedPtr server_;
    //回调函数
    void callback_this(const AddInts::Request::SharedPtr request,AddInts::Response::SharedPtr response){
        RCLCPP_INFO(this->get_logger(),
            "收到请求: num1=%d,num2=%d",
            request->num1,
            request->num2
        );
        //创建响应对象
        response->sum = request->num1 + request->num2;
        RCLCPP_INFO(this->get_logger(),
            "响应结果: sum=%d",
            response->sum
        );
    
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<Server_node>("server_node_cpp"));

    //释放资源
    rclcpp::shutdown();
    return 0;
}