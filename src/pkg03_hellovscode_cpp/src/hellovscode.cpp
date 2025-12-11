#include "rclcpp/rclcpp.hpp"

int main(int argc, char *argv[])
{
	//初始化ros2客户端库
	rclcpp::init(argc, argv);
	//创建ros2节点指针
	auto node = rclcpp::Node::make_shared("hellovscode_cpp");
	//输出日志信息
	RCLCPP_INFO(node->get_logger(), "Hello VSCode C++ ROS2");
	//释放资源
	rclcpp::shutdown();
	return 0;
}


