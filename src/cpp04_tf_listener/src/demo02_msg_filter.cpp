#include "rclcpp/rclcpp.hpp"
#include "tf2_ros/buffer.hpp" //缓冲区头文件
#include "tf2_ros/transform_listener.hpp" //监听器头文件
#include "tf2_ros/create_timer_ros.h" //定时器头文件
#include "message_filters/subscriber.h" //订阅方头文件
#include "geometry_msgs/msg/point_stamped.hpp" //坐标点消息
#include "tf2_ros/message_filter.hpp" //过滤器头文件

#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp" //坐标点转换头文件


/*
    需求:广播laser 到 base_link的坐标系相对关系
        然后发布point->laser的坐标
        求解point->base_link的坐标
    自定义增加功能: 将消息过滤器的坐标点偏移z+3.0 发布动态点坐标
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建坐标变换监听器
            3-2创建坐标点消息订阅方
            3-3创建过滤器,解析数据
        4.调用spin函数,并传入节点对象指针
        5.释放资源

    使用编写的launch文件启动这个案例 demo01_test_launch.py
*/
using namespace std::chrono_literals;
class TFPOintListener :public rclcpp::Node{
public:
    TFPOintListener(std::string str1,std::string str2):Node(str1,str2){
        //创建坐标动态发布节点
        point_pub_ = this->create_publisher<geometry_msgs::msg::PointStamped>(
            "point",
            10
        );
        //本函数以下为课程内容,以上为增加内容可删去
        RCLCPP_INFO(this->get_logger(),(str1+"节点创建成功").c_str());
        //创建坐标变换监听器
        tf_buffer_ = std::make_shared<tf2_ros::Buffer>(this->get_clock());
        /*
            rclcpp::node_interfaces::NodeBaseInterface::SharedPtr node_base,
            rclcpp::node_interfaces::NodeTimersInterface::SharedPtr node_timers, 
            rclcpp::CallbackGroup::SharedPtr callback_group = nullptr
        */
        timer_ = std::make_shared<tf2_ros::CreateTimerROS>(
            this->get_node_base_interface(),
            this->get_node_timers_interface()
        );

        tf_buffer_->setCreateTimerInterface(timer_);
        tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);
        //创建坐标点订阅方
        point_sub_.subscribe(this,"/active_point/point");
        //创建过滤器,解析数据
        point_filter_ = std::make_shared<tf2_ros::MessageFilter<geometry_msgs::msg::PointStamped>>(
            /*
                F & f, 订阅对象
                BufferT & buffer, 监听缓存区
                const std::string & target_frame, base_link目标坐标系
                uint32_t queue_size, 队列长度10
                const rclcpp::node_interfaces::NodeLoggingInterface::SharedPtr & node_logging, 日志接口
                const rclcpp::node_interfaces::NodeClockInterface::SharedPtr & node_clock, 时钟接口
                std::chrono::duration<TimeRepT, TimeT> buffer_timeout = 超时时间 在指定时间内没有得到坐标系变换数据则报错丢弃
                std::chrono::duration<TimeRepT, TimeT>::max()
            */
           point_sub_,
           *tf_buffer_,
           "base_link",
           10,
           this->get_node_logging_interface(),
           this->get_node_clock_interface(),
           1s
        );
        //解析数据
        point_filter_->registerCallback(&TFPOintListener::transform_point,this);
        
    }

private:
    std::shared_ptr<tf2_ros::Buffer> tf_buffer_;//缓冲区对象
    std::shared_ptr<tf2_ros::TransformListener> tf_listener_;//监听器对象
    std::shared_ptr<tf2_ros::CreateTimerROS> timer_;//定时器对象
    message_filters::Subscriber<geometry_msgs::msg::PointStamped> point_sub_;//坐标点订阅方 使用指针在过滤器中会抛出异常
    std::shared_ptr<tf2_ros::MessageFilter<geometry_msgs::msg::PointStamped>> point_filter_;//过滤器对象 包含过滤模板类型

    rclcpp::Publisher<geometry_msgs::msg::PointStamped>::SharedPtr point_pub_;//坐标点发布方

    //回调函数
    void transform_point(const geometry_msgs::msg::PointStamped::SharedPtr &point_msg){
        //实现坐标点的变换
        //T transform<T>(const T &in, const std::string &target_frame, tf2::Duration timeout = tf2::durationFromSec((0.0))) const
        //使用该函数必须包含头文件 #include "tf2_geometry_msgs/tf2_geometry_msgs.hpp" 不然编译报错
        auto point_out = tf_buffer_->transform(*point_msg,"base_link");
        RCLCPP_INFO(this->get_logger(),
            "转换后的坐标点信息: 父级坐标系: %s 坐标值: (%.2f , %.2f , %.2f )",
            point_out.header.frame_id.c_str(),
            point_out.point.x,
            point_out.point.y,
            point_out.point.z
        );
        
        publish_point(point_out);
    }

    //发布坐标函数
    void publish_point(geometry_msgs::msg::PointStamped point){
        //坐标点z轴偏移3.0
        point.header.stamp = this->now();
        point.point.z += 3.0;
        //增加内容: 发布坐标点数据
        point_pub_->publish(point);
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<TFPOintListener>("node_name","namespace"));

    //释放资源
    rclcpp::shutdown();
    return 0;
}