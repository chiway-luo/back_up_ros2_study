#include "std_msgs/msg/string.hpp"
#include "rclcpp/rclcpp.hpp"
/* 
    需求: 将发布方和订阅方放在同一进程内,实现发布方与订阅方
    之间的通信,并且不使用ROS2的通信机制,而是直接在内存中进行数据传递

    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.创建发布方和订阅方
        4.核心:将发布方和订阅方集成到统一进程之内
        5.释放资源
*/
#include "intra_process_cpp/pub_sub.hpp"

using namespace std::chrono_literals; //使用时间命名空间
using namespace std::placeholders; //占位符命名空间


int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);
    //核心:将发布方对象和订阅方对象集成到同一进程

    //创建一个进程执行器
    // rclcpp::executors::SingleThreadedExecutor executor;//有利于资源优化 单线程执行器,当执行的时候再创建
    // rclcpp::executors::StaticSingleThreadedExecutor  executor;//更高效 单线程执行器,当创建的时候就已经初始化了,不需要调用spin函数,直接调用spin_some函数即可
    rclcpp::executors::MultiThreadedExecutor executor;//多线程执行器(并行),当执行的时候再创建


    //创建节点对象并添加进进程执行器
    auto pub_node = std::make_shared<Pub>("miniPub_node_cpp");
    auto sub_node = std::make_shared<Sub>("miniSub_node_cpp");
    executor.add_node(pub_node);
    executor.add_node(sub_node);
    //执行
    executor.spin();
    // executor.spin_some(1000ms);//执行100ms,如果有消息就执行,没有消息就直接返回,不阻塞
    //调用spin函数,使用自定义类对象指针
    // rclcpp::spin(std::make_shared<Pub>("pub_node_cpp"));//node_name, (namespace可选)
    //释放资源
    rclcpp::shutdown();
    return 0;
}