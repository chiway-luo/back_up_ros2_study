#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/nav_sat_fix.hpp"
#include "nav_msgs/msg/path.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"

/*
    需求:订阅gnss消息,将其转换为path消息并发布
    核心:将gnss消息中的经纬度消息转换为path中的米单位
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建订阅方对象gnss
            3-2创建发布方对象path
            3-3创建path消息对象
            // 3-3创建gnss消息对象
            // 3-4创建定时器对象
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/

using namespace std::chrono_literals; //使用时间命名空间
using namespace std::placeholders; //占位符命名空间

class Gnss2Path :public rclcpp::Node{
public:
    Gnss2Path(std::string str1):Node(str1){
        RCLCPP_INFO(this->get_logger(),"namesapce:  node: %s 节点创建成功",str1.c_str());
        //3-1创建订阅方对象gnss
        gnss_sub_ = this->create_subscription<sensor_msgs::msg::NavSatFix>(
            "/gnss/fix",rclcpp::SensorDataQoS(),std::bind(&Gnss2Path::callback_this,this,_1)
        );
        //3-2创建发布方对象path
        path_pub_ = this->create_publisher<nav_msgs::msg::Path>("/gnss/path",10);
        //3-3创建path消息对象
        path_msg_ = std::make_shared<nav_msgs::msg::Path>();
        path_msg_->header.frame_id = "map";//设置path消息头部信息
        //创建pose消息对象(用于转换)
        pose_msg_ = std::make_shared<geometry_msgs::msg::PoseStamped>();
        pose_msg_->header.frame_id = "map";//设置pose消息头部信息
        

    }

private:
    rclcpp::Subscription<sensor_msgs::msg::NavSatFix>::SharedPtr gnss_sub_;//订阅方对象指针
    rclcpp::Publisher<nav_msgs::msg::Path>::SharedPtr path_pub_;//发布方对象指针
    std::shared_ptr<nav_msgs::msg::Path> path_msg_;//path消息对象指针
    std::shared_ptr<geometry_msgs::msg::PoseStamped> pose_msg_;//pose消息对象指针
    bool first_flag_ = true;//第一次标志位 记录原点
    double origin_lat_,origin_lon_,origin_al_;//原点经纬度,海拔高度

    //回调函数
    void callback_this(const sensor_msgs::msg::NavSatFix::SharedPtr msg){
        //记录原点
        if(first_flag_){
            //记录经纬度
            origin_lat_ = msg->latitude;
            origin_lon_ = msg->longitude;
            origin_al_ = msg->altitude;
            first_flag_ = false;
        }
        
        //转换pose数据
        pose_msg_->pose.position.x = (msg->longitude - origin_lon_) * 111319.5;//经度转换为米
        pose_msg_->pose.position.y = (msg->latitude - origin_lat_) * 111319.5;//纬度转换为米
        pose_msg_->pose.position.z = msg->altitude - origin_al_;//高度直接使用
        //设置默认姿态(四元数)
        pose_msg_->pose.orientation.x = 0.0;
        pose_msg_->pose.orientation.y = 0.0;
        pose_msg_->pose.orientation.z = 0.0;
        pose_msg_->pose.orientation.w = 1.0;
        pose_msg_->header.stamp = this->get_clock()->now();
        
        //将pose数据添加到path消息中
        path_msg_->poses.push_back(*pose_msg_);

        //判断数据量是否过多,超过100个则删除最早的
        if(path_msg_->poses.size() > 100){
            path_msg_->poses.erase(path_msg_->poses.begin());
        }
        
        //更新path消息头部时间戳
        path_msg_->header.stamp = this->get_clock()->now();
        //发布path消息
        path_pub_->publish(*path_msg_);

                    
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<Gnss2Path>("Gnss2Path_node_cpp"));//node_name, (namespace可选)

    //释放资源
    rclcpp::shutdown();
    return 0;
}