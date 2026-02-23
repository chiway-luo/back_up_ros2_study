#include "rclcpp/rclcpp.hpp"
#include "tf2_ros/static_transform_broadcaster.h"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "tf2/LinearMath/Quaternion.h"
#include "geometry_msgs/msg/pose_stamped.hpp"

#include "tf2_ros/transform_listener.hpp"//坐标变换监听
#include "tf2_ros/buffer.hpp"//坐标变换缓存


/*
    需求:实现编队逻辑
    接收静态坐标变化的目标点,发布导航请求
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1发布目标坐标系(静态坐标变换)也可以在launch中使用命令行工具发布坐标变换
            3-2将目标坐标系转换成相对于当前从车的导航的目标点
            3-3定时发布导航请求 /robot_2/goal_pose geometry_msgs/msg/PoseStamped 导航请求的消息类型
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/

using namespace std::chrono_literals; //使用时间命名空间
using namespace std::placeholders; //占位符命名空间

class Convoy :public rclcpp::Node{
public:
    Convoy(std::string str1):Node(str1){
        RCLCPP_INFO(this->get_logger(),"namesapce:  node: %s 节点创建成功",str1.c_str());

        //参数设置
        this->declare_parameter("car_num","robot_1");
        this->declare_parameter("main_car_frame","robot_0/base_link");
        this->declare_parameter("goal_frame","robot_1/goal");
        // this->declare_parameter("follow_car_frame","robot_1/base_link");
        this->declare_parameter("x_offset",1.0);
        this->declare_parameter("y_offset",0.0);
        this->declare_parameter("yaw_offset",0.0);//单位为弧度

        //获取参数
        this->get_parameter("car_num",car_num);
        this->get_parameter("main_car_frame",main_car_frame);
        this->get_parameter("goal_frame",goal_frame);
        // this->get_parameter("follow_car_frame",follow_car_frame);
        this->get_parameter("x_offset",x_offset);
        this->get_parameter("y_offset",y_offset);
        this->get_parameter("yaw_offset",yaw_offset);
        

        //静态坐标变换广播器
        static_broadcaster_ = std::make_shared<tf2_ros::StaticTransformBroadcaster>(this);

        this->bro_tf();//发布坐标变换

        //创建坐标变换监听器
        tf_buffer_ = std::make_shared<tf2_ros::Buffer>(this->get_clock());
        tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);
        //创建坐标变换对象
        transformStamped = std::make_shared<geometry_msgs::msg::TransformStamped>();

        //创建消息发布器,发布导航请求
        goal_pub_ = this->create_publisher<geometry_msgs::msg::PoseStamped>("/" + car_num + "/goal_pose",10);
        //创建消息对象,减少内存占用
        goal_msg_ = std::make_shared<geometry_msgs::msg::PoseStamped>();

        //创建定时器,周期为1s
        timer_ = this->create_wall_timer(1s,std::bind(&Convoy::pub_goal,this));

    }

private:
    std::shared_ptr<tf2_ros::StaticTransformBroadcaster> static_broadcaster_;//发布静态坐标变换
    std::string car_num;//车的编号
    std::string main_car_frame;//主车坐标系
    std::string goal_frame;//目标坐标系
    std::string follow_car_frame;//从车坐标系
    double x_offset;//x轴偏移
    double y_offset;//y轴偏移
    double yaw_offset;//yaw偏移
    std::shared_ptr<rclcpp::TimerBase> timer_;//定时器
    std::shared_ptr<rclcpp::Publisher<geometry_msgs::msg::PoseStamped>> goal_pub_;//导航请求发布器
    std::shared_ptr<geometry_msgs::msg::PoseStamped> goal_msg_;//导航请求消息对象

    std::shared_ptr<tf2_ros::Buffer> tf_buffer_;//坐标变换缓存
    std::shared_ptr<tf2_ros::TransformListener> tf_listener_;//坐标变换监听器

    std::shared_ptr<geometry_msgs::msg::TransformStamped> transformStamped;//坐标变换对象

    //根据传入的参数发布坐标变换(目标坐标系相对于主车坐标系的变换)
    void bro_tf(){
        //填充数据并发布
        geometry_msgs::msg::TransformStamped tfs;
        tfs.header.stamp = this->get_clock()->now();
        tfs.header.frame_id = this->main_car_frame;
        tfs.child_frame_id = this->goal_frame;
        tfs.transform.translation.x = this->x_offset;
        tfs.transform.translation.y = this->y_offset;
        tfs.transform.translation.z = 0.0;
        //四元数转换
        tf2::Quaternion q;
        q.setRPY(0, 0, this->yaw_offset);
        tfs.transform.rotation.x = q.x();
        tfs.transform.rotation.y = q.y();
        tfs.transform.rotation.z = q.z();
        tfs.transform.rotation.w = q.w();
        static_broadcaster_->sendTransform(tfs);
    
    }

    //发布导航请求的回调函数
    void pub_goal(){
        //获取目标坐标系相对于从车坐标系的变换
        try
        {
            *this->transformStamped = tf_buffer_->lookupTransform("map", this->goal_frame, tf2::TimePointZero);
        }
        catch(const std::exception& e)
        {
            RCLCPP_ERROR(this->get_logger(), "坐标变换获取失败: %s", e.what());
            return;
        }
        goal_msg_->header.stamp = this->get_clock()->now();
        goal_msg_->header.frame_id = "map";
        goal_msg_->pose.position.x = this->transformStamped->transform.translation.x;
        goal_msg_->pose.position.y = this->transformStamped->transform.translation.y;
        goal_msg_->pose.position.z = this->transformStamped->transform.translation.z;
        goal_msg_->pose.orientation.x = this->transformStamped->transform.rotation.x;
        goal_msg_->pose.orientation.y = this->transformStamped->transform.rotation.y;
        goal_msg_->pose.orientation.z = this->transformStamped->transform.rotation.z;
        goal_msg_->pose.orientation.w = this->transformStamped->transform.rotation.w;

        goal_pub_->publish(*goal_msg_);
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<Convoy>("convoy_node_cpp"));//node_name, (namespace可选)

    //释放资源
    rclcpp::shutdown();
    return 0;
}