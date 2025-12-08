#include "rclcpp/rclcpp.hpp"
#include "tf2_ros/static_transform_broadcaster.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "tf2/LinearMath/Quaternion.hpp"//欧拉角转换为四元数头文件


/*
    需求:编写静态坐标变换程序,执行时传入两个坐标系的相对位姿关系以及父子级坐标系id,
        程序运行发布静态坐标变换  ros2 run 包 可执行程序  x y z roll pitch yaw frame child_frame
        ros2 run cpp03_tf_broadcaster demo01_static_tf_broadcaster 0.4 0 0.2 0 0 0 base_link laser
        ros2 run cpp03_tf_broadcaster demo01_static_tf_broadcaster -0.5 0 0.4 0 0 0 base_link camera
    流程:
        1.包含头文件
        2.判断传入参数是否合法
        3.初始化ros2客户端
        4.自定义节点类
            3-1创建广播对象
            3-2组织并发布数据
        5.调用spin函数,并传入节点对象指针
        6.释放资源
*/

class TfBroadcaster :public rclcpp::Node{
public:
    TfBroadcaster(std::string str1,std::string str2,char * transform[]):Node(str1,str2){
        RCLCPP_INFO(this->get_logger(),(str1+"节点创建成功").c_str());
        //创建广播对象
        broadcaster_ = std::make_shared<tf2_ros::StaticTransformBroadcaster>(this);
        //组织并发布数据 最好封装一个函数
        pub_static_tf(transform);

    }

private:
    std::shared_ptr<tf2_ros::StaticTransformBroadcaster> broadcaster_;
    //回调函数
    void pub_static_tf(char * transform[]){
        //组织消息
        geometry_msgs::msg::TransformStamped tf_msg;
        tf_msg.header.stamp = this->now();//时间戳

        tf_msg.child_frame_id = transform[8];//子坐标系
        tf_msg.header.frame_id = transform[7];//父坐标系

        tf_msg.transform.translation.x = atof(transform[1]);//x偏移量
        tf_msg.transform.translation.y = atof(transform[2]);//y偏移量
        tf_msg.transform.translation.z = atof(transform[3]);//z偏移量

        //设置四元数  先将欧拉角转换为四元数
        tf2::Quaternion qtn;
        qtn.setRPY(atof(transform[4]),atof(transform[5]),atof(transform[6]));//roll pitch yaw

        tf_msg.transform.rotation.x = qtn.x();
        tf_msg.transform.rotation.y = qtn.y();
        tf_msg.transform.rotation.z = qtn.z();
        tf_msg.transform.rotation.w = qtn.w();

        //发布
        broadcaster_->sendTransform(tf_msg);
    }
};

int main(int argc, char * argv[])
{
    //判断参数是否合法
    if (argc != 9){
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"参数不合法");
        return 1;
    }
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<TfBroadcaster>("tf_static_broadcaster_node_cpp","namespace",argv));

    //释放资源
    rclcpp::shutdown();
    return 0;
}