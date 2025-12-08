#include "rclcpp/rclcpp.hpp"
#include "rosbag2_cpp/reader.hpp"
#include "geometry_msgs/msg/twist.hpp"

/*
    需求:读取获取的bag文件数据并输出在终端
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建读取对象(回放对象)
            3-2设置被读取的文件
            3-3读取消息
            3-4关闭文件
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/

class SimpleBagPlayer :public rclcpp::Node{
public:
    SimpleBagPlayer(std::string str1):Node(str1){
        RCLCPP_INFO(this->get_logger(),(str1+"消息回放节点创建成功").c_str());
        //创建读取对象
        reader_ = std::make_unique<rosbag2_cpp::Reader>();
        //设置被读取的文件
        reader_->open("src/cpp02_rosbag/bag/demo01_bag");
        //反序列化工具
        rclcpp::Serialization<geometry_msgs::msg::Twist> serializer;
        //读取消息
        int count = 0;
        while(reader_->has_next()){
            auto bag_message = reader_->read_next();//自动类型 rosbag2_storage::SerializedBagMessage 带有时间戳
            //从消息中提取时间戳
            rclcpp::Time stamp(bag_message->time_stamp);
            //反序列化
            rclcpp::SerializedMessage serialized_msg(*bag_message->serialized_data);
            geometry_msgs::msg::Twist twist_msg;
            serializer.deserialize_message(&serialized_msg, &twist_msg);
            // auto twist = reader_->read_next<geometry_msgs::msg::Twist>();
            //每8条打印一条
            if (count % 8 == 0)
            {
                RCLCPP_INFO(this->get_logger(),"time: %.2f v_x: %.2f v_y: %.2f w_z: %.2f",
                stamp.seconds(),twist_msg.linear.x,twist_msg.linear.y,twist_msg.angular.z);
            }
            ++count;
            
        }
        //关闭文件
        reader_->close();
        return;
    }

private:
    std::unique_ptr<rosbag2_cpp::Reader> reader_;//读取对象指针
    //回调函数
    void callback_this(){
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<SimpleBagPlayer>("sdy_1"));

    //释放资源
    rclcpp::shutdown();
    return 0;
}