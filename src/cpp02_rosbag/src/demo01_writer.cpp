#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "rosbag2_cpp/writer.hpp"
/*
    需求:
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建录制对象
            3-2设置磁盘文件
            3-3写出数据(创建数据订阅方,回调函数中执行写出数据)
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/

class SimpleBagRecorder :public rclcpp::Node{
public:
    SimpleBagRecorder(std::string str1,std::string str2):Node(str1,str2){
        RCLCPP_INFO(this->get_logger(),(str1+"消息录制节点创建成功").c_str());
        //创建录制对象
        writer_ = std::make_unique<rosbag2_cpp::Writer>();
        //设置磁盘文件
        writer_->open("src/cpp02_rosbag/bag/demo01_bag");//相对路径,是工作空间的直接子集
        //写出数据 void write(std::shared_ptr<rclcpp::SerializedMessage> message, const std::string &topic_name, const std::string &type_name, const rclcpp::Time &time)
        // writer_->write();
        sub_ = this->create_subscription<geometry_msgs::msg::Twist>(
            "/turtle1/cmd_vel",
            10,
            std::bind(&SimpleBagRecorder::callback_this,this,std::placeholders::_1)
        );
    }

private:
    std::unique_ptr<rosbag2_cpp::Writer> writer_; //创建录制对象
    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr sub_;
    //回调函数
    void callback_this(std::shared_ptr<rclcpp::SerializedMessage> message){
        //void write<MessageT>(const MessageT &message, const std::string &topic_name, const rclcpp::Time &time)
        /*
            void write(std::shared_ptr<rclcpp::SerializedMessage> message, 被写出的消息
            const std::string &topic_name, 话题名称
            const std::string &type_name, 消息类型 可以不写
            const rclcpp::Time &time) 时间戳
        */
        writer_->write(message,"/turtle1/cmd_vel","geometry_msgs/msg/Twist",this->now());
        // RCLCPP_INFO(this->get_logger(),"接收的消息: 线速度x: %f 角速度z: %f ",message->get_rcl_serialized_message().buffer[0],message->get_rcl_serialized_message().buffer[4]);
        RCLCPP_INFO(this->get_logger(),"接收消息");
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<SimpleBagRecorder>("sdy_1","sdy_rosbag_recorder"));

    //释放资源
    rclcpp::shutdown();
    return 0;
}