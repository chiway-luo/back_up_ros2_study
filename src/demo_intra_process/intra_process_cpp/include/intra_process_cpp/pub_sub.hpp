#ifndef PUB_SUB_HPP
#define PUB_SUB_HPP
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include <unistd.h>

using namespace std::chrono_literals; //使用时间命名空间
using namespace std::placeholders; //占位符命名空间
/* 
    通信时节点默认通过dds实现,如果要实现高效的进程内通信,需要在构造函数中设置参数:
    rclcpp::NodeOptions().use_intra_process_comms(true)
    允许进程内通信
*/
class Pub:public rclcpp::Node{
public:
    Pub(std::string str1):Node(str1,rclcpp::NodeOptions().use_intra_process_comms(true)){
        RCLCPP_INFO(this->get_logger(),"namesapce:  node: %s 节点创建成功",str1.c_str());
        //创建发布方对象
        pub_ = this->create_publisher<std_msgs::msg::String>("progress_topic",10);
        //创建定时器对象
        timer_ = this->create_wall_timer(
            50ms,
            std::bind(&Pub::timer_cb,this)
        );
        //初始化msg对象
        msg_ = std::make_shared<std_msgs::msg::String>();
        msg_->data = "测试消息";
        

    }
private:
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_;//发布方对象
    rclcpp::TimerBase::SharedPtr timer_;//定时器对象
    std::shared_ptr<std_msgs::msg::String> msg_;//消息对象

    //定时器回调函数
    // void timer_cb(){
    //     std_msgs::msg::String temp;
    //     temp.data = msg_->data + "@" + std::to_string(this->now().nanoseconds());
    //     pub_->publish(temp);
    //     // RCLCPP_INFO(this->get_logger(),"发布方线程号: %d",std::this_thread::get_id());
    //     RCLCPP_INFO(this->get_logger(),"消息地址值: %p",&temp);
    // }

    //更高效的进程内通信(实现0成本的数据拷贝)内存共享
    void timer_cb(){
        auto temp = std::make_unique<std_msgs::msg::String>();//使用智能指针,避免内存泄漏
        temp->data = msg_->data + "@" + std::to_string(this->now().nanoseconds());

        // RCLCPP_INFO(this->get_logger(),"发布方线程号: %d",std::this_thread::get_id());
        RCLCPP_INFO(this->get_logger(),"消息地址值: %p",static_cast<const void *>(temp.get()));

        pub_->publish(std::move(temp));
    }
};


class Sub:public rclcpp::Node{
public:
    Sub(std::string str1):Node(str1,rclcpp::NodeOptions().use_intra_process_comms(true)){
        RCLCPP_INFO(this->get_logger(),"namesapce:  node: %s 节点创建成功",str1.c_str());
        //创建订阅方对象
        sub_ = this->create_subscription<std_msgs::msg::String>(
            "progress_topic",
            10,
            std::bind(&Sub::sub_cb,this,_1)
        );
    }
private:
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_;//订阅方对象
    //回调函数
    // void sub_cb(const std_msgs::msg::String::SharedPtr msg){
    //     //剪切消息,将时间转换为int,计算与当前时间的差值,输出
    //     std::string time_str = msg->data.substr(msg->data.find_last_of("@")+1);
    //     int64_t time = std::stoll(time_str);
    //     int64_t now_time = this->now().nanoseconds();
    //     long double diff = now_time - time;
    //     // RCLCPP_INFO(this->get_logger(),"接收到消息: %s, 时间差: %Lf , 当前进程号: %d",msg->data.c_str(),diff/1000000.0L,std::this_thread::get_id());
    //     RCLCPP_INFO(this->get_logger(),"接收到消息: %s, 时间差: %Lf , 当前地址: %p",msg->data.c_str(),diff/1000000.0L,msg.get());
    // }

    void sub_cb(const std_msgs::msg::String::UniquePtr msg){
        //剪切消息,将时间转换为int,计算与当前时间的差值,输出
        std::string time_str = msg->data.substr(msg->data.find_last_of("@")+1);
        int64_t time = std::stoll(time_str);
        int64_t now_time = this->now().nanoseconds();
        long double diff = now_time - time;
        // RCLCPP_INFO(this->get_logger(),"接收到消息: %s, 时间差: %Lf , 当前进程号: %d",msg->data.c_str(),diff/1000000.0L,std::this_thread::get_id());
        RCLCPP_INFO(this->get_logger(),"接收到消息: %s, 时间差: %Lf , 当前地址: %p",msg->data.c_str(),diff/1000000.0L,static_cast<const void *>(msg.get()));
    }
};



#endif  // PUB_SUB_HPP