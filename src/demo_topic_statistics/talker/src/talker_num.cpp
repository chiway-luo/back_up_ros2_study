#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"
/*
    ros2 topic echo /chatter
    需求：以某个固定频率发布文本“helloworld”，文本后缀编号，没法送一条信息，编号递增1
        1.包含头文件
        2.初始化ros2客户端
        3.定义节点类
            3-1.创建发布方
            3-2创建定时器
            3-3组织消息并发送
        4.调用spin函数，并传入节点指针
        5.释放资源
*/
using namespace std::chrono_literals;

class Talker_sdy:public rclcpp::Node{
    public:
        Talker_sdy(std::string str):Node(str){//构造函数
            //空实现 只实例化节点
            RCLCPP_INFO(this->get_logger(),(str+"节点创建成功").c_str());
        }
        void my_publish(){
            //创建发布方 也可以使用auto类型 返回值 发布对象指针
            publisher_ = this->create_publisher<std_msgs::msg::Int32>("chatter",10);//话题名称，通信质量
            //创建定时器 参数1 时间间隔 参数2 回调函数
            timer_ = this->create_wall_timer(1s,std::bind(&Talker_sdy::on_timer,this));
        }
    private:
        rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr publisher_;
        rclcpp::TimerBase::SharedPtr timer_;
        void on_timer(){
            //组织消息并发布
            static int count_=0;
            auto message = std_msgs::msg::Int32();
            message.data = count_++;
            RCLCPP_INFO(this->get_logger(),"发布消息:%d",message.data);
            publisher_->publish(message);
        }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数，使用自定义类对象指针  //创建对象并调用发布函数
    // rclcpp::spin(std::make_shared<Talker_sdy>("sdy_1"));
    auto node = std::make_shared<Talker_sdy>("sdy_1");
    node->my_publish();
    rclcpp::spin(node);


    //释放资源
    rclcpp::shutdown();

}