#include "rclcpp/rclcpp.hpp"
#include "tf2_ros/transform_broadcaster.hpp"
#include "turtlesim/msg/pose.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "tf2/LinearMath/Quaternion.hpp"


/*
    需求:启动turtlesim_node 节点 编写程序,发布乌龟(turtle1)相对于窗体(word)的位姿(运动的) 
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建动态坐标变换广播器
            3-2订阅乌龟位姿消息
            3-3回调函数中获取乌龟位姿并生成相对关系然后发布
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/

class TfDynamic :public rclcpp::Node{
public:
    TfDynamic(std::string str1,std::string str2):Node(str1,str2){
        RCLCPP_INFO(this->get_logger(),(str1+"节点创建成功").c_str());
        //创建动态坐标变换广播器
        broadcater_ = std::make_shared<tf2_ros::TransformBroadcaster>(this);
        //创建乌龟位姿订阅方
        sub_turtle_pose_ = this->create_subscription<turtlesim::msg::Pose>(
            "/turtle1/pose",
            10,
            std::bind(&TfDynamic::callback_this,this,std::placeholders::_1)
        );

    }

private:
    std::shared_ptr<tf2_ros::TransformBroadcaster> broadcater_;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr sub_turtle_pose_;
    //回调函数
    void callback_this(const turtlesim::msg::Pose::SharedPtr msg){
        //创建坐标变换消息
        geometry_msgs::msg::TransformStamped tfs;

        tfs.header.stamp = this->now();
        tfs.header.frame_id = "base_link";
        tfs.child_frame_id = "turtle1";

        tfs.transform.translation.x = msg->x;
        tfs.transform.translation.y = msg->y;
        tfs.transform.translation.z = 0.0;

        //四元数转换
        tf2::Quaternion qtn;
        qtn.setRPY(0,0,msg->theta);//roll pitch yaw
        tfs.transform.rotation.x = qtn.x();
        tfs.transform.rotation.y = qtn.y();
        tfs.transform.rotation.z = qtn.z();
        tfs.transform.rotation.w = qtn.w();

        //发布
        broadcater_->sendTransform(tfs);
        
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<TfDynamic>("tf_dynamic_broadcaster_node_cpp","namespace"));

    //释放资源
    rclcpp::shutdown();
    return 0;
}