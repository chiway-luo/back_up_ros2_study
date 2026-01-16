#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/imu.hpp"
#include "tf2_ros/transform_broadcaster.h"
#include "geometry_msgs/msg/transform_stamped.hpp"

/*
    需求:订阅imu消息,提取旋转角度数据,生成坐标变换(机器人mycar相对于地面world投影)消息发布
    
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建订阅方对象
            3-2创建坐标变换广播器
            3-3
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/

using namespace std::chrono_literals; //使用时间命名空间
using namespace std::placeholders; //占位符命名空间

class Imu2Tf :public rclcpp::Node{
public:
    Imu2Tf(std::string str1):Node(str1){
        RCLCPP_INFO(this->get_logger(),"namesapce:  node: %s 节点创建成功",str1.c_str());
        //创建坐标变换广播器
        tf_bro_ = std::make_shared<tf2_ros::TransformBroadcaster>(*this);
        //创建订阅方对象
        imu_sub_ = this->create_subscription<sensor_msgs::msg::Imu>("/imu",10,std::bind(&Imu2Tf::imu_cb,this,_1));
        //创建imu消息指针 没用到
        imu_msg_ = std::make_shared<sensor_msgs::msg::Imu>();
        //创建坐标变换消息指针
        tf_msg_ = std::make_shared<geometry_msgs::msg::TransformStamped>();

    }

private:
    std::shared_ptr<tf2_ros::TransformBroadcaster> tf_bro_;//坐标变换广播器指针
    rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr imu_sub_;//订阅方对象
    std::mutex imu_mutex_;//互斥锁 imu 消息
    std::shared_ptr<sensor_msgs::msg::Imu> imu_msg_;//imu消息指针
    std::shared_ptr<geometry_msgs::msg::TransformStamped> tf_msg_;//坐标变换消息指针

    //回调函数
    void imu_cb(const sensor_msgs::msg::Imu::SharedPtr msg){
        //在回调函数中没有必要加锁,因为回调函数本身就是单线程执行的
        
        //转换坐标变换消息
        tf_msg_->header.stamp = msg->header.stamp;
        tf_msg_->header.frame_id = "world";//父坐标系
        tf_msg_->child_frame_id = "mycar";//子坐标系

        //平移数据全部置0
        tf_msg_->transform.translation.x = 0.0;
        tf_msg_->transform.translation.y = 0.0;
        tf_msg_->transform.translation.z = 1.0;

        //直接使用imu消息中的四元数数据
        tf_msg_->transform.rotation = msg->orientation;

        //广播坐标变换
        tf_bro_->sendTransform(*tf_msg_);

    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<Imu2Tf>("Imu2Tf_node_cpp"));//node_name, (namespace可选)

    //释放资源
    rclcpp::shutdown();
    return 0;
}