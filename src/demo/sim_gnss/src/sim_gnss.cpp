#include "rclcpp/rclcpp.hpp"
// #include "sensor_msgs/msg/nav_sat_status.hpp"
#include "sensor_msgs/msg/nav_sat_fix.hpp"

/*
- 位置信息消息类型，包含GNSS定位信息 sensor_msgs/msg/NavSatFix

- 状态消息类型，包含GNSS状态信息 sensor_msgs/msg/NavSatStatus 

    需求:模拟gnss消息并发布(实现圆圈效果)
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建gnss消息发布方
            3-2创建gnss消息对象
            3-3创建定时器对象
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/

using namespace std::chrono_literals; //使用时间命名空间
using namespace std::placeholders; //占位符命名空间

class SimGnss :public rclcpp::Node{
public:
    SimGnss(std::string str1):Node(str1){
        RCLCPP_INFO(this->get_logger(),"namesapce:  node: %s 节点创建成功",str1.c_str());
        //创建gnss消息发布方
        gnss_pub_ = this->create_publisher<sensor_msgs::msg::NavSatFix>("gnss/fix",10);//消息队列长度10
        //创建gnss消息对象
        gnss_msg_ = std::make_shared<sensor_msgs::msg::NavSatFix>();
        //创建定时器对象
        timer_ = this->create_wall_timer(50ms,std::bind(&SimGnss::callback_this,this));
        
        //初始化gnss消息对象
        gnss_msg_->header.frame_id = "gnss_link";//坐标系
        gnss_msg_->status.status = sensor_msgs::msg::NavSatStatus::STATUS_FIX;//有信号
        gnss_msg_->status.service = sensor_msgs::msg::NavSatStatus::SERVICE_COMPASS;//北斗
        gnss_msg_->position_covariance_type = sensor_msgs::msg::NavSatFix::COVARIANCE_TYPE_APPROXIMATED;//协方差类型

        // 协方差（示例：水平0.5m，垂直1.0m）
        double sigma_xy = 0.5;
        double sigma_z  = 1.0;
        gnss_msg_->position_covariance = {
            sigma_xy*sigma_xy, 0.0, 0.0,
            0.0, sigma_xy*sigma_xy, 0.0,
            0.0, 0.0, sigma_z*sigma_z
        };

        //起始时间
        start_time_ = this->now();


    }

private:
    rclcpp::Publisher<sensor_msgs::msg::NavSatFix>::SharedPtr gnss_pub_;//gnss消息发布方对象指针
    std::shared_ptr<sensor_msgs::msg::NavSatFix> gnss_msg_;//gnss消息对象指针
    rclcpp::TimerBase::SharedPtr timer_;//定时器对象指针

    rclcpp::Time start_time_;//起始时间
    double center_lat_deg_ = 39.9042;   // 圆心经度/纬度（可随便改）
    double center_lon_deg_ = 116.4074;
    double altitude_m_     = 50.0;
    double radius_m_ = 10.0;   // 圆半径（m）
    double omega_    = 0.3;    // 角速度（rad/s）

    //回调函数
    //注意单位为 米 度 秒
    void callback_this(){
        //(1)时间
        rclcpp::Time now_t = this->now();
        double t = (now_t - start_time_).seconds();//经过时间 秒
        double theta = omega_ * t; //当前角度 弧度

        //(2)平面圆轨迹
        double east = radius_m_ * std::cos(theta); //东向偏移 米
        double north= radius_m_ * std::sin(theta); //北向偏移 米

        //(3)米->经纬度 (小范围近似)
        constexpr double Re = 6378137.0; //地球半径 米
        double lat0_rad = center_lat_deg_ * M_PI / 180.0; //圆心纬度 弧度
        double cos_lat0 = std::cos(lat0_rad);//圆心纬度余弦值

        if (std::fabs(cos_lat0) < 1e-6) cos_lat0 = 1e-6; // 防止极点附近除0

        double dlat_deg = (north / Re) * (180.0 / M_PI); //纬度偏移 度
        double dlon_deg = (east / (Re * cos_lat0)) * (180.0 / M_PI); //经度偏移 度

        //填充消息并发布
        gnss_msg_->header.stamp = now_t;//时间戳
        gnss_msg_->latitude  = center_lat_deg_ + dlat_deg;//纬度
        gnss_msg_->longitude = center_lon_deg_ + dlon_deg;//经度
        gnss_msg_->altitude  = altitude_m_;//海拔
        
        gnss_pub_->publish(*gnss_msg_);//发布消息

    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<SimGnss>("SimGnss_node_cpp"));//node_name, (namespace可选)

    //释放资源
    rclcpp::shutdown();
    return 0;
}