// #include "rclcpp/rclcpp.hpp"
// #include <string>

// int main(int argc, char ** argv)
// {
//   //初始化ros2客户端
//   rclcpp::init(argc, argv);
//   //创建ros2节点对象对应的指针 节点指针
//   // rclcpp::Node::SharedPtr node = rclcpp::Node::make_shared("helloworld");
//   auto node = rclcpp::Node::make_shared("hellloworld_sdy");
//   //打印日志信息
//   for (size_t i = 0; i < 10; i++)
//   {
//     std::string str = "Hello World C++ ROS2 "+std::to_string(i);
//     RCLCPP_INFO(node->get_logger(), str.c_str());
//     std::this_thread::sleep_for(std::chrono::seconds(1));
//   }
//   //释放资源
//   rclcpp::shutdown();
//   return 0;
// }

#include "rclcpp/rclcpp.hpp"
#include <string>

class HelloWorldNode:public rclcpp::Node //继承
{
public:
	HelloWorldNode(const std::string & node_name):Node(node_name)
	{
		RCLCPP_INFO(this->get_logger(), "Hello World C++ ROS2");
	}

};
/*
    初始化和资源释放在程序中起到什么作用？

    初始化（rclcpp::init）用于设置ROS2客户端库的环境，
    确保节点能够正确地与ROS2通信系统进行交互。
    资源释放（rclcpp::shutdown）用于清理和释放在初始化过程中分配的资源，防止内存泄漏和其他潜在问题。

        1.前提：构建的程序可能由若干步骤或阶段组成；
          初始化-->节点对象-->日志输出-->数据的发布/数据的订阅/同时进行....-->节点/资源释放
        2.不同步骤或者不同阶段之间涉及到数据的传递和处理
        3.怎么实现数据的传递呢?
            使用context上下文对象 来实现数据的传递 这是一个容器,它可以存储数据也可以从中读取数据
        4.初始化其实就是要创建context上下文对象,资源释放就是销毁context上下文对象
*/

int main(int argc, char *argv[])
{
	//初始化ros2客户端库
	rclcpp::init(argc, argv);
	//创建ros2节点指针 通过类封装的节点可以实例化多个节点对象
	auto node = std::make_shared<HelloWorldNode>("helloworld_cpp");
	auto node2 = std::make_shared<HelloWorldNode>("helloworld_cpp2");
	//输出日志信息
	for (size_t i = 0; i < 10; i++)
	{
		std::string str = "hello world c++ ros2 " + std::to_string(i);
		RCLCPP_INFO(node->get_logger(), str.c_str());
		RCLCPP_INFO(node2->get_logger(), str.c_str());
        
	}
	//释放资源
	rclcpp::shutdown();
	return 0;
}
