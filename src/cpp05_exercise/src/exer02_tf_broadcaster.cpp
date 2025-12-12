#include "rclcpp/rclcpp.hpp"
#include "tf2_ros/transform_broadcaster.hpp"
#include "turtlesim/msg/pose.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "tf2/LinearMath/Quaternion.hpp"


/*
    需求:广播不同乌龟相对于base_link的坐标系相对关系
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

class TFBroadcaster :public rclcpp::Node{
public:
    TFBroadcaster(std::string str1,std::string str2):Node(str1,str2){
        this->declare_parameter("turtle","turtle1");
        RCLCPP_INFO(this->get_logger(),(str1+"节点创建成功").c_str());
        //创建动态坐标系广播器
        tf_broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>(this);
        //订阅乌龟位姿信息
        pose_sub_ = this->create_subscription<turtlesim::msg::Pose>(
            "/" + this->get_parameter("turtle").as_string()+"/pose",10,
            std::bind(&TFBroadcaster::sub_pose,this,std::placeholders::_1)
        );
        //在回调函数中获取乌龟位姿并生成相对关系然后发布
    }

private:
    std::shared_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;//动态坐标系变换广播器指针
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr pose_sub_;//乌龟位姿消息订阅器指针
    //回调函数
    void sub_pose(const turtlesim::msg::Pose::SharedPtr msg){
        //创建坐标系变换消息
        geometry_msgs::msg::TransformStamped tfs;
        tfs.header.stamp = this->get_clock()->now();//时间戳
        tfs.header.frame_id = "base_link";//父坐标系
        tfs.child_frame_id = this->get_parameter("turtle").as_string();//子坐标系
        //坐标系相对关系
        tfs.transform.translation.x = msg->x;
        tfs.transform.translation.y = msg->y;
        tfs.transform.translation.z = 0.0;
        //四元数计算
        tf2::Quaternion qtn;
        qtn.setRPY(0,0,msg->theta);
        tfs.transform.rotation.x = qtn.x();
        tfs.transform.rotation.y = qtn.y();
        tfs.transform.rotation.z = qtn.z();
        tfs.transform.rotation.w = qtn.w();
        //发布相对关系
        tf_broadcaster_->sendTransform(tfs);
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<TFBroadcaster>("node_name","namespace"));

    //释放资源
    rclcpp::shutdown();
    return 0;
}