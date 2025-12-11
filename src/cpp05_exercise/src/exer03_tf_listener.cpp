#include "rclcpp/rclcpp.hpp"
#include "tf2_ros/transform_listener.hpp"
#include "tf2_ros/buffer.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"


/*
    需求:监听坐标变换广播数据,并生成turtle1相对于turtle2的坐标变换关系,
        进而生成turtle2的运动指令
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1声明参数服务
            3-2创建缓冲区对象
            3-3创建坐标系变换监听器与缓冲区对象关联
            3-4创建速度发布方
            3-5创建定时器,回调函数中实现坐标变换,生成速度指令并发布
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/
using namespace std::chrono_literals;
class TFListener :public rclcpp::Node{
public:
    TFListener(std::string str1,std::string str2):Node(str1,str2){
        RCLCPP_INFO(this->get_logger(),(str1+"节点创建成功").c_str());
        // 3-1声明参数服务
        this->declare_parameter("frame_id","turtle2");
        this->declare_parameter("child_frame_id","turtle1");
        frame_id_ = this->get_parameter("frame_id").as_string();
        child_frame_id_ = this->get_parameter("child_frame_id").as_string();
        // 3-2创建缓冲区对象
        tf_buffer_ = std::make_shared<tf2_ros::Buffer>(this->get_clock());
        // 3-3创建坐标系变换监听器与缓冲区对象关联
        tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);
        // 3-4创建速度发布方
        vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/" + frame_id_ + "/cmd_vel",10);
        // 3-5创建定时器,回调函数中实现坐标变换,生成速度指令并发布
        timer_ = this->create_wall_timer(
            100ms,
            std::bind(&TFListener::callback_this,this)
        );
    }

private:
    std::string frame_id_;//父坐标系
    std::string child_frame_id_;//子坐标系
    std::shared_ptr<tf2_ros::Buffer> tf_buffer_;//坐标系变换缓冲区对象指针
    std::shared_ptr<tf2_ros::TransformListener> tf_listener_;//坐标系变换监听器指针
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr vel_pub_;//速度消息发布器指针
    rclcpp::TimerBase::SharedPtr timer_;//定时器指针
    //回调函数
    void callback_this(){
        //实现坐标变换
        geometry_msgs::msg::TransformStamped transform_stamped;
        try
        {
            transform_stamped = tf_buffer_->lookupTransform(
            frame_id_,//父坐标系
            child_frame_id_,//子坐标系
            tf2::TimePointZero//时间戳(最新数据)
            );
            //组织并发布速度消息
            geometry_msgs::msg::Twist twist_msg;
            double d_x = transform_stamped.transform.translation.x;
            double d_y = transform_stamped.transform.translation.y;
            double distance = std::sqrt(d_x*d_x + d_y*d_y);

            double target_angle = std::atan2(d_y, d_x);
            double K_linear = 1.0;
            double K_angular = 1.0;

            twist_msg.linear.x = K_linear * distance;
            twist_msg.angular.z = K_angular * target_angle;

            if (distance < 0.1)
            {
                twist_msg.linear.x = 0.0;
                twist_msg.angular.z = 0.0;
            }
            
            vel_pub_->publish(twist_msg);
            
        }
        catch(const tf2::LookupException& e)
        {
            std::cerr << e.what() << '\n';
        }
        

    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<TFListener>("node_name","namespace"));

    //释放资源
    rclcpp::shutdown();
    return 0;
}